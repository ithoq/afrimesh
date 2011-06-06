#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=redis-lua
PKG_VERSION:=2.0.0-0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://cloud.github.com/downloads/nrk/redis-lua/
PKG_MD5SUM:=db1f9a74d13158c1b551a4fa054a92ba

include $(INCLUDE_DIR)/package.mk

define Package/redis-lua
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Lua library for Redis
  URL:=https://github.com/nrk/redis-lua/
  DEPENDS:=+luasocket
endef

define Package/redis-lua/description
 A Lua client library for the redis key value storage system.
endef

define Build/Configure
endef

define Build/Compile
	touch $(PKG_BUILD_DIR)/Makefile
endef

define Package/redis-lua/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	cp -r $(PKG_BUILD_DIR)/src/redis.lua $(1)/usr/lib/lua
endef

$(eval $(call BuildPackage,redis-lua))
