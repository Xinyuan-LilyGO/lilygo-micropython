SHELL:=/bin/bash

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

define LilyGo-EPD47/info
  PKG_NAME:=LilyGo-EPD47
  PKG_SOURCE_URL:=https://github.com/Xinyuan-LilyGO/LilyGo-EPD47.git
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=esp32s3
  PKG_SOURCE_MD5:=505c09c8fdef57c04180f2b1676ccab8
endef

define LilyGo-EPD47/prereq
	$(eval $(LilyGo-EPD47/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
	$(call Package/patches,$(TARGET_BUILD_DIR)/$(PKG_NAME),$(shell pwd)/patches)
endef

all:
	$(call LilyGo-EPD47/prereq)
