
define lcd/info
  PKG_NAME:=lcd_binding_micropython
  PKG_SOURCE_URL:=https://github.com/liangyingy/lcd_binding_micropython.git
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=d822b4239e72475b1ca18f40a1c47a2fe5ed8c53
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
