
include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

define rp2/info
	PKG_NAME:=
	PKG_SOURCE_URL:=
	PKG_SOURCE_PROTO:=
	PKG_SOURCE_VERSION:=
	PKG_SOURCE_MD5:=
endef

# $(call DownloadMethod/git,$(PKG_NAME),$(PKG_SOURCE_URL),$(PKG_SOURCE_VERSION)) ;
define rp2/prereq
	# sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
	@echo "Please pre-install gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential"
endef

##
## $(1) MODULES
##
define rp2/compile
	make -C $(TARGET_BUILD_DIR)/micropython/mpy-cross && \
	cd $(TARGET_BUILD_DIR)/micropython/ports/rp2 && \
	make submodules && \
	make MICROPY_BOARD_DIR=$(TOP_DIR)/target/$(TARGET)/boards/$(BOARD) \
			BOARD=$(BOARD) \
			BUILD=$(TARGET_BUILD_DIR)/build-$(BOARD) \
			USER_C_MODULES=$(TOP_DIR)/extmod/micropython.cmake \
			EXTMOD_FROZEN_DIR=$(TOP_DIR)/extmod && \
	cd -
endef


define rp2/install
	[ -d $(BIN_DIR) ] && mkdir -p $(BIN_DIR)/rp2/$(BOARD)
	img_name=LilyGo-MicroPython_$(TARGET)_$(BOARD) && \
	cd $(TARGET_BUILD_DIR)/micropython && \
	micropython_version=`git show -s --pretty=format:%h` && \
	img_name=$${img_name}_MPY-$${micropython_version} && \
	time=`date +"%Y%m%d"` && \
	img_name=$${img_name}_B$${time} && \
	md5=`md5sum $(TARGET_BUILD_DIR)/build-$(BOARD)/firmware.bin | awk '{print $$1}'` && \
	img_name=$${img_name}_$${md5:0:10} && \
	cp $(TARGET_BUILD_DIR)/build-$(BOARD)/firmware.uf2 $(BIN_DIR)/rp2/$(BOARD)/$${img_name}.uf2
endef
