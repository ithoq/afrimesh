# Afrimesh: easy management for mesh networks
#
# This software is licensed as free software under the terms of the
# New BSD License. See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk


# - configuration ----------------------------------------------------------
PKG_NAME:=afrimesh
PKG_REV:=1158
#PKG_REV:=`svn info | grep -o "Revision: [0-9]*" | sed -e "s/Revision: //"`
PKG_VERSION:=1.0pre-r$(PKG_REV)
PKG_RELEASE:=1


# See: http://downloads.openwrt.org/people/florian/fosdem/openwrt_cfp_fosdem2008.pdf
#      https://dev.openwrt.org/ticket/3815
#      http://forum.openwrt.org/viewtopic.php?id=18090
##PKG_SOURCE_MIRROR=0
# - to use local sources, disable PKG_SOURCE_URL & set LOCAL_SOURCES -------
#LOCAL_SOURCES:=~/afrimesh
ifeq "$(wildcard $(LOCAL_SOURCES) )"  ""
PKG_SOURCE_URL:=http://afrimesh.googlecode.com/svn/trunk
#PKG_SOURCE_URL:=http://afrimesh.googlecode.com/svn/branches/unstable
#PKG_SOURCE_URL:=http://afrimesh.googlecode.com/svn/tags/afrimesh-1.0pre-r1153-unstable
endif


# - sources ----------------------------------------------------------------
PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(if $(PKG_BRANCH),$(PKG_BRANCH),$(PKG_NAME))-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)


# - afrimesh-base ----------------------------------------------------------
include $(INCLUDE_DIR)/package.mk
define Package/afrimesh-base
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+libuci +libjson +ntpclient +mini-httpd +netcat +lua +luasocket +lualogging +luajson +libuci-lua
	TITLE:=Easy management for wireless community mesh netwo3rks
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
	#$(call Config,afrimesh.location.latitude,string,-25.7481182,Router Latitude)
	#$(call Config,afrimesh.location.longitude,string,28.2822312,Router Longitude)
endef

# if CONTROL/conffiles manages upgrades, then what does this section do?
define Package/base-files/conffiles
/etc/config/afrimesh
$(call $(TARGET)/conffiles)
endef

define Package/afrimesh-base/description
	Easy management for wireless community networks
endef

define Package/afrimesh-base/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_DIR)  $(1)/etc/crontabs
	$(INSTALL_DIR)  $(1)/www/cgi-bin

	mkdir -p $(1)/CONTROL
	echo "/etc/config/afrimesh" >> $(1)/CONTROL/conffiles
	$(INSTALL_CONF) ./files/etc/config/afrimesh             $(1)/etc/config
	$(INSTALL_BIN)  ./files/etc/crontabs/afrimesh           $(1)/etc/crontabs/root
	$(INSTALL_BIN)  ./files/www/cgi-bin/notify.sh           $(1)/www/cgi-bin/notify
	$(INSTALL_BIN)  ./files/www/cgi-bin/status.sh           $(1)/www/cgi-bin/status
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/villagebus/villagebus  $(1)/www/cgi-bin/villagebus

	# villagebus.lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/luarocks
	touch $(1)/usr/lib/lua/luarocks/require.lua # fake luarocks.require for OpenWRT
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/villagebus/lua/villagebus.lua           $(1)/www/cgi-bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/urlcode.lua             $(1)/www/cgi-bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/common.lua              $(1)/www/cgi-bin
	$(INSTALL_DIR)  $(1)/www/cgi-bin/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/modules/http.lua         $(1)/www/cgi-bin/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/modules/provision.lua    $(1)/www/cgi-bin/modules

	# - runonce --
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_BIN)  ./files/etc/init.d/runonce  $(1)/etc/init.d/runonce.afrimesh-base
endef


# - afrimesh-provision -----------------------------------------------------
define Package/afrimesh-provision
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base
	TITLE:=Mesh router auto-provisioning support
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/afrimesh-provision/description
	Mesh router auto-provisioning support
endef

define Package/afrimesh-provision/install
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_DIR)  $(1)/www/cgi-bin
	$(INSTALL_BIN)  ./files.provision/etc/init.d/provision             $(1)/etc/init.d/provision
	$(INSTALL_BIN)  ./files.provision/www/cgi-bin/provision.lua        $(1)/www/cgi-bin/provision

	# - runonce --
	$(INSTALL_BIN)  ./files.provision/etc/init.d/runonce          		 $(1)/etc/init.d/runonce.afrimesh-provision
endef


# - afrimesh-batman-adv ----------------------------------------------------
define Package/afrimesh-batman-adv
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base +kmod-batman-adv
	TITLE:=Afrimesh for B.A.T.M.A.N. Layer-II wireless mesh networks
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/afrimesh-batman-adv/description
	Afrimesh for B.A.T.M.A.N. Layer-II wireless mesh networks
endef

define Package/afrimesh-batman-adv/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_DIR)  $(1)/etc/config

	# - runonce --
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_BIN)  ./files.batman-adv/etc/init.d/runonce  $(1)/etc/init.d/runonce.afrimesh-batman-adv
endef


# - villagetelco-device ---------------------------------------------------------
define Package/villagetelco-device
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+libuci +libjson +netcat +lua +luasocket +lualogging +luajson +libuci-lua
	TITLE:=Basically afrimesh-base and afrimesh-provision
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/villagetelco-device/description
	Build OpenWRT router firmware that can join a VillageTelco mesh
endef

define Package/villagetelco-device/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_DIR)  $(1)/etc/crontabs
	$(INSTALL_DIR)  $(1)/usr/lib/asterisk/sounds
	$(INSTALL_DIR)  $(1)/www/cgi-bin

	mkdir -p $(1)/CONTROL
	echo "/etc/config/afrimesh" >> $(1)/CONTROL/conffiles
	$(INSTALL_CONF) ./files.villagetelco/etc/config/afrimesh              $(1)/etc/config
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/villagebus/villagebus                $(1)/www/cgi-bin/villagebus
	$(INSTALL_BIN)  ./files/etc/crontabs/afrimesh                         $(1)/etc/crontabs/root
	$(INSTALL_BIN)  ./files/www/cgi-bin/notify.sh               			    $(1)/www/cgi-bin/notify
	$(INSTALL_BIN)  ./files/www/cgi-bin/status.sh                         $(1)/www/cgi-bin/status
	$(INSTALL_BIN)  ./files.provision/etc/init.d/provision                $(1)/etc/init.d/provision
	$(INSTALL_BIN)  ./files.provision/usr/lib/asterisk/sounds/callme.gsm  $(1)/usr/lib/asterisk/sounds
	$(INSTALL_BIN)  ./files.provision/www/cgi-bin/provision.lua           $(1)/www/cgi-bin/provision
	$(INSTALL_BIN)  ./files.provision/www/cgi-bin/provision-handset.lua   $(1)/www/cgi-bin/provision-handset

	# villagebus.lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/luarocks
	touch $(1)/usr/lib/lua/luarocks/require.lua # fake luarocks.require for OpenWRT
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/villagebus/lua/villagebus.lua           $(1)/www/cgi-bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/urlcode.lua             $(1)/www/cgi-bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/common.lua              $(1)/www/cgi-bin
	$(INSTALL_DIR)  $(1)/www/cgi-bin/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/modules/http.lua         $(1)/www/cgi-bin/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/modules/provision.lua    $(1)/www/cgi-bin/modules

	# TODO 
	#$(INSTALL_BIN)  ./files.villagetelco/etc/init.d/fallback-ip  $(1)/etc/init.d
	#$(INSTALL_DATA) ./files.villagetelco/etc/config/network      $(1)/etc/config
	#$(INSTALL_DATA) ./files.villagetelco/etc/config/wireless     $(1)/etc/config

	# - runonce --
	# Packages not part of the base distribution tend to overwrite anything we install above
	#$(INSTALL_DATA) ./files.villagetelco/etc/firewall.user.runonce   $(1)/etc/firewall.user.villagetelco-device
	#$(INSTALL_DATA) ./files.villagetelco/etc/config/firewall.runonce $(1)/etc/config/firewall.villagetelco-device
	#$(INSTALL_DATA) ./files.villagetelco/etc/config/batmand.runonce  $(1)/etc/config/batmand.villagetelco-device
	$(INSTALL_BIN)  ./files.villagetelco/etc/init.d/runonce          $(1)/etc/init.d/runonce.villagetelco-device
endef


# - fabfi-device ---------------------------------------------------------
define Package/fabfi-device
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+libuci +libjson +netcat +lua +luasocket +lualogging +luajson
	TITLE:=Basically afrimesh-base and afrimesh-provision
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/fabfi-device/description
	Afrimesh-enable a FabFi router
endef

define Package/fabfi-device/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_DIR)  $(1)/etc/crontabs
	$(INSTALL_DIR)  $(1)/www/cgi-bin

	mkdir -p $(1)/CONTROL
	echo "/etc/config/afrimesh" >> $(1)/CONTROL/conffiles
	$(INSTALL_CONF) ./files.fabfi/etc/config/afrimesh                     $(1)/etc/config
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/villagebus/villagebus                $(1)/www/cgi-bin/villagebus
	$(INSTALL_BIN)  ./files/etc/crontabs/afrimesh                         $(1)/etc/crontabs/root
	$(INSTALL_BIN)  ./files/www/cgi-bin/notify.sh               			    $(1)/www/cgi-bin/notify
	$(INSTALL_BIN)  ./files/www/cgi-bin/status.olsrd.sh                   $(1)/www/cgi-bin/status
	#$(INSTALL_BIN)  ./files.provision/etc/init.d/provision                $(1)/etc/init.d/provision
	#$(INSTALL_BIN)  ./files.provision/www/cgi-bin/provision.lua           $(1)/www/cgi-bin/provision
	#$(INSTALL_BIN)  ./files.provision/www/cgi-bin/provision-handset.lua   $(1)/www/cgi-bin/provision-handset

	$(INSTALL_DIR)  $(1)/usr/sbin
	$(INSTALL_BIN)  ./files.fabfi/etc/init.d/tunnel-mysql                 $(1)/etc/init.d
	$(INSTALL_BIN)  ./files.fabfi/usr/sbin/tunnel-mysql                   $(1)/usr/sbin

	# villagebus.lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/luarocks
	touch $(1)/usr/lib/lua/luarocks/require.lua # fake luarocks.require for OpenWRT
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/villagebus/lua/villagebus.lua           $(1)/www/cgi-bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/urlcode.lua             $(1)/www/cgi-bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/common.lua              $(1)/www/cgi-bin
	$(INSTALL_DIR)  $(1)/www/cgi-bin/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/modules/http.lua         $(1)/www/cgi-bin/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/villagebus/lua/modules/provision.lua    $(1)/www/cgi-bin/modules

	# TODO 
	#$(INSTALL_BIN)  ./files.fabfi/etc/init.d/fallback-ip  $(1)/etc/init.d
	#$(INSTALL_DATA) ./files.fabfi/etc/config/network      $(1)/etc/config
	#$(INSTALL_DATA) ./files.fabfi/etc/config/wireless     $(1)/etc/config

	# - runonce --
	# Packages not part of the base distribution tend to overwrite anything we install above
	$(INSTALL_BIN)  ./files.fabfi/etc/init.d/runonce          $(1)/etc/init.d/runonce.fabfi-device
endef


# - afrimesh-batman --------------------------------------------------------
define Package/afrimesh-batman
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base +batmand
	TITLE:=Easy management for B.A.T.M.A.N. wireless mesh networks
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/afrimesh-batman/description
	Easy management for B.A.T.M.A.N. wireless mesh networks
endef

define Package/afrimesh-batman/install
endef


# - afrimesh-olsr ----------------------------------------------------------
define Package/afrimesh-olsr
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base +olsrd
	TITLE:=Easy management for OLSR wireless mesh networks
	VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/afrimesh-olsr/description
	Easy management for OLSR wireless mesh networks
endef

define Package/afrimesh-olsr/install
endef


# - afrimesh-gateway--------------------------------------------------------
define Package/afrimesh-gateway
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+snmpd +batmand +vis +haserl +afrimesh-pmacctd +afrimesh-pmacct-client +afrimesh-base +luci-admin-full +luci-admin-rpc
	TITLE:=Mesh gateway router configuration
endef

define Package/afrimesh-gateway/description
	Mesh gateway router configuration
endef

define Package/afrimesh-gateway/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_DIR)  $(1)/etc/init.d

	$(INSTALL_DATA) ./files.gateway/etc/config/network      $(1)/etc/config
	$(INSTALL_DATA) ./files.gateway/etc/config/wireless     $(1)/etc/config
	$(INSTALL_BIN)  ./files.gateway/etc/init.d/fallback-ip  $(1)/etc/init.d

	# - runonce --
	$(INSTALL_DATA) ./files.gateway/etc/pmacctd.conf.runonce    $(1)/etc/pmacctd.conf.afrimesh-gateway
	$(INSTALL_DATA) ./files.gateway/etc/firewall.user.runonce   $(1)/etc/firewall.user.afrimesh-gateway
	$(INSTALL_DATA) ./files.gateway/etc/config/firewall.runonce $(1)/etc/config/firewall.afrimesh-gateway
	$(INSTALL_DATA) ./files.gateway/etc/config/batmand.runonce  $(1)/etc/config/batmand.afrimesh-gateway
	$(INSTALL_DATA) ./files.gateway/etc/config/vis.runonce      $(1)/etc/config/vis.afrimesh-gateway
	$(INSTALL_BIN)  ./files.gateway/etc/init.d/vis.runonce      $(1)/etc/init.d/vis.afrimesh-gateway
	$(INSTALL_BIN)  ./files.gateway/etc/init.d/runonce          $(1)/etc/init.d/runonce.afrimesh-gateway
endef

define Package/afrimesh-gateway/postinst
#!/bin/sh
# check if we are on the real system
if [ -z "$${IPKG_INSTROOT}" ]; then
        touch /etc/afrimesh.postinst.test
fi
endef



# - afrimesh-webif ---------------------------------------------------------
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
	$(INSTALL_DIR)   $(1)/www/cgi-bin
	$(INSTALL_DIR)   $(1)/www/afrimesh
	$(INSTALL_DIR)   $(1)/www/afrimesh/images
	$(INSTALL_DIR)   $(1)/www/afrimesh/javascript
	$(INSTALL_DIR)   $(1)/www/afrimesh/javascript/jquery
	$(INSTALL_DIR)   $(1)/www/afrimesh/javascript/openlayers
	$(INSTALL_DIR)   $(1)/www/afrimesh/modules
	$(INSTALL_DIR)   $(1)/www/afrimesh/style
	$(INSTALL_DIR)   $(1)/www/afrimesh/style/images
	$(INSTALL_DIR)   $(1)/www/afrimesh/style/images/controls
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/index.redirect.html   $(1)/www/index.html
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/index.router.html     $(1)/www/afrimesh/index.html
	$(INSTALL_BIN)   $(PKG_BUILD_DIR)/dashboard/cgi-bin/ajax-proxy.cgi    $(1)/www/cgi-bin
	$(INSTALL_BIN)   $(PKG_BUILD_DIR)/dashboard/cgi-bin/mesh-topology.kml $(1)/www/cgi-bin
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/images/*.gif          $(1)/www/afrimesh/images
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/images/*.png          $(1)/www/afrimesh/images
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/javascript/jquery/*       $(1)/www/afrimesh/javascript/jquery
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/javascript/openlayers/*   $(1)/www/afrimesh/javascript/openlayers
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/javascript/afrimesh*.js   $(1)/www/afrimesh/javascript
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/modules/*.html        $(1)/www/afrimesh/modules
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/modules/*.js          $(1)/www/afrimesh/modules
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/style/*.css           $(1)/www/afrimesh/style
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/style/images/*.gif    $(1)/www/afrimesh/style/images
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/style/images/*.png    $(1)/www/afrimesh/style/images
	$(INSTALL_DATA)  $(PKG_BUILD_DIR)/dashboard/www/style/images/controls/*.png   $(1)/www/afrimesh/style/images/controls
endef


# - afrimesh-portal --------------------------------------------------------
define Package/afrimesh-portal
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base +awesome-chilli
	TITLE:=Afrimesh captive portal support
endef

define Package/afrimesh-portal/description
	Coova Chilli captive portal support
endef

define Package/afrimesh-portal/install
	$(INSTALL_DIR)  $(1)/etc/chilli
	$(INSTALL_DIR)  $(1)/etc/chilli/www
	$(INSTALL_DIR)  $(1)/www/cgi-bin

	$(INSTALL_DATA) ./files.portal/etc/chilli/defaults             $(1)/etc/chilli
	$(INSTALL_DATA) ./files.portal/etc/chilli/www/ChilliLibrary.js $(1)/etc/chilli/www
	$(INSTALL_DATA) ./files.portal/etc/chilli/www/chilli.js        $(1)/etc/chilli/www
	$(INSTALL_DATA) ./files.portal/etc/chilli/www/index.html       $(1)/etc/chilli/www
	$(INSTALL_BIN)  ./files.portal/www/cgi-bin/hotspotlogin.cgi    $(1)/www/cgi-bin
	$(INSTALL_BIN)  ./files.portal/www/cgi-bin/portal-auth.cgi     $(1)/www/cgi-bin
	$(INSTALL_BIN)  ./files.portal/www/cgi-bin/uam.pl              $(1)/www/cgi-bin
endef


# - afrimesh-dashboard -----------------------------------------------------
define Package/afrimesh-dashboard
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+afrimesh-base +afrimesh-pmacctd +afrimesh-pmacct-client
	TITLE:=Mesh management dashboard
endef

define Package/afrimesh-dashboard/description
	Mesh management dashboard
endef

define Package/afrimesh-dashboard/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_DIR)  $(1)/www/afrimesh
	$(INSTALL_DIR)  $(1)/www/cgi-bin
	$(INSTALL_DIR)  $(1)/www/afrimesh/images
	$(INSTALL_DIR)  $(1)/www/afrimesh/javascript
	$(INSTALL_DIR)  $(1)/www/afrimesh/javascript/jquery
	$(INSTALL_DIR)  $(1)/www/afrimesh/javascript/openlayers
	$(INSTALL_DIR)  $(1)/www/afrimesh/style/images
	$(INSTALL_DIR)  $(1)/www/afrimesh/style
	$(INSTALL_DIR)  $(1)/www/afrimesh/modules

	$(INSTALL_CONF) ./files/etc/config/afrimesh $(1)/etc/config
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/index.html             $(1)/www/afrimesh/index.html
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/dashboard/cgi-bin/ajax-proxy.cgi     $(1)/www/cgi-bin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/dashboard/cgi-bin/mesh-topology.kml  $(1)/www/cgi-bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/images/*.gif           $(1)/www/afrimesh/images
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/images/*.png           $(1)/www/afrimesh/images
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/jquery/*        $(1)/www/afrimesh/javascript/jquery
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/openlayers/*    $(1)/www/afrimesh/javascript/openlayers
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/afrimesh*.js    $(1)/www/afrimesh/javascript
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/*.html         $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/*.js           $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/style/*.css            $(1)/www/afrimesh/style
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/style/images/*.gif     $(1)/www/afrimesh/style/images
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/style/images/*.png     $(1)/www/afrimesh/style/images
endef


# - afrimesh-potato --------------------------------------------------------
define Package/afrimesh-potato
	SECTION:=admin
	CATEGORY:=Administration
	DEPENDS:=+libuci +libjson +batmand +haserl +luci-admin-rpc
	TITLE:=Web interface for the Mesh Potato
endef

define Package/afrimesh-potato/description
	Web interface for the Mesh Potato
endef

define Package/afrimesh-potato/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_DIR)  $(1)/www/afrimesh
	$(INSTALL_DIR)  $(1)/www/cgi-bin
	$(INSTALL_DIR)  $(1)/www/afrimesh/images
	$(INSTALL_DIR)  $(1)/www/afrimesh/javascript
	$(INSTALL_DIR)  $(1)/www/afrimesh/javascript/jquery
	$(INSTALL_DIR)  $(1)/www/afrimesh/javascript/openlayers
	$(INSTALL_DIR)  $(1)/www/afrimesh/modules
	$(INSTALL_DIR)  $(1)/www/afrimesh/style
	$(INSTALL_DIR)  $(1)/www/afrimesh/style/images
	$(INSTALL_DIR)  $(1)/www/afrimesh/style/images/controls

	$(INSTALL_CONF) ./files.potato/etc/config/afrimesh                  $(1)/etc/config
	$(INSTALL_CONF) ./files.potato/www/index.luci.html                  $(1)/www/afrimesh/index.luci.html

	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/index.redirect.html  $(1)/www/afrimesh/index.redirect.html
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/index.potato.html    $(1)/www/afrimesh/index.html
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/villagebus/villagebus                  $(1)/www/cgi-bin/villagebus
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/utilities/urldecode                    $(1)/www/cgi-bin/urldecode
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/dashboard/cgi-bin/firmware-upload.cgi  $(1)/www/cgi-bin/firmware-upload.cgi
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/dashboard/cgi-bin/firmware-flash.cgi   $(1)/www/cgi-bin/firmware-flash.cgi
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/dashboard/cgi-bin/ajax-proxy.cgi       $(1)/www/cgi-bin/ajax-proxy.cgi
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/dashboard/cgi-bin/mesh-topology.kml    $(1)/www/cgi-bin/mesh-topology.kml
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/images/*.gif         $(1)/www/afrimesh/images
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/images/*.png         $(1)/www/afrimesh/images
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/jquery/jquery.tooltip.min.js  \
			            $(1)/www/afrimesh/javascript/jquery
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/jquery/jquery.min.js  \
			            $(1)/www/afrimesh/javascript/jquery
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/openlayers/OpenLayers.js    $(1)/www/afrimesh/javascript/openlayers
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/openlayers/OpenStreetMap.js $(1)/www/afrimesh/javascript/openlayers
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/javascript/afrimesh*.js  $(1)/www/afrimesh/javascript
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/help.chat.html                $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/help.group.html               $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/help.howto.html               $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/network.map.html              $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/utility.settings.potato.html  $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/utility.login.html            $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/utility.upgrade.html          $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/network.map.js                $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/modules/utility.settings.js           $(1)/www/afrimesh/modules
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/style/*.css          $(1)/www/afrimesh/style
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/style/images/*.gif   $(1)/www/afrimesh/style/images
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/style/images/*.png   $(1)/www/afrimesh/style/images
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dashboard/www/style/images/controls/*.png   $(1)/www/afrimesh/style/images/controls
endef

define Package/afrimesh-potato/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	# relocate LuCI 
	echo "relocating LuCI"
	cp /www/afrimesh/index.luci.html     /www/index.luci.html
	cp /www/afrimesh/index.redirect.html /www/index.html
	# fix permissions for config - TODO why ?!
	chmod 0666 /etc/config/afrimesh
}
endef


# - build configuration ----------------------------------------------------
PKG_EXTRA_CFLAGS=\
	-D_METAPACK \
	-DOPENWRT \
	-I$(PKG_BUILD_DIR) \
	-I$(PKG_BUILD_DIR)/common \
  -I$(STAGING_DIR)/usr/include \
  -I$(STAGING_DIR)/include-host \
	-I$(STAGING_DIR_HOST)/include \
	-include endian.h \
	-DAFRIMESH_VERSION="\"$(VERSION)\"" \
	-DDISABLE_LOG 
TARGET_CFLAGS += $(FPIC)
MAKE_ARGS += \
	CC='$(TARGET_CC) $(PKG_EXTRA_CFLAGS) $(TARGET_CFLAGS_EXTRA) ' \
	CFLAGS='$(TARGET_CFLAGS)' \
	LDFLAGS='$(TARGET_LDFLAGS)' \
	FLAVOR='LINUX'
MAKE_VILLAGEBUS_ARGS += \
	CC='$(TARGET_CC) $(PKG_EXTRA_CFLAGS) $(TARGET_CFLAGS_EXTRA) ' \
	CFLAGS='$(TARGET_CFLAGS)' \
	LDFLAGS='$(TARGET_LDFLAGS) -ljson -luci' \
	FLAVOR='LINUX'


# - build rules ------------------------------------------------------------
define Build/Prepare
	-$(call Build/Prepare/Default)
	if [ -z "$(PKG_SOURCE_URL)" ]; then \
		echo "Building from local sources" ; \
		rm -rf $(PKG_BUILD_DIR)	  ; \
		mkdir  $(PKG_BUILD_DIR)   ; \
    $(LOCAL_SOURCES)/clean.sh ; \
		$(CP) $(LOCAL_SOURCES)/*  $(PKG_BUILD_DIR) ; \
	fi
endef

define Build/Configure
endef

define Build/Compile
	cd $(PKG_BUILD_DIR)/utilities  ; VERSION=$(PKG_VERSION) make $(MAKE_ARGS) 
	cd $(PKG_BUILD_DIR)/villagebus ; VERSION=$(PKG_VERSION) make $(MAKE_VILLAGEBUS_ARGS) 
endef


# - eval packages ----------------------------------------------------------
$(eval $(call BuildPackage,afrimesh-base))
$(eval $(call BuildPackage,afrimesh-batman))
$(eval $(call BuildPackage,afrimesh-batman-adv))
$(eval $(call BuildPackage,afrimesh-olsr))
$(eval $(call BuildPackage,afrimesh-provision))
$(eval $(call BuildPackage,afrimesh-gateway))
$(eval $(call BuildPackage,afrimesh-portal))
$(eval $(call BuildPackage,afrimesh-webif))
$(eval $(call BuildPackage,afrimesh-potato))
$(eval $(call BuildPackage,afrimesh-dashboard))
$(eval $(call BuildPackage,fabfi-device))
$(eval $(call BuildPackage,villagetelco-device))

