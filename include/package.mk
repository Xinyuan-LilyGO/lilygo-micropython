
##
## $(1) TARGET_BUILD_DIR
##
define Package/prereq
	$(LOG_LEVEL)if [ -e $(1)/$(PKG_NAME) ]; then \
		if [ \! -e $(1)/$(PKG_NAME)/prereq ]; then \
			rm -rf $(1)/$(PKG_NAME) ; \
			if [ \! -e $(DL_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz ]; then \
				$(call Download) ; \
			fi ;\
			echo "unpack $(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz" ; \
			tar -xf $(DL_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz -C $(1) ; \
			touch $(1)/$(PKG_NAME)/prereq ; \
		fi ; \
	else \
		if [ \! -e $(DL_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz ]; then \
			$(call Download) ; \
		fi ; \
		echo "unpack $(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz" ; \
		tar -xf $(DL_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz -C $(1) ; \
		touch $(1)/$(PKG_NAME)/prereq ; \
	fi
endef

##
##
##
define Package/depend
	$(foreach var, $(PKG_DEPEND), make -C $(TOP_DIR)/libs/$(var);)
endef

##
## $(1) TARGET_DIR
## $(2) PATCH_DIR
##
##
define Package/patches
	$(LOG_LEVEL)echo "Push patch on $(1)" && \
	cd $(1) ; \
	quilt import $(2)/* ; \
	quilt push -a ; \
	cd - ; \
	for patch in `ls $(2)` ; \
	do \
		if [ ! -e $(1)/prereq_$$patch ]; then \
			touch $(1)/prereq_$$patch ; \
		fi ; \
	done
endef
