include $(TOPDIR)/rules.mk

PKG_NAME:=openvoip
PKG_VERSION:=0.2
PKG_RELEASE:=1
PKG_SOURCE:=p2p.tar.gz
PKG_SOURCE_URL:=http://www1.cs.columbia.edu/~salman/peer/src/0_2
PKG_MD5SUM:=
PKG_BUILD_DIR:=$(BUILD_DIR)/p2p
PKG_BUILD_DEPENDS:=pjproject

include $(INCLUDE_DIR)/package.mk

define Package/openvoip-p2pp
	SECTION:=net
	CATEGORY:=Network
	URL:=http://www1.cs.columbia.edu/~salman/peer/
	DEPENDS:=+uclibcxx +libuuid 
	TITLE:=An Open Peer-to-Peer VoIP and IM System
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/openvoip-p2pp/description
	An Open Peer-to-Peer VoIP and IM System
endef

TARGET_CFLAGS  += -I$(STAGING_DIR)/usr/include/uClibc++ -I$(STAGING_DIR)/usr/include \
                  -DOPENWRT $(FPIC) -fno-builtin -nostdinc++ -fno-rtti
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib -nodefaultlibs -luClibc++ -lc -lm -lgcc -luuid 

define Build/Configure
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR=$(PKG_INSTALL_DIR) all install
endef

define Package/openvoip-p2pp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/checker $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/p2pmain $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/p2pp
	$(INSTALL_DATA) ./files/configa.txt $(1)/etc/p2pp
	$(INSTALL_DATA) ./files/configc.txt $(1)/etc/p2pp
	$(INSTALL_DATA) ./files/configp.txt $(1)/etc/p2pp
endef

$(eval $(call BuildPackage,openvoip-p2pp))
