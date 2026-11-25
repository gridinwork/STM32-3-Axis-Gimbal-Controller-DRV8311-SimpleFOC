# Пин-карта платы GIMBAL (STM32F103RCT6, совместимость со STorM32 BGC v1.3x)

Источник: открытая схема **STorM32 BGC v1.30** (Eagle 6.5, `reference_STorM32_v130\STorM32 BGC v130.sch`, TAPR OHL), нетлист снят скриптом `eagle_pins.py` 24.11.2026 (`pinmap_v130_from_eagle.txt`). По wiki автора, «The pins of the v1.31 board are identical to that of the v1.3 board» — v1.31/v1.32 (с DRV8313) используют ту же карту. Сверка: мотор 0 на PB1/PB0/PA7 совпадает с кодом в теме SimpleFOC «Code adaptation for storm32 BGC v1.31».

## STM32F103RCT6 (LQFP-64)

| Вывод | Пин | Сигнал GEC-BGC | У STorM32 | Таймер / периферия | Примечание |
|---|---|---|---|---|---|
| 5/6 | PD0/PD1 | OSC_IN/OUT | кварц 8 МГц | HSE | 2 × 18 пФ при CL = 12 пФ |
| 7 | NRST | RESET | SW1 | — | кнопка + 100 нФ |
| 8 | PC0 | POT0 | POT0 | ADC12_IN10 | до 3.3 В |
| 9 | PC1 | POT1 | POT1 | ADC12_IN11 | |
| 10 | PC2 | POT2 | POT2 | ADC12_IN12 | |
| 11 | PC3 | BUT | BUT | ADC12_IN13 | кнопка-аналог |
| 14 | PA0 | IR | IR | TIM2_CH1/TIM5_CH1 | ИК-светодиод пульта камеры (опция) |
| 15 | PA1 | **M2_B** | B2 | TIM2_CH2 | мотор 2 (yaw), фаза B |
| 16 | PA2 | **M1_C** | C1 | TIM2_CH3 | мотор 1 (roll), фаза C |
| 17 | PA3 | **M1_B** | B1 | TIM2_CH4 | мотор 1, фаза B |
| 21 | PA5 | VBAT_SENSE | LIPO | ADC12_IN5 | делитель 47 кОм / 10 кОм (как v1.3) |
| 22 | PA6 | **M1_A** | A1 | TIM3_CH1 | мотор 1, фаза A |
| 23 | PA7 | **M0_C** | C0 | TIM3_CH2 | мотор 0 (pitch), фаза C |
| 24 | PC4 | SBUS_INV | XOR | GPIO | управление инвертором RC0 (XOR 1G86) |
| 26 | PB0 | **M0_B** | B0 | TIM3_CH3 | мотор 0, фаза B |
| 27 | PB1 | **M0_A** | A0 | TIM3_CH4 | мотор 0, фаза A |
| 28 | PB2 | BOOT1 | BOOT1 | — | 10 кОм на GND |
| 29 | PB10 | IMU_SCL | SCL (разъём «I2C», IMU камеры) | I2C2_SCL | **модуль GY-521** |
| 30 | PB11 | IMU_SDA | SDA | I2C2_SDA | |
| 33 | PB12 | LED0 | LED0 | GPIO | зелёный |
| 34 | PB13 | LED1 | LED1 | GPIO | красный |
| 35 | PB14 | DRV_nSLEEP | AUX0 | GPIO | общий nSLEEP трёх DRV8311, подтяжка 10 кОм к 3.3 В (для STorM32 — всегда включено) |
| 36 | PB15 | DRV_nFAULT | AUX1 | GPIO | nFAULT трёх драйверов, монтажное ИЛИ, подтяжка 10 кОм |
| 37 | PC6 | RC2_0 | RC2-0 | TIM8_CH1 / TIM3_CH1 | входы RC PWM |
| 38 | PC7 | RC2_1 | RC2-1 | TIM8_CH2 | |
| 39 | PC8 | RC2_2 | RC2-2 | TIM8_CH3 | |
| 40 | PC9 | RC2_3 | RC2-3 | TIM8_CH4 | |
| 41 | PA8 | RC2 | RC2 | TIM1_CH1 | RC-2 / Sum-PPM |
| 42 | PA9 | RC1 / USART1_TX | RC1/TX | USART1 | |
| 43 | PA10 | RC0 / USART1_RX | RC0/RX | USART1 | S.Bus через XOR, Spektrum |
| 44 | PA11 | USB_DM | USB-DM | USB | |
| 45 | PA12 | USB_DP | USB-DP | USB | подтяжка 1.5 кОм через PNP, управляется PB5 |
| 46 | PA13 | SWDIO | SWDIO | SWD | |
| 49 | PA14 | SWCLK | SWCLK | SWD | |
| 50 | PA15 | AUX2 | AUX2 | GPIO | свободный на разъём AUX |
| 51 | PC10 | UART4_TX | TX | UART4 | разъём UART (115200) |
| 52 | PC11 | UART4_RX | RX | UART4 | |
| 57 | PB5 | USB_PULLUP_EN | USB-D | GPIO | включение подтяжки D+ (T2 PNP, R 1.5 кОм) |
| 58 | PB6 | IMU2_SCL | SCL#2 (встроенный MPU + разъём I2C#2) | I2C1_SCL | второй IMU / компас (опция) |
| 59 | PB7 | IMU2_SDA | SDA#2 | I2C1_SDA | |
| 60 | BOOT0 | BOOT0 | SW2 | — | кнопка к 3.3 В, 10 кОм на GND |
| 61 | PB8 | **M2_C** | C2 | TIM4_CH3 | мотор 2, фаза C |
| 62 | PB9 | **M2_A** | A2 | TIM4_CH4 | мотор 2, фаза A |
| 1, 13, 19, 32, 48, 64 | VBAT, VDDA, VDD | 3.3 В | | | 100 нФ у каждого + 10 мкФ; VDDA через бусину |

Свободные пины (на тестовые площадки): PB3, PB4 (JTAG, можно GPIO), PC5, PC12, PC13–PC15 (LSE не нужен), PD2.

## Моторы и DRV8311H (×3)

Каждый мотор — свой DRV8311H в режиме **3x PWM**:

| Вывод DRV8311H | Подключение |
|---|---|
| INHA / INHB / INHC | PWM фаз A/B/C с STM32 (пины выше) |
| INLA / INLB / INLC | все три → AVDD этого драйвера (фазы всегда активны; таблица 8-4 даташита: INLx=1, INHx=PWM) |
| MODE | → AVDD: 3x PWM, порог OCP 5 А (Hi-Z даёт 3x PWM с OCP 9 А — лишнее для гимбал-моторов). Защёлкивается при включении |
| SLEW | Hi-Z или 47 кОм на GND (средняя скорость фронтов, меньше помех на IMU) |
| GAIN | → AGND: 0.25 В/А; выходы SOA/SOB/SOC — на тестовые площадки (в v1 не читаются: свободных АЦП нет) |
| CSAREF | → AVDD + 100 нФ |
| nSLEEP | общий DRV_nSLEEP (PB14) с подтяжкой 10 кОм к 3.3 В; внутри пулдаун 230–300 кОм, поэтому без подтяжки драйвер спит |
| nFAULT | открытый сток → общая линия DRV_nFAULT (PB15), подтяжка 10 кОм к 3.3 В |
| VM, VIN_AVDD | VBAT (2S–4S, 6…17 В; максимум 20 В по даташиту) — 100 нФ + 10 мкФ 25 В у каждого драйвера, 100 нФ VM–CP |
| AVDD | 1 мкФ 6.3 В на AGND (встроенный LDO 3.3 В, до 100 мА) |
| OUTA/B/C | разъём мотора JST-XH 3p |
| Thermal pad | PGND, 4–6 переходных отверстий в полигон земли |

Логические входы: VIH ≥ 1.65 В, 3.3 В совместимы. Задержка 0.5–1.5 мкс, мёртвое время 0.4–1.2 мкс (по SLEW). PWM до 200 кГц; GEC-BGC использует 25 кГц, STorM32 — свои значения.

## Разъёмы

| Разъём | Тип | Контакты | Назначение |
|---|---|---|---|
| BAT | XT30 или винтовая 2p | VBAT, GND | 2S–4S Li-ion/LiPo (6–17 В) |
| MOT0 / MOT1 / MOT2 | JST-XH 3p ×3 | A, B, C | pitch / roll / **yaw — только MOT2** (требование STorM32) |
| IMU (камера) | JST-GH 4p | 3V3, GND, SCL (PB10), SDA (PB11) | модуль GY-521 на проводе ≤ 30 см; VCC модуля от 3.3 В (на GY-521 есть свой LDO, работает и от 3.3 В, и от 5 В) |
| IMU2 | JST-GH 4p | 3V3, GND, SCL (PB6), SDA (PB7) | второй IMU на раме / компас (опция) |
| RC | JST-GH 4p | GND, RC0, RC1, RC2 | PWM / S.Bus (RC0) / PPM (RC2); 5 В-толерантные |
| RC2 | 2.54 мм 5p | GND, RC2-0…RC2-3 | ещё 4 входа PWM |
| POT | 2.54 мм 5p | GND, POT0, POT1, POT2, 3V3 | потенциометры/джойстик, ≤ 3.3 В |
| BUT / AUX | 2.54 мм 2p / 4p | GND, BUT; GND, AUX0*, AUX1*, AUX2 | *AUX0/AUX1 заняты nSLEEP/nFAULT — на разъём выводить только AUX2 |
| UART | JST-GH 4p | 5V, TX (PC10), RX (PC11), GND | телеметрия/MAVLink к FC |
| USB | USB-C | | конфигурация, прошивка (GEC-BGC по CDC; STorM32 через GUI) |
| SWD | 2.54 мм 4p | SWDIO, GND, SWCLK, 3V3 | ST-Link |
| 5V OUT | в составе RC/UART | | питание приёмника, до 1 А |

## Питание

VBAT → защита (P-MOSFET от переполюсовки, как T1 SI4835 у v1.3, TVS SMAJ18A) → **VM драйверов напрямую** и → TPS54202 (5 В, 2 А, 500 кГц, L 10 мкГн) → XC6220 (3.3 В для MCU, IMU, логики). Токи: моторы до 3 × 1.5 А импульсно, логика < 150 мА. Полигоны VBAT/GND, развязка IMU-разъёма от силовой части.
