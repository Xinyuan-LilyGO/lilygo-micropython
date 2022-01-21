
define epd/info
  PKG_NAME:=epd
  PKG_DEPEND:=LilyGo-EPD47
endef

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

# PKG_DEPEND:=LilyGo-EPD47
# $(foreach var, $(PKG_DEPEND), make -C $(TOP_DIR)/libs/$(var);)
all:
	$(eval $(epd/info))
	$(call Package/depend)