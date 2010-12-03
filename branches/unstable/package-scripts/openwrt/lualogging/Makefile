#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lualogging
PKG_VERSION:=1.1.4
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://luaforge.net/frs/download.php/2693/
PKG_MD5SUM:=72a8622748a525f5fb8ed23278326f80

include $(INCLUDE_DIR)/package.mk

define Package/lualogging
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Lua logging library
  URL:=http://www.keplerproject.org/lualogging/
  DEPENDS:=+liblua
endef

define Package/lualogging/description
 LuaLogging provides a simple API to use logging features in Lua.	
endef

define Package/lualogging/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	cp -r $(PKG_BUILD_DIR)/src/logging $(1)/usr/lib/lua
	echo 'require("logging.logging")' > $(1)/usr/lib/lua/logging.lua
endef

$(eval $(call BuildPackage,lualogging))
