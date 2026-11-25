# GEC-BGC — отчёт сборки прошивки (24.11.2026)

## Результат

`GEC_BGC.ino` v0.1.0 собирается без ошибок:

```
Скетч использует 76812 байт (29%) памяти устройства. Всего доступно 262144 байт.
Глобальные переменные используют 7764 байт (15%) динамической памяти, оставляя 41388 байт.
```

Файлы: `build\GEC_BGC.ino.hex` (217 658 Б), `build\GEC_BGC.ino.bin` (77 352 Б). На железе не проверялось (платы нет).

## Окружение

- arduino-cli из Arduino IDE: `C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe`, конфиг `C:\dev\arduino-cli-cfg\arduino-cli.yaml`.
- Ядро STMicroelectronics:stm32 **3.1.0** (установлено ранее для проекта 160).
- Библиотеки (установлены `lib install`): **Simple FOC 2.4.0**, **MPU6050_light 1.2.1** → `C:\Users\Oleg\Documents\Arduino\libraries\`.
- Рабочая копия скетча: `C:\dev\gimbal_fw\GEC_BGC\` (ASCII-путь), копия здесь.

## Команда сборки

```powershell
& "C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe" --config-file C:\dev\arduino-cli-cfg\arduino-cli.yaml compile --fqbn "STMicroelectronics:stm32:GenF1:pnum=GENERIC_F103RCTX,usb=CDCgen,upload_method=swdMethod" --output-dir C:\dev\gimbal_fw\GEC_BGC\build C:\dev\gimbal_fw\GEC_BGC
```

Плата: Generic F103RCTx (256 КБ / 48 КБ), USB CDC как `Serial`. Заливка — ST-Link по SWD (STM32CubeProgrammer) или через USB DFU системного загрузчика нет у F103 (только UART1-bootloader при BOOT0=1).

## Патч SimpleFOC 2.4.0 под ядро STM32duino 3.x

Без патча: `stm32_mcu.cpp:126: error: cannot convert 'TIM_HandleTypeDef*' to 'TIM_TypeDef*'` — в ядре 3.x функция `enableTimerClock()` принимает `TIM_TypeDef*`, а SimpleFOC передаёт handle (своя реализация с handle объявлена только для `__MBED__`).

Патч (`patch_simplefoc.py`, применён к `src/drivers/hardware_specific/stm32/stm32_mcu.cpp`):

```cpp
#if !defined(__MBED__) && defined(STM32_CORE_VERSION) && (STM32_CORE_VERSION >= 0x03000000)
  enableTimerClock(handle->Instance);   // GEC patch: STM32duino 3.x expects TIM_TypeDef*
#else
  enableTimerClock(handle);
#endif
```

При обновлении библиотеки патч накладывать заново (или проверить, не исправлено ли в новой версии SimpleFOC).

## Что внутри v0.1.0

- Пины — по карте STorM32 v1.3 (`..\Pinmap_GIMBAL.md`): M0 PB1/PB0/PA7, M1 PA6/PA3/PA2, M2 PB9/PA1/PB8; nSLEEP PB14, nFAULT PB15, LED PB12/PB13, VBAT PA5; IMU на I2C2 PB10/PB11 (`TwoWire imuWire(PB11, PB10)`).
- 3 × `BLDCDriver3PWM` + `BLDCMotor(7)` в режиме `angle_openloop`, PWM 25 кГц, ограничение 4 В на мотор.
- MPU6050_light: гиро ±500 °/с, акселерометр ±2 g, комплементарный фильтр библиотеки, калибровка смещений при старте.
- Петля 500 Гц: PID по углам pitch/roll/yaw → скорость мотора → угол открытого контура.
- USB-команды: `e1`/`e0` моторы, `p<град>` `r<град>` `y<град>` уставки, `c` калибровка IMU, `s` статус (углы, уставки, VBAT, fault).

Известные упрощения (к v0.2): углы IMU берутся как углы камеры напрямую, без пересчёта в углы рамы и кинематики подвеса; знак/направление моторов не автоопределяется; нет сохранения параметров и RC-входов.
