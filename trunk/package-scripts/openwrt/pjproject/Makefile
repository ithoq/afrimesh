include $(TOPDIR)/rules.mk

PKG_NAME:=pjproject
PKG_VERSION:=0.8.0
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.pjsip.org/release/$(PKG_VERSION)
PKG_MD5SUM:=
PKG_INSTALL_DIR:=/tmp/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/pjproject
	SECTION:=net
	CATEGORY:=Network
	URL:=http://www.pjsip.org/
	DEPENDS:= +uclibcxx
	TITLE:=Open source SIP stack
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/pjproject/description
	Open source SIP stack and media stack for presence, im/instant messaging, 
	and multimedia communication
endef

TARGET_CFLAGS += -DOPENWRT $(FPIC) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR_HOST)/include
TARGET_LDFLAGS += 
CONFIGURE_ARGS += 
CONFIGURE_VARS += CPPFLAGS="$$$$CPPFLAGS -I$(STAGING_DIR)/usr/include/uClibc++" \
                  CXXFLAGS="$$$$CXXFLAGS -fno-builtin -fno-rtti -nostdinc++ -I$(LINUX_DIR)/include" \
                  LIBS="-nodefaultlibs -luClibc++ -lm -lgcc"

define Build/Configure
	$(RM)    -rf $(PKG_INSTALL_DIR)
	$(MKDIR) -p  $(PKG_INSTALL_DIR)
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR=$(PKG_INSTALL_DIR) LD="\$$$$(CC)" dep all install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/local/include/*.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/local/include/*.hpp $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pj $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pj++ $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjlib-util $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjmedia $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjmedia-codec $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjnath $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjsip $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjsip-simple $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjsip-ua $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/pjsua-lib $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/local/include/speex $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	#$(CP) $(PKG_INSTALL_DIR)/usr/local/lib/lib*.{a,so*} $(1)/usr/lib/
	$(CP) -f $(PKG_INSTALL_DIR)/usr/local/lib/lib*.a $(1)/usr/lib/	
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP) -f $(PKG_INSTALL_DIR)/usr/local/lib/pkgconfig/libpj.pc $(1)/usr/lib/pkgconfig/
endef

define Package/pjproject/install
	$(INSTALL_DIR) $(1)/usr/lib
	#$(CP) $(PKG_INSTALL_DIR)/usr/local/lib/lib*.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,pjproject))
