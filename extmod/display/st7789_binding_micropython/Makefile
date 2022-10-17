
define st7789/info
  PKG_NAME:=st7789_binding_micropython
  PKG_SOURCE_URL:=https://github.com/liangyingy/st7789_binding_micropython.git
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=5d40d1f280b0bb264f362905b927b81efb407758
  PKG_SOURCE_MD5:=
endef

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

# PKG_DEPEND:=LilyGo-EPD47
# $(foreach var, $(PKG_DEPEND), make -C $(TOP_DIR)/libs/$(var);)
all:
	$(eval $(st7789/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
	# $(call Package/patches,$(TARGET_BUILD_DIR)/$(PKG_NAME),$(shell pwd)/patches)