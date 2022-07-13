
include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

define nrf/info
	PKG_NAME:=
	PKG_SOURCE_URL:=
	PKG_SOURCE_PROTO:=
	PKG_SOURCE_VERSION:=
	PKG_SOURCE_MD5:=
endef


define nrf/prereq
	# sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
	@echo "Please pre-install gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential"
	@cp $(TOP_DIR)/target/$(TARGET)/boards/$(BOARD)-$(MICROPYTHON_CORE) $(TARGET_BUILD_DIR)/$(MICROPYTHON_CORE)/ports/nrf/boards/ -rf
	#@mv $(TARGET_BUILD_DIR)/$(MICROPYTHON_CORE)/ports/nrf/boards/$(BOARD)-$(MICROPYTHON_CORE) $(TARGET_BUILD_DIR)/$(MICROPYTHON_CORE)/ports/nrf/boards/$(BOARD)
	@if [ $(MICROPYTHON_CORE) = circuitpython ]; then \
		cd $(TARGET_BUILD_DIR)/$(MICROPYTHON_CORE) && make fetch-submodules && pip3 install -r requirements-dev.txt ; \
	fi
endef


define nrf/compile
	make -C $(TARGET_BUILD_DIR)/$(MICROPYTHON_CORE)/mpy-cross
	cd $(TARGET_BUILD_DIR)/$(MICROPYTHON_CORE)/ports/nrf && \
	make BOARD=$(BOARD)-$(MICROPYTHON_CORE) \
			BUILD=$(TARGET_BUILD_DIR)/$(BOARD)-$(MICROPYTHON_CORE) \
			USER_C_MODULES=$(TOP_DIR)/extmod/micropython.cmake \
			EXTMOD_FROZEN_DIR=$(TOP_DIR)/extmod SD=s140
endef


define nrf/install
	@echo "Installing firmware to $(BIN_DIR)/nrf/$(BOARD)"
	@[ -d $(BIN_DIR) ] && mkdir -p $(BIN_DIR)/nrf/$(BOARD)
	@img_name=LilyGo-MicroPython_$(TARGET)_$(BOARD) && \
	cd $(TARGET_BUILD_DIR)/$(MICROPYTHON_CORE) && \
	micropython_version=`git show -s --pretty=format:%h` && \
	if [ $(MICROPYTHON_CORE) = circuitpython ]; then \
		img_name=CPY-$${micropython_version} ; \
	fi && \
	if [ $(MICROPYTHON_CORE) = micropython ]; then \
		img_name=$${img_name}_MPY-$${micropython_version} ; \
	fi && \
	time=`date +"%Y%m%d"` && \
	img_name=$${img_name}_B$${time} && \
	md5=`md5sum $(TARGET_BUILD_DIR)/$(BOARD)-$(MICROPYTHON_CORE)/firmware.bin | awk '{print $$1}'` && \
	img_name=$${img_name}_$${md5:0:10} && \
	if [ $(MICROPYTHON_CORE) = circuitpython ]; then \
		cp $(TARGET_BUILD_DIR)/$(BOARD)-$(MICROPYTHON_CORE)/firmware.uf2 $(BIN_DIR)/nrf/$(BOARD)/$${img_name}.uf2 ; \
	fi && \
	cp $(TARGET_BUILD_DIR)/$(BOARD)-$(MICROPYTHON_CORE)/firmware.hex $(BIN_DIR)/nrf/$(BOARD)/$${img_name}.hex && \
	cp $(TARGET_BUILD_DIR)/$(BOARD)-$(MICROPYTHON_CORE)/firmware.bin $(BIN_DIR)/nrf/$(BOARD)/$${img_name}.bin
endef
