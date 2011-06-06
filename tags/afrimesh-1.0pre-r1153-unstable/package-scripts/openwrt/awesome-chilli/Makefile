#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=awesome-chilli
PKG_VERSION:=1.2.6
PKG_RELEASE:=2

PKG_SOURCE:=coova-chilli-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ap.coova.org/chilli
PKG_BUILD_DIR:=$(BUILD_DIR)/coova-chilli-$(PKG_VERSION)
PKG_MD5SUM:=3153c200dad514976760dc5176ee8897

PKG_FIXUP:=libtool
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/awesome-chilli
  SUBMENU:=Captive Portals
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-tun +librt
  TITLE:=Wireless LAN HotSpot controller (Afrimesh Version)
  URL:=http://afrimesh.org
endef

define Package/awesome-chilli/description
	A sane OpenWRT packaging for coova-chilli which reduces risk of heart 
	attacks and/or creeping insanity.
endef

define Package/awesome-chilli/conffiles
/etc/chilli.conf
/etc/config/chilli
endef

CONFIGURE_ARGS += \
	--enable-layer3

#CONFIGURE_VARS += \
#LDFLAGS="$$$$LDFLAGS" \
#CFLAGS="" \
#CC="$(filter-out ccache,$(TARGET_CC))"

#define Build/Configure
#	$(call Build/Configure/Default,,bash)
#endef


define Package/awesome-chilli/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_CONF) ./files/etc/chilli.conf   $(1)/etc/
	$(INSTALL_CONF) ./files/etc/config/chilli $(1)/etc/config/
	$(INSTALL_BIN)  ./files/etc/init.d/chilli $(1)/etc/init.d/

	$(INSTALL_DIR) $(1)/etc/chilli
	$(INSTALL_DIR) $(1)/etc/chilli/www
	$(CP) $(PKG_INSTALL_DIR)/etc/chilli/* $(1)/etc/chilli/
	$(INSTALL_BIN)  ./files/etc/chilli/www/config.sh $(1)/etc/chilli/www/

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/chilli* $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so.* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/www
	$(INSTALL_DIR) $(1)/www/uam
	$(CP) ./files/www/uam/index.html $(1)/www/uam/
	$(CP) ./files/www/uam/chilli.js  $(1)/www/uam/
	$(CP) ./files/www/uam/coova.jpg  $(1)/www/uam/
endef

$(eval $(call BuildPackage,awesome-chilli))
