#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pmacct
PKG_VERSION:=0.12.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.pmacct.net/
PKG_MD5SUM:=564c3db9fdeaeb44e3bb3d958937c18c

PKG_BUILD_DEPENDS:=libpcap

include $(INCLUDE_DIR)/package.mk

define Package/afrimesh-pmacct/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=pmacct (accounting and aggregation tools)
  URL:=http://www.pmacct.net/
endef

define Package/afrimesh-nfacctd/Default
  $(call Package/afrimesh-pmacct/Default)
  TITLE:=NetFlow accounting daemon
endef

define Package/afrimesh-nfacctd
  $(call Package/afrimesh-nfacctd/Default)
endef

define Package/afrimesh-nfacctd-mysql
  $(call Package/afrimesh-nfacctd/Default)
  TITLE+= with MySQL support
  DEPENDS+= +libmysqlclient
endef

define Package/afrimesh-nfacctd-pgsql
  $(call Package/afrimesh-nfacctd/Default)
  TITLE+= with PostreSQL support
  DEPENDS+= +libpq
endef

define Package/afrimesh-nfacctd-sqlite
  $(call Package/afrimesh-nfacctd/Default)
  TITLE+= with SQLite support
  DEPENDS+= +libsqlite3
endef

define Package/afrimesh-pmacctd/Default
  $(call Package/afrimesh-pmacct/Default)
  DEPENDS+= +libpcap +zlib
  TITLE:=IP pcap-based accounting daemon
endef

define Package/afrimesh-pmacctd
  $(call Package/afrimesh-pmacctd/Default)
endef

define Package/afrimesh-pmacctd-mysql
  $(call Package/afrimesh-pmacctd/Default)
  TITLE+= with MySQL support
  DEPENDS+= +libmysqlclient
endef

define Package/afrimesh-pmacctd-pgsql
  $(call Package/afrimesh-pmacctd/Default)
  TITLE+= with PostreSQL support
  DEPENDS+= +libpq
endef

define Package/afrimesh-pmacctd-sqlite
  $(call Package/afrimesh-pmacctd/Default)
  TITLE+= with SQLite support
  DEPENDS+= +libsqlite3
endef

define Package/afrimesh-pmacct-client
  $(call Package/afrimesh-pmacct/Default)
  TITLE:=Command-line client to gather data from the IMT plugin
endef

define Compile/Template

$(PKG_BUILD_DIR)/nfacctd$(1) $(PKG_BUILD_DIR)/pmacctd$(1): $(STAMP_CONFIGURED)
	-$(MAKE) -C $(PKG_BUILD_DIR) distclean
	$(call Build/Configure/Default, \
		--with-pcap-includes="$(STAGING_DIR)/usr/include" \
		--with-pcap-libs="$(STAGING_DIR)/usr/lib" \
		$(2) \
		,\
		LIBS="-lz" \
		,,\
	);
	$(MAKE) -C $(PKG_BUILD_DIR) \
		TARGET_FLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		all
	mv -f $(PKG_BUILD_DIR)/src/nfacctd $(PKG_BUILD_DIR)/nfacctd$(1)
	mv -f $(PKG_BUILD_DIR)/src/pmacctd $(PKG_BUILD_DIR)/pmacctd$(1)

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/nfacctd$(1) $(PKG_BUILD_DIR)/pmacctd$(1)

endef


define Install/Template

define Package/afrimesh-$(1)$(2)/conffiles
/etc/default/$(1)
/etc/$(1).conf
endef

define Package/afrimesh-$(1)$(2)/install
	$(INSTALL_DIR) $$(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(1)$(2) $$(1)/usr/sbin/$(1)
	$(INSTALL_DIR) $$(1)/etc
	$(INSTALL_DATA) ./files/$(1).conf $$(1)/etc/
	$(INSTALL_DIR) $$(1)/etc/default
	$(INSTALL_DATA) ./files/$(1).default $$(1)/etc/default/$(1)
	$(INSTALL_DIR) $$(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(1).init $$(1)/etc/init.d/$(1)
endef

endef

ifneq ($(SDK)$(CONFIG_PACKAGE_afrimesh-nfacctd)$(CONFIG_PACKAGE_afrimesh-pmacctd),)
  define Compile/pmacct/basic
    $(call Compile/Template,, \
	--disable-mysql \
	--disable-pgsql \
	--disable-sqlite3 \
    )
    $(call Install/Template,nfacctd,)
    $(call Install/Template,pmacctd,)
  endef
endif
$(eval $(Compile/pmacct/basic))

ifneq ($(SDK)$(CONFIG_PACKAGE_afrimesh-nfacctd-mysql)$(CONFIG_PACKAGE_afrimesh-pmacctd-mysql),)
  define Compile/pmacct/mysql
    $(call Compile/Template,-mysql, \
	--enable-mysql \
	--with-mysql-includes="$(STAGING_DIR)/usr/include" \
	--with-mysql-libs="$(STAGING_DIR)/usr/lib/mysql" \
    )
    $(call Install/Template,nfacctd,-mysql)
    $(call Install/Template,pmacctd,-mysql)
  endef
endif
$(eval $(Compile/pmacct/mysql))

ifneq ($(SDK)$(CONFIG_PACKAGE_afrimesh-nfacctd-pgsql)$(CONFIG_PACKAGE_afrimesh-pmacctd-pgsql),)
  define Compile/pmacct/pgsql
    $(call Compile/Template,-pgsql, \
	--enable-pgsql \
	--with-pgsql-includes="$(STAGING_DIR)/usr/include" \
	--with-pgsql-libs="$(STAGING_DIR)/usr/lib" \
    )
    $(call Install/Template,nfacctd,-pgsql)
    $(call Install/Template,pmacctd,-pgsql)
  endef
endif
$(eval $(Compile/pmacct/pgsql))

ifneq ($(SDK)$(CONFIG_PACKAGE_afrimesh-nfacctd-sqlite)$(CONFIG_PACKAGE_afrimesh-pmacctd-sqlite),)
  define Compile/pmacct/sqlite
    $(call Compile/Template,-sqlite, \
	--enable-sqlite3 \
	--with-sqlite3-includes="$(STAGING_DIR)/usr/include" \
	--with-sqlite3-libs="$(STAGING_DIR)/usr/lib" \
    )
    $(call Install/Template,nfacctd,-sqlite)
    $(call Install/Template,pmacctd,-sqlite)
  endef
endif
$(eval $(Compile/pmacct/sqlite))

define Build/Configure
endef

define Build/Compile
endef

define Package/afrimesh-pmacct-client/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/pmacct $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,afrimesh-nfacctd))
#$(eval $(call BuildPackage,afrimesh-nfacctd-mysql))
#$(eval $(call BuildPackage,afrimesh-nfacctd-pgsql))
#$(eval $(call BuildPackage,afrimesh-nfacctd-sqlite))
$(eval $(call BuildPackage,afrimesh-pmacctd))
#$(eval $(call BuildPackage,afrimesh-pmacctd-mysql))
#$(eval $(call BuildPackage,afrimesh-pmacctd-pgsql))
#$(eval $(call BuildPackage,afrimesh-pmacctd-sqlite))
$(eval $(call BuildPackage,afrimesh-pmacct-client))
