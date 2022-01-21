# Location of top-level MicroPython directory
MPY_DIR = ../micropython

# Name of module
MOD = gsm

# Source files (.c or .py)
SRC = modgsm_mpy.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH = xtensawin

CFLAGS += -I. 
CFLAGS += -I$(MPY_DIR)/ports/esp32 
CFLAGS += -I$(MPY_DIR)/esp-idf/components/tcpip_adapter/include/
CFLAGS += -I$(MPY_DIR)/esp-idf/components/freertos/include/
CFLAGS += -I$(MPY_DIR)/ports/esp32/build-GENERIC/config/
CFLAGS += -I$(MPY_DIR)/esp-idf/components/xtensa/include/
CFLAGS += -I$(MPY_DIR)/esp-idf/components/xtensa/esp32/include/
CFLAGS += -I$(MPY_DIR)/esp-idf/components/esp_rom/include/
CFLAGS += -I$(MPY_DIR)/esp-idf/components/soc/esp32/include/
CFLAGS += -I$(MPY_DIR)/esp-idf/components/esp_common/include/
CFLAGS += -I$(MPY_DIR)/esp-idf/components/heap/include/
CFLAGS += -I$(MPY_DIR)/micropython/esp-idf/components/esp32/include/esp_attr.h

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk