# 
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=afrimesh-olsrd
#PKG_REV:=018bb250e33a9c5605577b00f090e28ba3f779b7
#PKG_VERSION:=20100825
#PKG_INSTALL:=1
PKG_VERSION=0.6.0
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
#PKG_SOURCE_URL:=git://olsr.org/olsrd.git
#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
#PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE:=olsrd-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.olsr.org/releases/0.6
PKG_SOURCE_SUBDIR:=olsrd-$(PKG_VERSION)
PKG_MD5SUM:=8425a10e315be5e061143ac310f2424b

PKG_BUILD_DIR:=olsrd-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/afrimesh-olsrd/template
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Saverio Proto <zioproto@gmail.com>
  TITLE:=OLSR (Optimized Link State Routing) daemon
  URL:=http://www.olsr.org/
endef

define Package/afrimesh-olsrd
  $(call Package/afrimesh-olsrd/template)
  MENU:=1
  DEPENDS:=+libpthread
endef

define Package/afrimesh-olsrd/conffiles
/etc/config/olsrd
endef

define Package/afrimesh-olsrd-mod-arprefresh
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Kernel ARP cache refresh plugin
endef

define Package/afrimesh-olsrd-mod-dot-draw
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Dot topology information plugin
endef

define Package/afrimesh-olsrd-mod-json-draw
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=JSON topology information plugin
endef

define Package/afrimesh-olsrd-mod-bmf
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd +kmod-tun
  TITLE:=Basic multicast forwarding plugin
endef

define Package/afrimesh-olsrd-mod-dyn-gw
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Dynamic internet gateway plugin
endef

define Package/afrimesh-olsrd-mod-dyn-gw-plain
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Dynamic internet gateway plain plugin
endef

define Package/afrimesh-olsrd-mod-httpinfo
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Small informative web server plugin
endef

define Package/afrimesh-olsrd-mod-mdns
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Multicast DNS plugin
endef

define Package/afrimesh-olsrd-mod-nameservice
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Lightweight hostname resolver plugin
endef

define Package/afrimesh-olsrd-mod-p2pd
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Peer to Peer Discovery plugin
endef

define Package/afrimesh-olsrd-mod-quagga
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Quagga plugin
endef

define Package/afrimesh-olsrd-mod-secure
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Message signing plugin to secure routing domain
endef

define Package/afrimesh-olsrd-mod-txtinfo
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Small informative web server plugin
endef

define Package/afrimesh-olsrd-mod-watchdog
  $(call Package/afrimesh-olsrd/template)
  DEPENDS:=afrimesh-olsrd
  TITLE:=Watchdog plugin
endef

define Package/afrimesh-olsrd-mod-secure/conffiles
/etc/olsrd.d/olsrd_secure_key
endef

MAKE_FLAGS+= \
	NO_DEBUG_MESSAGES=1 \
	OS="linux" \
	DESTDIR="$(PKG_INSTALL_DIR)" \
	STRIP="true" \
	INSTALL_LIB="true" \
	SUBDIRS="arprefresh bmf dot_draw json_draw dyn_gw dyn_gw_plain httpinfo mdns nameservice p2pd quagga secure txtinfo watchdog" \
	all libs install install_libs

define Package/afrimesh-olsrd/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/olsrd.config $(1)/etc/config/olsrd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/olsrd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/olsrd.init $(1)/etc/init.d/olsrd
endef

define Package/afrimesh-olsrd-mod-arprefresh/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/arprefresh/olsrd_arprefresh.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-dot-draw/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/dot_draw/olsrd_dot_draw.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-json-draw/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/json_draw/olsrd_json_draw.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-bmf/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/bmf/olsrd_bmf.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-cl-roam/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/cl_roam/olsrd_cl_roam.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-dyn-gw/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/dyn_gw/olsrd_dyn_gw.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-dyn-gw-plain/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/dyn_gw_plain/olsrd_dyn_gw_plain.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-httpinfo/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/httpinfo/olsrd_httpinfo.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-mdns/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/mdns/olsrd_mdns.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-nameservice/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/nameservice/olsrd_nameservice.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-p2pd/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/p2pd/olsrd_p2pd.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-quagga/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/quagga/olsrd_quagga.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-secure/install
	$(INSTALL_DIR) $(1)/etc/olsrd.d
	$(CP) ./files/olsrd_secure_key $(1)/etc/olsrd.d/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/secure/olsrd_secure.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-txtinfo/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/txtinfo/olsrd_txtinfo.so.* $(1)/usr/lib/
endef

define Package/afrimesh-olsrd-mod-watchdog/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/watchdog/olsrd_watchdog.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,afrimesh-olsrd))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-arprefresh))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-dot-draw))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-json-draw))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-bmf))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-dyn-gw))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-dyn-gw-plain))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-httpinfo))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-mdns))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-nameservice))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-p2pd))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-quagga))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-secure))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-txtinfo))
$(eval $(call BuildPackage,afrimesh-olsrd-mod-watchdog))
