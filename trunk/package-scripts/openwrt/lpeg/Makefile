#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lpeg
PKG_VERSION:=0.10
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.inf.puc-rio.br/~roberto/lpeg/
PKG_MD5SUM:=9925fa806fc0ec2b88c8aa961994962b

include $(INCLUDE_DIR)/package.mk

define Package/lpeg
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LPeg pattern-matching library for Lua.
  URL:=http://www.inf.puc-rio.br/~roberto/lpeg/lpeg.html
  DEPENDS:=+lua 
endef

define Package/lpeg/description
  LPeg is a pattern-matching library for Lua, based on Parsing Expression 
	Grammars (PEGs). 
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
    CC="$(TARGET_CC) $(TARGET_CFLAGS) -I"$(STAGING_DIR)/usr/include" -std=gnu99 -fPIC" \
		lpeg.so
endef

define Package/lpeg/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/re.lua $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lpeg.so $(1)/usr/lib/lua
endef

$(eval $(call BuildPackage,lpeg))
