include $(TOPDIR)/rules.mk

PKG_NAME:=assolo
PKG_VERSION:=0.9a
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://netlab-mn.unipv.it/assolo
PKG_MD5SUM:=91c459bec6393343c1f9d9a3cc7e7080

include $(INCLUDE_DIR)/package.mk

define Package/assolo
  SECTION:=net
  CATEGORY:=Network
	TITLE:=End-to-end available bandwidth estimation tool
  URL:=http://netlab-mn.unipv.it/assolo/
endef

define Package/assolo/description
 ASSOLO is a new active probing tool for estimating available bandwidth 
 based on the concept of "self-induced congestion". ASSOLO features a 
 new probing traffic profile called REACH (Reflected ExponentiAl Chirp), 
 which tests a wide range of rates being more accurate in the center of 
 the probing interval. 
endef

define Package/assolo/install
	$(INSTALL_DIR)  $(1)/usr/bin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/Bin/$(ARCH)/assolo_rcv   $(1)/usr/bin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/Bin/$(ARCH)/assolo_run   $(1)/usr/bin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/Bin/$(ARCH)/assolo_snd   $(1)/usr/bin
endef

$(eval $(call BuildPackage,assolo))
