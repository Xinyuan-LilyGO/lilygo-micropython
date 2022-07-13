
include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/package.mk

define circuitpython/info
	PKG_NAME:=circuitpython
	PKG_SOURCE_URL:=https://github.com/adafruit/circuitpython.git
	PKG_SOURCE_PROTO:=git
	PKG_SOURCE_VERSION:=7.3.1
	PKG_SOURCE_MD5:=639c7fcbe62a8252cf9367147d94c150
	# PKG_SOURCE_OPT:=--recurse-submodules --depth=1
endef

define circuitpython/prereq
	$(eval $(circuitpython/info))
	$(call Package/prereq,$(TARGET_BUILD_DIR))
	$(call Package/patches,$(TARGET_BUILD_DIR)/$(PKG_NAME),$(TOP_DIR)/$(PKG_NAME)/patches-$(PKG_SOURCE_VERSION))
endef
