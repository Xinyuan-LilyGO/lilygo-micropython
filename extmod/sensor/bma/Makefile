
define bma/info
  PKG_NAME:=bma
  PKG_SOURCE_URL:=https://github.com/liangyingy/bma_binding_micropython.git
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=master
  PKG_SOURCE_MD5:=
  PKG_SOURCE_OPT:=--depth 1 --branch master
endef

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

# PKG_DEPEND:=LilyGo-EPD47
# $(foreach var, $(PKG_DEPEND), make -C $(TOP_DIR)/libs/$(var);)
all:
	$(eval $(bma/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
	$(call Package/patches,$(TARGET_BUILD_DIR)/$(PKG_NAME),$(shell pwd)/patches)