
define lcd/info
  PKG_NAME:=lcd_binding_micropython
  PKG_SOURCE_URL:=https://github.com/liangyingy/lcd_binding_micropython.git
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=59bbbdae5ecc9ae84b986ce54c089250a13cb768
  PKG_SOURCE_MD5:=
endef

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

# PKG_DEPEND:=LilyGo-EPD47
# $(foreach var, $(PKG_DEPEND), make -C $(TOP_DIR)/libs/$(var);)
all:
	$(eval $(lcd/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
	# $(call Package/patches,$(TARGET_BUILD_DIR)/$(PKG_NAME),$(shell pwd)/patches)