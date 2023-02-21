
define framebuf_plus/info
  PKG_NAME:=framebuf-plus
  PKG_SOURCE_URL:=https://github.com/lbuque/framebuf-plus.git
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=12a80666d1c38d1bddeb2261b71f1c622ea7efa5
  PKG_SOURCE_MD5:=
endef

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

# PKG_DEPEND:=LilyGo-EPD47
# $(foreach var, $(PKG_DEPEND), make -C $(TOP_DIR)/libs/$(var);)
all:
	$(eval $(framebuf_plus/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
	# $(call Package/patches,$(TARGET_BUILD_DIR)/$(PKG_NAME),$(shell pwd)/patches)