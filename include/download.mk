
ifdef PKG_SOURCE_VERSION
# PKG_VERSION := $(if $(shell echo $(PKG_VERSION) | grep "[0-9a-f]\{5,40\}"))
PKG_SOURCE_SUBDIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE := $(PKG_SOURCE_SUBDIR).tar.xz
endif

##
## $(1) SUBDIR
## $(2) URL
## $(3) VERSION
## $(4) OPTS
##
define DownloadMethod/git
	[ -d $(TMP_DIR) ] && mkdir -p $(TMP_DIR)/dl && \
	cd $(TMP_DIR)/dl && \
	rm -rf $(1) && \
	[ \! -d $(1) ] && \
	git clone $(4) $(2) $(1) && \
	(cd $(1) && git checkout $(3) && \
	git submodule update --init --recursive) && \
	echo "Packing checkout..." && \
	tar czf $(DL_DIR)/$(1)-$(3).tar.gz $(1)
endef
# tar czf $(DL_DIR)/$(1)-$(3).tar.gz $(1)

define Download
	retry=0 ; \
	while [ $$retry -le 2 ]; \
	do \
		if [ ! -e $(DL_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz ]; then \
			$(call DownloadMethod/git,$(PKG_NAME),$(PKG_SOURCE_URL),$(PKG_SOURCE_VERSION)) ; \
		fi ; \
		retry=`expr $$retry + 1` ; \
	done
endef

