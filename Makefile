SHELL:=/bin/bash

TOP_DIR:=$(shell pwd)
TMP_DIR:=$(TOP_DIR)/tmp
DL_DIR:=$(TOP_DIR)/dl
INCLUDE_DIR:=$(TOP_DIR)/include
BUILD_DIR:=$(TOP_DIR)/build_dir
BIN_DIR:=$(TOP_DIR)/bin

include config

TARGET:=$(if $(CONFIG_TARGET),$(CONFIG_TARGET),esp32)
BOARD:=$(if $(CONFIG_BOARD),$(CONFIG_BOARD),GENERIC_SPIRAM)

TARGET_BUILD_DIR:=$(BUILD_DIR)/$(TARGET)

export TMP_DIR DL_DIR INCLUDE_DIR BUILD_DIR TARGET_BUILD_DIR TOP_DIR SHELL

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
	$(call micropython/prereq)

.PHONY: extmod
extmod:
	$(foreach var, $(shell find extmod -name Makefile), make -C $(shell dirname $(var));)

.PHONY: prereq
prereq:
	@if [ ! -d $(TMP_DIR) ]; then \
		mkdir -p $(TMP_DIR); \
	fi
	@if [ ! -d $(DL_DIR) ]; then \
		mkdir -p $(DL_DIR); \
	fi
	@if [ ! -d $(TARGET_BUILD_DIR) ]; then \
		mkdir -p $(TARGET_BUILD_DIR); \
	fi
	@if [ ! -d $(BIN_DIR) ]; then \
		mkdir -p $(BIN_DIR); \
	fi


.PHONY: clean
clean:
	rm -rf $(TMP_DIR)
	rm -rf $(BIN_DIR)
	rm -rf $(BUILD_DIR)
