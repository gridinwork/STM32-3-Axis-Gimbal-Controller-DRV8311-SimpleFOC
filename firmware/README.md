# Firmware

## GEC-BGC (own firmware, v0.1.0)

`GEC_BGC/GEC_BGC.ino` - skeleton firmware: 3 FOC motors (SimpleFOC), MPU-6050 IMU, attitude filter, PID, USB serial commands. Builds without errors (76.8 KB, 29 % of flash). **Not tested on hardware.**

Toolchain: Arduino + STM32duino core **3.1.0**, board *Generic F103RCTx* (USB CDC as `Serial`), libraries **Simple FOC 2.4.0** and **MPU6050_light 1.2.1**. SimpleFOC 2.4.0 needs a small patch for core 3.x - see `patch_simplefoc.py` and `BUILD_REPORT.md` (Russian, contains the exact command line). Pre-built `build/GEC_BGC.ino.hex` / `.bin` are included.

## STorM32 (alternative, not included)

The board is pin-compatible with **STorM32 BGC v1.3x**, so the free ready-made firmware **o323BGC** by OlliW can run on it. It is closed-source and may not be redistributed in modified form, so it is **not** included here - download it from the author: https://github.com/olliw42/storm32bgc (folder `firmware binaries & gui`), target for board v1.30 (STM32F103RC). Notes for this board: yaw motor on MOT2, no onboard second IMU (disable it in the GUI), AUX0/AUX1 are used by the driver signals.

`reference/pinmap_STorM32_v130.txt` is the STM32 pin assignment extracted from the open STorM32 v1.30 schematic.
