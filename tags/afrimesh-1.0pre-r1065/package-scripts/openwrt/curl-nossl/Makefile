# 
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=curl
PKG_VERSION:=7.19.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://curl.haxx.se/download/ \
	http://www.mirrorspace.org/curl/ \
	http://curl.mirror.internet.tp/download/ \
	ftp://ftp.sunet.se/pub/www/utilities/curl/ \
	ftp://ftp.planetmirror.com/pub/curl/ \
	http://www.mirrormonster.com/curl/download/ \
	http://curl.mirrors.cyberservers.net/download/
PKG_MD5SUM:=8402c1f654c51ad7287aad57c3aa79be

PKG_FIXUP = libtool

include $(INCLUDE_DIR)/package.mk

define Package/curl-nossl/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://curl.haxx.se/
endef

define Package/curl-nossl
  $(call Package/curl-nossl/Default)
  DEPENDS:=
  TITLE:=A client-side URL transfer library
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--disable-shared \
	--enable-static \
	--disable-thread \
	--disable-cookies \
	--disable-crypto-auth \
	--enable-nonblocking \
	--disable-file \
	--disable-ftp \
	--enable-http \
	--disable-ares \
	--disable-debug \
	--disable-dict \
	--disable-gopher \
	--disable-ldap \
	--disable-manual \
	--disable-sspi \
	--disable-telnet \
	--disable-tftp \
	--disable-verbose \
	--without-random \
	--without-ssl \
	--without-ca-bundle \
	--without-gnutls \
	--without-krb4 \
	--without-libidn \
	--without-nss \
	--without-libssh2 \
	--without-zlib \

CONFIGURE_VARS += \
	LDFLAGS="$$$$LDFLAGS" \
	CFLAGS="-static -DCURL_STATICLIB" \
	CC="$(filter-out ccache,$(TARGET_CC))"

define Build/Configure
	$(call Build/Configure/Default,,bash)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC="$(TARGET_CC)" \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin $(1)/usr/bin $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/curl-config $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/curl $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libcurl.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-L$$$${exec_prefix}/lib,,g' $(1)/usr/bin/curl-config
	$(SED) 's#$(TARGET_LDFLAGS)##g' $(1)/usr/lib/pkgconfig/libcurl.pc
	ln -sf $(STAGING_DIR)/usr/bin/curl-config $(2)/bin/
endef

define Package/curl-nossl/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/curl $(1)/usr/bin/curl-nossl
endef

$(eval $(call BuildPackage,curl-nossl))

