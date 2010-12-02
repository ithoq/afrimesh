#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luajson
PKG_VERSION:=1.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://luaforge.net/frs/download.php/4676/
PKG_MD5SUM:=9efc22baf18b757d7a592376c2113bfd

include $(INCLUDE_DIR)/package.mk

define Package/luajson
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LuaJSON
  URL:=http://github.com/harningt/luajson/
  DEPENDS:=+lua +lpeg
endef

define Package/luajson/description
  JSON parser/encoder for Lua. Parses JSON using LPEG for speed and 
	flexibility. Depending on parser/encoder options, various values 
	are preserved as best as possible.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/ \
		all
endef


define Package/luajson/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	cp -r $(PKG_BUILD_DIR)/lua/* $(1)/usr/lib/lua
endef

$(eval $(call BuildPackage,luajson))
