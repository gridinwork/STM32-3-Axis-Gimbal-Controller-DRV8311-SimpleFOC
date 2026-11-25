/*
 * GEC-BGC — собственная прошивка 3-осевого бесколлекторного подвеса камеры.
 * Плата: GEC GIMBAL (проект 175, плата 08), пин-совместима со STorM32 BGC v1.3x:
 *   STM32F103RCT6, 3x DRV8313, внешний модуль MPU6050 (GY-521) на разъёме IMU (I2C2: PB10 SCL, PB11 SDA).
 * Ядро: STM32duino 3.1.0, плата Generic F103RCTx, USB CDC.
 * Библиотеки: Simple FOC 2.4.0 (открытое синусное управление без датчиков положения), MPU6050_light 1.2.1.
 * Версия 0.1.0 — скелет: 3 мотора, IMU, комплементарный фильтр, PID по осям, команды по USB.
 */
#include <Arduino.h>
#include <Wire.h>
#include <SimpleFOC.h>
#include <MPU6050_light.h>

// ---- Пины (карта STorM32 v1.3, снята из открытой схемы Eagle v130) ----
// Мотор 0 (pitch): A=PB1 TIM3_CH4, B=PB0 TIM3_CH3, C=PA7 TIM3_CH2
// Мотор 1 (roll):  A=PA6 TIM3_CH1, B=PA3 TIM2_CH4, C=PA2 TIM2_CH3
// Мотор 2 (yaw):   A=PB9 TIM4_CH4, B=PA1 TIM2_CH2, C=PB8 TIM4_CH3
#define M0_A PB1
#define M0_B PB0
#define M0_C PA7
#define M1_A PA6
#define M1_B PA3
#define M1_C PA2
#define M2_A PB9
#define M2_B PA1
#define M2_C PB8
#define PIN_DRV_NSLEEP PB14   // AUX0 на STorM32; на нашей плате nSLEEP всех DRV8313 (подтяжка 10k к 3.3 В)
#define PIN_DRV_NFAULT PB15   // AUX1; nFAULT (открытый сток, подтяжка)
#define PIN_LED0 PB12
#define PIN_LED1 PB13
#define PIN_VBAT PA5          // делитель батареи, ADC12_IN5
#define PIN_RC0 PA10          // RC-0 / USART1_RX (S.Bus через инвертор)
#define PIN_RC1 PA9
#define PIN_RC2 PA8
#define IMU_SCL PB10          // разъём IMU (камера), I2C2
#define IMU_SDA PB11

// ---- Параметры ----
static const float VBUS_V = 12.0f;        // напряжение питания моторов (3S)
static const float V_LIMIT = 4.0f;        // ограничение напряжения на мотор (нагрев гимбал-моторов)
static const int   POLE_PAIRS = 7;        // GBM2804 и подобные: 14 полюсов = 7 пар
static const float LOOP_HZ = 500.0f;
static const float KP[3] = {8.0f, 8.0f, 6.0f};
static const float KI[3] = {0.5f, 0.5f, 0.3f};
static const float KD[3] = {0.05f, 0.05f, 0.02f};

TwoWire imuWire(IMU_SDA, IMU_SCL);
MPU6050 mpu(imuWire);

BLDCMotor motor[3] = { BLDCMotor(POLE_PAIRS), BLDCMotor(POLE_PAIRS), BLDCMotor(POLE_PAIRS) };
BLDCDriver3PWM driver[3] = {
  BLDCDriver3PWM(M0_A, M0_B, M0_C),
  BLDCDriver3PWM(M1_A, M1_B, M1_C),
  BLDCDriver3PWM(M2_A, M2_B, M2_C)
};

float target_deg[3] = {0, 0, 0};   // уставки pitch, roll, yaw (градусы, yaw — относительно старта)
float motor_angle[3] = {0, 0, 0};  // электромеханический угол мотора (рад), открытый контур
float err_i[3] = {0, 0, 0};
float err_prev[3] = {0, 0, 0};
float yaw_ref = 0;
bool motors_on = false;
uint32_t t_last = 0;

static void motorsEnable(bool on) {
  motors_on = on;
  digitalWrite(PIN_DRV_NSLEEP, on ? HIGH : LOW);
  for (int i = 0; i < 3; i++) { if (on) motor[i].enable(); else motor[i].disable(); }
}

static void handleCommand(const String& s) {
  if (s.length() == 0) return;
  char c = s[0];
  float v = s.substring(1).toFloat();
  switch (c) {
    case 'e': motorsEnable(v != 0); Serial.printf("motors %s\n", motors_on ? "ON" : "OFF"); break;
    case 'p': target_deg[0] = v; break;
    case 'r': target_deg[1] = v; break;
    case 'y': target_deg[2] = v; break;
    case 'c': mpu.calcOffsets(true, true); yaw_ref = mpu.getAngleZ(); Serial.println("IMU calibrated"); break;
    case 's':
      Serial.printf("P %.1f R %.1f Y %.1f | tgt %.1f %.1f %.1f | vbat %.2f | fault %d\n",
        mpu.getAngleX(), mpu.getAngleY(), mpu.getAngleZ() - yaw_ref,
        target_deg[0], target_deg[1], target_deg[2],
        analogRead(PIN_VBAT) * (3.3f / 1023.0f) * 11.0f, digitalRead(PIN_DRV_NFAULT) == LOW);
      break;
    default: Serial.println("cmd: e0/e1 p<deg> r<deg> y<deg> c s");
  }
}

void setup() {
  pinMode(PIN_LED0, OUTPUT); pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_DRV_NSLEEP, OUTPUT); digitalWrite(PIN_DRV_NSLEEP, LOW);
  pinMode(PIN_DRV_NFAULT, INPUT_PULLUP);
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 3000) {}
  Serial.println("GEC-BGC v0.1.0");

  imuWire.begin();
  imuWire.setClock(400000);
  byte st = mpu.begin(1, 0);   // gyro ±500 dps, accel ±2 g
  Serial.printf("MPU6050 begin: %d (0 = OK)\n", st);
  if (st == 0) { delay(500); mpu.calcOffsets(true, true); yaw_ref = mpu.getAngleZ(); }

  for (int i = 0; i < 3; i++) {
    driver[i].voltage_power_supply = VBUS_V;
    driver[i].voltage_limit = V_LIMIT;
    driver[i].pwm_frequency = 25000;
    driver[i].init();
    motor[i].linkDriver(&driver[i]);
    motor[i].voltage_limit = V_LIMIT;
    motor[i].controller = MotionControlType::angle_openloop;
    motor[i].velocity_limit = 20.0f;      // рад/с, ограничение скорости в открытом контуре
    motor[i].init();
    motor[i].disable();
  }
  t_last = micros();
}

void loop() {
  // команды по USB
  static String line;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') { handleCommand(line); line = ""; }
    else if (line.length() < 32) line += c;
  }

  uint32_t now = micros();
  float dt = (now - t_last) * 1e-6f;
  if (dt < 1.0f / LOOP_HZ) { for (int i = 0; i < 3; i++) motor[i].move(motor_angle[i]); return; }
  t_last = now;

  mpu.update();   // комплементарный фильтр внутри библиотеки (0.98 гиро / 0.02 акселерометр)
  float meas[3] = { mpu.getAngleX(), mpu.getAngleY(), mpu.getAngleZ() - yaw_ref };

  for (int i = 0; i < 3; i++) {
    float err = target_deg[i] - meas[i];
    err_i[i] = constrain(err_i[i] + err * dt, -50.0f, 50.0f);
    float d = (err - err_prev[i]) / dt;
    err_prev[i] = err;
    float u_deg_s = KP[i] * err + KI[i] * err_i[i] + KD[i] * d;   // требуемая скорость мотора, град/с
    u_deg_s = constrain(u_deg_s, -300.0f, 300.0f);
    if (motors_on) motor_angle[i] += u_deg_s * DEG_TO_RAD * dt;   // интегрируем в угол открытого контура
    motor[i].move(motor_angle[i]);
  }

  static uint32_t blink = 0;
  if (millis() - blink > (motors_on ? 200u : 1000u)) { blink = millis(); digitalWrite(PIN_LED0, !digitalRead(PIN_LED0)); }
}
