# Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
# Copyright (C) 2008-2009 Meraka Institute of the CSIR
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holders nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#

include $(TOPDIR)/rules.mk


# - configuration ------------------------------------------------------------
PKG_NAME:=afrimesh
PKG_REV:=240
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

LOCAL_SOURCES:=../../../

# See: http://downloads.openwrt.org/people/florian/fosdem/openwrt_cfp_fosdem2008.pdf
#      https://dev.openwrt.org/ticket/3815
#      http://forum.openwrt.org/viewtopic.php?id=18090
##PKG_SOURCE_MIRROR=0
PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(if $(PKG_BRANCH),$(PKG_BRANCH),$(PKG_NAME))-$(PKG_VERSION)
PKG_SOURCE_URL:=http://afrimesh.googlecode.com/svn/trunk
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

#PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)



COMPILE_COMMAND:=	$(TARGET_CC) $(TARGET_CFLAGS) \
                -DDISABLE_LOG \
                -D_METAPACK \
                -DOPENWRT \
                -I$(PKG_BUILD_DIR) \
                -I$(STAGING_DIR)/include-host -I$(STAGING_DIR_HOST)/include -include endian.h \
                -I$(STAGING_DIR)/usr/include \
                $(TARGET_CFLAGS_EXTRA) \
                $(TARGET_LDFLAGS) 

PKG_EXTRA_CFLAGS= \
	-DDISABLE_LOG                 \
	-D_METAPACK                   \
	-DOPENWRT                     \
	-I$(PKG_BUILD_DIR)            \
	-I$(PKG_BUILD_DIR)/common     \
  -I$(STAGING_DIR)/usr/include  \
  -I$(STAGING_DIR)/include-host \
	-I$(STAGING_DIR_HOST)/include \
	-include endian.h             \


MAKE_UCI_ARGS += \
	CC='$(TARGET_CC) $(PKG_EXTRA_CFLAGS) $(TARGET_CFLAGS_EXTRA) ' \
	CFLAGS='$(TARGET_CFLAGS) ' \
	LDFLAGS='$(TARGET_LDFLAGS) -ljson -luci'



# - afrimesh-base ------------------------------------------------------------
include $(INCLUDE_DIR)/package.mk

define Package/afrimesh-base
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+busybox +libuci +libjson +batmand
	TITLE:=Easy management for B.A.T.M.A.N. wireless mesh networks
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/afrimesh-base/description
	Easy management for B.A.T.M.A.N. wireless mesh networks
endef

define Package/afrimesh-base/install
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_DIR)  $(1)/www/cgi-bin
	$(INSTALL_BIN)  ./files/etc/firewall.afrimesh $(1)/etc
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/village-bus-uci/village-bus-uci $(1)/www/cgi-bin/village-bus-uci
	$(INSTALL_DATA) ./files/etc/config/afrimesh $(1)/etc/config
endef


# - afrimesh-config ----------------------------------------------------------
define Package/afrimesh-config
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base
	TITLE:=Easy management for B.A.T.M.A.N. wireless mesh networks
endef

define Package/afrimesh-config/description
	Manual configuration for Mesh nodes
endef

define Package/afrimesh-config/install
	$(INSTALL_DIR)  $(1)/etc/config
endef


# - afrimesh-portal ----------------------------------------------------------
define Package/afrimesh-portal
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base +coova-chilli
	TITLE:=Afrimesh captive portal support
endef

define Package/afrimesh-portal/description
	Coova Chilli captive portal support
endef

define Package/afrimesh-portal/install
	$(INSTALL_DIR)  $(1)/etc/chilli
	$(INSTALL_DIR)  $(1)/etc/chilli/www
	$(INSTALL_DIR)  $(1)/www/cgi-bin
	$(INSTALL_DATA) ./files/etc/chilli/defaults $(1)/etc/chilli
	$(INSTALL_DATA) ./files/etc/chilli/www/ChilliLibrary.js $(1)/etc/chilli/www
	$(INSTALL_DATA) ./files/etc/chilli/www/chilli.js  $(1)/etc/chilli/www
	$(INSTALL_DATA) ./files/etc/chilli/www/index.html $(1)/etc/chilli/www
	$(INSTALL_BIN)  ./files/www/cgi-bin/hotspotlogin.cgi $(1)/www/cgi-bin
	$(INSTALL_BIN)  ./files/www/cgi-bin/portal-auth.cgi $(1)/www/cgi-bin
	$(INSTALL_BIN)  ./files/www/cgi-bin/uam.pl $(1)/www/cgi-bin
endef


# - afrimesh-webif -----------------------------------------------------------
define Package/afrimesh-webif
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base
	TITLE:=Web interface for mesh nodes
endef

define Package/afrimesh-webif/description
	Web interface for mesh nodes
endef

define Package/afrimesh-webif/install
	$(INSTALL_DIR)  $(1)/www/cgi-bin
endef


# - build rules --------------------------------------------------------------
define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Configure
endef

define Build/Compile
	cd $(PKG_BUILD_DIR)/village-bus-uci ; make $(MAKE_UCI_ARGS)
endef


define Build/Prepare-local
	$(call Build/Prepare/Default)
	$(CP) $(LOCAL_SOURCES)/common/*          $(PKG_BUILD_DIR)
	$(CP) $(LOCAL_SOURCES)/village-bus-uci/* $(PKG_BUILD_DIR)
endef

define Build/Compile-local
	cd $(PKG_BUILD_DIR) ; \
	$(COMPILE_COMMAND) -o $(PKG_BUILD_DIR)/village-bus-uci json_cgi.c village-bus-uci.c main.c -ljson -luci
	$(STRIP) $(PKG_BUILD_DIR)/village-bus-uci
endef




$(eval $(call BuildPackage,afrimesh-base))
$(eval $(call BuildPackage,afrimesh-config))
$(eval $(call BuildPackage,afrimesh-portal))
$(eval $(call BuildPackage,afrimesh-webif))

