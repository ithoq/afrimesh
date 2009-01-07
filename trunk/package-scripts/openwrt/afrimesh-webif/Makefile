#
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

PKG_NAME:=afrimesh-webif
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

COMPILE_COMMAND:=	$(TARGET_CC) $(TARGET_CFLAGS) \
                -DDISABLE_LOG \
                -D_METAPACK \
                -DOPENWRT \
                -Isrc \
                -I$(STAGING_DIR)/include-host -I$(STAGING_DIR_HOST)/include -include endian.h \
                -I$(STAGING_DIR)/usr/include \
                $(TARGET_CFLAGS_EXTRA) \
                $(TARGET_LDFLAGS) 


include $(INCLUDE_DIR)/package.mk

define Package/afrimesh-webif
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base
	TITLE:=Web interface for mesh nodes
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/afrimesh-webif/description
	Web interface for mesh nodes
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	#$(CP) src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/afrimesh-webif/install
	$(CP) ./files/* $(1)/
	find $(1) -name 'CVS' -o -name '.svn' -o -name '*~' | $(XARGS) rm -rf
endef

$(eval $(call BuildPackage,afrimesh-webif))
