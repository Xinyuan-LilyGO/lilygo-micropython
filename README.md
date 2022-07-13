# LilyGo MicroPython

# Index

- [1. Install Prerequisites](#install-prerequisites)
- [2. Supported Boards](#supported-boards)
- [3. Build](#build)
  - [3.1 T-SIM7000G](#t-sim7000g)
  - [3.1 T5-4.7](#t5-47)
  - [3.2 T-CALL SIM800](#t-call-sim800)
  - [3.3 T-PicoC3](#t-picoc3)
  - [3.4 T5-4.7 Plus](#t5-47-plus)
  - [3.5 T-Echo](#t-echo)
- [4. Modules](#modules)
  - [4.1 Display Module](#display-module)
- [5. Interesting Project](#interesting-project)
- [6. Future plans](#future-plans)

---

## Install Prerequisites

To compile with lilygo-micropython you need to get the following packages. The command to run depends on which distribution of Linux you are using:

- Ubuntu and Debian:

```
sudo apt-get install git make python3 python3-pip cmake quilt
```

## Supported Boards

- LilyGo T-SIM7000G
- LilyGo T5-4.7
- LilyGo T-T-CALL SIM800
- LilyGo T-PicoC3
- LilyGo T5-4.7 Plus (esp32s3)
- LilyGo T-Echo

## Build

### T-SIM7000G

```shell
$ cp config_T-SIM7000G config
$ make
```

### T5-4.7

```shell
$ cp config_T5-4.7 config
$ make
```

### T-CALL SIM800

```shell
$ cp config_T-CALL config
$ make
```

### T-PicoC3

```shell
$ cp config_T-PicoC3 config
$ sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
$ make
```

### T5-4.7 Plus

```shell
$ cp config_T5-4.7-Plus config
$ make
```

### T-Echo

```shell
$ cp config_T-Echo config
$ make
```

> Use circuitpython by default

To use micropython, you need to modify the config file:

```diff
--- config_T-Echo       2022-07-13 17:11:04.374660308 +0800
+++ config      2022-07-13 17:11:00.694565940 +0800
@@ -1,3 +1,3 @@
 CONFIG_TARGET=nrf
 CONFIG_BOARD=LILYGO_T-Echo
-CONFIG_MICROPYTHON_CORE=circuitpython
+# CONFIG_MICROPYTHON_CORE=circuitpython
```

## Modules

### Display Module

- [epd](./extmod/display/epd/README.md): [LilyGo-EPD47](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47) using in micropython.
- [framebuf1](./extmod/display/framebuf1/): This module provides a general frame buffer which can be used to epd module.
- [st7789](./extmod/display/st7789/): Fast MicroPython driver for ST7789 display module written in C.

## Interesting Project

[LilyGo-EPD-4-7-OWM-Weather-Display](https://github.com/Xinyuan-LilyGO/LilyGo-EPD-4-7-OWM-Weather-Display/tree/web/micropython): Using the LilyGo EPD 4.7" display to show OWM Weather Data

## Future plans

- Support menuconfig
- More boards
- Complete development documentation
