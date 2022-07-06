SHELL:=/bin/bash

TOP_DIR:=$(shell pwd)
TMP_DIR:=$(TOP_DIR)/tmp
DL_DIR:=$(TOP_DIR)/dl
INCLUDE_DIR:=$(TOP_DIR)/include
BUILD_DIR:=$(TOP_DIR)/build_dir
BIN_DIR:=$(TOP_DIR)/bin

LOG_LEVEL := $(if $(V),$(V),1)
ifeq ($(LOG_LEVEL),1)
LOG_LEVEL := @
else
LOG_LEVEL :=
endif

include config

TARGET:=$(if $(CONFIG_TARGET),$(CONFIG_TARGET),esp32)
BOARD:=$(if $(CONFIG_BOARD),$(CONFIG_BOARD),GENERIC_SPIRAM)

TARGET_BUILD_DIR:=$(BUILD_DIR)/$(TARGET)

export TMP_DIR DL_DIR INCLUDE_DIR BUILD_DIR TARGET_BUILD_DIR TOP_DIR SHELL LOG_LEVEL

include $(TOP_DIR)/target/$(TARGET).mk
include $(TOP_DIR)/micropython/micropython.mk

# $(foreach var, $(dir), make -C $(shell dirname $(var));)
# make -C $(TOP_DIR)/target/$(TARGET)
# $(call micropython/prereq)

all: prereq micropython extmod
	$(call $(TARGET)/prereq)
	$(call $(TARGET)/compile)
	$(call $(TARGET)/install)

.PHONY: micropython
micropython:
	$(LOG_LEVEL)echo "Prepare micropython source code..."
	$(call micropython/prereq)
	$(LOG_LEVEL)echo "Prepare micropython source code...ok"

.PHONY: extmod
extmod:
	$(LOG_LEVEL)echo "Prepare expansion module..."
	$(foreach var, $(shell find extmod -name Makefile), make -C $(shell dirname $(var));)
	$(LOG_LEVEL)echo "Prepare expansion module...ok"

.PHONY: prereq
prereq:
	$(LOG_LEVEL)echo "Prepare working directory..."
	$(LOG_LEVEL)if [ ! -d $(TMP_DIR) ]; then \
		mkdir -p $(TMP_DIR); \
	fi
	$(LOG_LEVEL)if [ ! -d $(DL_DIR) ]; then \
		mkdir -p $(DL_DIR); \
	fi
	$(LOG_LEVEL)if [ ! -d $(TARGET_BUILD_DIR) ]; then \
		mkdir -p $(TARGET_BUILD_DIR); \
	fi
	$(LOG_LEVEL)if [ ! -d $(BIN_DIR) ]; then \
		mkdir -p $(BIN_DIR); \
	fi
	$(LOG_LEVEL)echo "Prepare working directory...ok"


.PHONY: clean
clean:
	$(LOG_LEVEL)echo "Clear working directory..."
	$(LOG_LEVEL)rm -rf $(TMP_DIR)
	$(LOG_LEVEL)rm -rf $(BIN_DIR)
	$(LOG_LEVEL)rm -rf $(BUILD_DIR)
	$(LOG_LEVEL)echo "Clear working directory...ok"
