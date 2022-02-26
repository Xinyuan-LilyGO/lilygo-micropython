# LilyGo MicroPython

## Install Prerequisites

To compile with lilygo-micropython you need to get the following packages. The command to run depends on which distribution of Linux you are using:

- Ubuntu and Debian:

```
sudo apt-get install git make python3 python3-pip cmake
```

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

## Supported Boards

- TTGO T-SIM7000G
- TTGO T5-4.7

## Modules

### Display Module

[LilyGo-EPD47 using in micropython](./extmod/display/epd/README.md)