
include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

define esp32/info
	PKG_NAME:=esp-idf
	PKG_SOURCE_URL:=https://github.com/espressif/esp-idf.git
	PKG_SOURCE_PROTO:=git
	PKG_SOURCE_VERSION:=v4.0.2
	PKG_SOURCE_MD5:=9416da50831df089fcf60db5fc2750f0
	PKG_SOURCE_OPT:=--recurse-submodules
endef

# $(call DownloadMethod/git,$(PKG_NAME),$(PKG_SOURCE_URL),$(PKG_SOURCE_VERSION)) ;
define esp32/prereq
	$(eval $(esp32/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
endef

##
## $(1) MODULES
##
define esp32/compile
	$(TARGET_BUILD_DIR)/esp-idf/install.sh
	. $(TARGET_BUILD_DIR)/esp-idf/export.sh && \
	make -C $(TARGET_BUILD_DIR)/micropython/ports/esp32 \
			MICROPY_BOARD_DIR=$(TOP_DIR)/target/$(TARGET)/boards/$(BOARD) \
			BOARD=$(BOARD) BUILD=$(TARGET_BUILD_DIR)/$(BOARD) \
			USER_C_MODULES=$(TOP_DIR)/extmod/micropython.cmake \
			EXTMOD_FROZEN_DIR=$(TOP_DIR)/extmod
endef


define esp32/install
	[ -d $(BIN_DIR) ] && mkdir -p $(BIN_DIR)/esp32/$(BOARD)
	img_name=LilyGo-MicroPython_$(TARGET)_$(BOARD) && \
	cd $(TARGET_BUILD_DIR)/micropython && \
	micropython_version=`git show -s --pretty=format:%h` && \
	img_name=$${img_name}_MPY-$${micropython_version} && \
	cd $(TARGET_BUILD_DIR)/$(PKG_NAME) && \
	idf_version=`git show -s --pretty=format:%h` && \
	img_name=$${img_name}_IDF-$${idf_version} && \
	time=`date +"%Y%m%d"` && \
	img_name=$${img_name}_B$${time} && \
	md5=`md5sum $(TARGET_BUILD_DIR)/$(BOARD)/firmware.bin | awk '{print $$1}'` && \
	img_name=$${img_name}_$${md5:0:10} && \
	cp $(TARGET_BUILD_DIR)/$(BOARD)/firmware.bin $(BIN_DIR)/esp32/$(BOARD)/$${img_name}.bin
endef
