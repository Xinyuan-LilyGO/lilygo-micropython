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
  - [3.6 T-Display](#t-display)
  - [3.7 T-DisplayS3](#t-displays3)
  - [3.8 T-RGB](#t-rgb)
  - [3.9 T7-S3](#t7-s3)
  - [3.10 T-DisplayS3-AMOLED](#t-displays3-amoled)
- [4. Modules](#modules)
  - [4.1 Display Module](#display-module)
- [5. Interesting Project](#interesting-project)
- [6. Issue](#issue)
- [7. Future plans](#future-plans)

---

## Install Prerequisites

To compile with lilygo-micropython you need to get the following packages. The command to run depends on which distribution of Linux you are using:

- Ubuntu and Debian:

```
sudo apt-get install git make python3 python3-pip cmake quilt libusb-1.0-0-dev libusb-dev libudev-dev
```

## Supported Boards

- LILYGO T-SIM7000G
- LILYGO T5-4.7
- LILYGO T-T-CALL SIM800
- LILYGO T-PicoC3
- LILYGO T5-4.7 Plus (esp32s3)
- LILYGO T-Echo
- LILYGO T-Display
- LILYGO T-DisplayS3
- LILYGO T-RGB
- LILYGO T7-S3
- LILYGO T-DisplayS3-AMOLED

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

### T-Display

```shell
$ cp config_T-Display config
$ make
```

### T-DisplayS3

```shell
$ cp config_T-DisplayS3 config
$ make
```

### T-RGB

```shell
$ cp config_T-RGB config
$ make
```

### T7-S3

```shell
$ cp config_T7-S3 config
$ make
```

### T-DisplayS3-AMOLED

```shell
$ cp config_T-DisplayS3-AMOLED config
$ make
```

## Modules

### Display Module

- [epd](./extmod/display/epd/README.md): [LilyGo-EPD47](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47) using in micropython.
- [framebuf1](./extmod/display/framebuf1/): This module provides a general frame buffer which can be used to epd module.
- [st7789](./extmod/display/st7789/): Fast MicroPython driver for ST7789 display module written in C.
- [bma](./extmod/sensor/bma/): micropython driver library for bma423
- [lcd](./extmod/display/lcd/): Driver library supporting I8080 and RGB interface
- [framebuf_plus](extmod/display/framebuf_plus): Added gfx font support on the basis of [framebuf](https://docs.micropython.org/en/latest/library/framebuf.html).

## Interesting Project

[LilyGo-EPD-4-7-OWM-Weather-Display](https://github.com/Xinyuan-LilyGO/LilyGo-EPD-4-7-OWM-Weather-Display/tree/web/micropython): Using the LilyGo EPD 4.7" display to show OWM Weather Data

## Issue

### 1. Compilation error caused by ninja

    Compilation gives the following error:

    ```
    ninja: build stopped: subcommand failed.
    ninja failed with exit code 1
    make[1]: *** [Makefile:42: all] Error 2
    make[1]: Leaving directory '/home/scruss/c/lilygo-micropython/build_dir/esp32s3/micropython/ports/esp32'
    make: *** [Makefile:36: all] Error 2
    ```

    Please uninstall ninja and try to compile again

    ```shell
    sudo apt remove ninja-build
    rm build_dir -rf
    ```

## Future plans

- Support menuconfig
- More boards
- Complete development documentation
