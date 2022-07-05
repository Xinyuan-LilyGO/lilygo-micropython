
define st7789/info
  PKG_NAME:=st7789
  PKG_SOURCE_URL:=https://github.com/russhughes/st7789_mpy.git
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=master
  PKG_SOURCE_MD5:=
endef

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

# PKG_DEPEND:=LilyGo-EPD47
# $(foreach var, $(PKG_DEPEND), make -C $(TOP_DIR)/libs/$(var);)
all:
	$(eval $(st7789/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
	$(call Package/patches,$(TARGET_BUILD_DIR)/$(PKG_NAME),$(shell pwd)/patches)