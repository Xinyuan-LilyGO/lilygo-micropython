USB_VID = 0x239A
USB_PID = 0x0029
USB_PRODUCT = "T-Echo"
USB_MANUFACTURER = "LILYGO"

MCU_CHIP = nrf52840

# INTERNAL_FLASH_FILESYSTEM = 1

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = "MX25R1635F,ZD25WQ16B"

CIRCUITPY_ALARM = 1

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_datetime
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_GPS
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BME280
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_framebuf
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_EPD
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_RFM9x
