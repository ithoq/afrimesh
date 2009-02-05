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

PKG_NAME:=afrimesh-base
PKG_VERSION:=0.1
PKG_RELEASE:=1

LOCAL_SOURCES:=../../../

# See: http://downloads.openwrt.org/people/florian/fosdem/openwrt_cfp_fosdem2008.pdf
#      https://dev.openwrt.org/ticket/3815
#      http://forum.openwrt.org/viewtopic.php?id=18090
#PKG_SOURCE_MIRROR=0
#PKG_SOURCE_PROTO:=svn
#PKG_SOURCE_URL:=http://afrimesh.googlecode.com/svn/trunk/src/$(PKG_NAME)
#PKG_SOURCE_SUBDIR:=$(PKG_NAME)
#PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

COMPILE_COMMAND:=	$(TARGET_CC) $(TARGET_CFLAGS) \
                -DDISABLE_LOG \
                -D_METAPACK \
                -DOPENWRT \
                -I$(PKG_BUILD_DIR) \
                -I$(STAGING_DIR)/include-host -I$(STAGING_DIR_HOST)/include -include endian.h \
                -I$(STAGING_DIR)/usr/include \
                $(TARGET_CFLAGS_EXTRA) \
                $(TARGET_LDFLAGS) 


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

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) $(LOCAL_SOURCES)/common/*          $(PKG_BUILD_DIR)
	$(CP) $(LOCAL_SOURCES)/village-bus-uci/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	cd $(PKG_BUILD_DIR) ; \
	$(COMPILE_COMMAND) -o $(PKG_BUILD_DIR)/village-bus-uci json_cgi.c village-bus-uci.c  -ljson -luci
	$(STRIP) $(PKG_BUILD_DIR)/village-bus-uci
endef

define Package/afrimesh-base/install
	$(CP) ./files/* $(1)/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/village-bus-uci $(1)/www/cgi-bin/village-bus-uci
	find $(1) -name 'CVS' -o -name '.svn' -o -name '*~' | $(XARGS) rm -rf
endef

$(eval $(call BuildPackage,afrimesh-base))
