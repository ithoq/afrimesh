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

# - configuration ----------------------------------------------------------
VERSION=0.9-r809
RELEASE=1

MODULES=\
  ENABLE_MOD_PROVISION=1 \
  ENABLE_MOD_DB=1 \
  ENABLE_MOD_SNMP=1

# Ubuntu Launchpad Personal Package Archive 
#   _DO_ set this to your personal ppa if you want to build test packages
#   _DON'T_ set this to afrimesh-ppa unless you are planning on doing a
#   release !!!                                   
PPA=antoine-7degrees-ppa
#PPA=afrimesh-ppa

# Some non-standard dependencies are hosted here
#DEPS_URL="https://launchpad.net/~antoine-7degrees/+archive/ppa/+files/"


# - platform detection -----------------------------------------------------
DEPROOT=/usr
WWW_ROOT=$(DESTDIR)/www
DASHBOARD_CGI=$(DESTDIR)/www/cgi-bin
DASHBOARD_ETC=$(DESTDIR)/etc
UNAME=$(shell uname)
FLAVOR=LINUX
MAKE=make

ifeq ($(UNAME),Linux)
WWW_ROOT=$(DESTDIR)/var/www
DASHBOARD_CGI=$(DESTDIR)/usr/lib/cgi-bin
DEPROOT=/usr
MAKE=make
FLAVOR=LINUX
endif

ifeq ($(UNAME),FreeBSD)
WWW_ROOT=$(DESTDIR)/usr/local/www/apache22/data
DASHBOARD_CGI=$(DESTDIR)/usr/local/www/apache22/cgi-bin
DEPROOT=/usr/local
MAKE=gmake
FLAVOR=BSD
endif

ifeq ($(UNAME),Darwin)
WWW_ROOT=$(DESTDIR)/Library/WebServer/share/httpd
DASHBOARD_CGI=$(DESTDIR)/Library/WebServer/CGI-Executables
#DEPROOT=/opt/local
DEPROOT=/Volumes/afrimesh-dev/ext
MAKE=make
FLAVOR=BSD
CFLAGS+=-m32
endif

DASHBOARD_WWW=$(WWW_ROOT)/afrimesh.vt # TODO - fix before release!


# - common -----------------------------------------------------------------
INSTALL=cp -rf
MKDIR=mkdir -p

MFLAGS += \
  FLAVOR=$(FLAVOR) \
	VERSION=$(VERSION) \
	CFLAGS=$(CFLAGS) \
	LDFLAGS=$(LDFLAGS) 

# - targets -----------------------------------------------------------------
all: 
	#TODO - export DEPROOT=$(DEPROOT); cd village-bus-radius ; $(MAKE)
	export DEPROOT=$(DEPROOT); cd villagebus ; $(MFLAGS) $(MODULES) $(MAKE) 
	export DEPROOT=$(DEPROOT); cd utilities  ; $(MFLAGS) $(MAKE) 

install: install-www install-config

install-www: 
	@echo "Installing dashboard web interface in: $(DASHBOARD_WWW)"
	@if ! test -d $(DASHBOARD_WWW) ; then mkdir -p $(DASHBOARD_WWW) ; fi
	$(INSTALL) dashboard/www/index.html $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/icons      $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/images     $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/style      $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/modules    $(DASHBOARD_WWW)
	$(INSTALL) dashboard/javascript     $(DASHBOARD_WWW) # TODO - crunch all javascript into a single file 
	@if ! test -f $(WWW_ROOT)/index.html ; then $(INSTALL) dashboard/www/index.redirect.html $(WWW_ROOT)/index.html ; fi # redirect
	@echo "Installing dashboard cgi scripts in: $(DASHBOARD_CGI)"
	@if ! test -d $(DASHBOARD_CGI) ; then mkdir -p $(DASHBOARD_CGI) ; fi
	$(INSTALL) ./package-scripts/openwrt/afrimesh/files.portal/www/cgi-bin/uam.pl $(DASHBOARD_CGI)
	$(INSTALL) dashboard/cgi-bin/ajax-proxy.cgi       $(DASHBOARD_CGI)
	$(INSTALL) dashboard/cgi-bin/mesh-topology.kml    $(DASHBOARD_CGI)
	chmod 0755 $(DASHBOARD_CGI)/mesh-topology.kml
	-$(INSTALL) utilities/urldecode                   $(DASHBOARD_CGI)
	-$(INSTALL) villagebus/villagebus                 $(DASHBOARD_CGI)
	-$(INSTALL) village-bus-radius/village-bus-radius $(DASHBOARD_CGI)
	find $(DASHBOARD_WWW) -name "*~"   | xargs rm -f
	find $(DASHBOARD_WWW) -name ".svn" | xargs rm -rf
	find $(DASHBOARD_CGI) -name "*~"   | xargs rm -f
	find $(DASHBOARD_CGI) -name ".svn" | xargs rm -rf

install-config: 
	@echo "Installing configuration files in: $(DASHBOARD_ETC)"
	[ -d $(DASHBOARD_ETC)/config ] || { \
		mkdir -p $(DASHBOARD_ETC)/config; \
		chmod 0775 $(DASHBOARD_ETC)/config; \
	}
	[ -e $(DASHBOARD_ETC)/config/afrimesh ] || { \
		$(INSTALL) config/afrimesh $(DASHBOARD_ETC)/config/afrimesh; \
		chmod 0664 $(DASHBOARD_ETC)/config/afrimesh; \
	}
	[ -e $(DASHBOARD_ETC)/config/batmand ] || { \
		$(INSTALL) config/batmand $(DASHBOARD_ETC)/config/batmand; \
		chmod 0664 $(DASHBOARD_ETC)/config/batmand; \
	}

clean : # clean-www
	cd village-bus-radius ; $(MAKE) clean
	cd villagebus         ; $(MAKE) clean
	cd utilities          ; $(MAKE) clean

clean-www: 
	@echo "Cleaning"
	rm -rf $(DASHBOARD_WWW)/index.html
	rm -rf $(DASHBOARD_WWW)/images
	rm -rf $(DASHBOARD_WWW)/style
	rm -rf $(DASHBOARD_WWW)/modules
	rm -rf $(DASHBOARD_WWW)/javascript
	rm -rf $(DASHBOARD_CGI)/*

distclean : clean
	#cd village-bus-radius ; make distclean

sources : clean
	echo "Assembling local sources"
	rm -rf /tmp/afrimesh-$(VERSION).tar.gz
	cp -r ../trunk /tmp/afrimesh-$(VERSION)
	rm -rf /tmp/afrimesh-$(VERSION)/upstream
	find /tmp/afrimesh-$(VERSION) -name "*~"   | xargs rm -f
	find /tmp/afrimesh-$(VERSION) -name "#*#"   | xargs rm -f
	find /tmp/afrimesh-$(VERSION) -name ".svn" | xargs rm -rf
	cd /tmp ; tar cf - afrimesh-$(VERSION) > afrimesh-$(VERSION).tar
	cd /tmp ; gzip afrimesh-$(VERSION).tar
	rm -rf /tmp/afrimesh-$(VERSION)
	ls -al /tmp/afrimesh-$(VERSION).tar.gz


# - linux --------------------------------------------------------------------
# also see: https://wiki.ubuntu.com/PackagingGuide/Complete
PKG_BUILD_DIR=/tmp/build
DEPS_HOOK="A70deps"
linux : all
install-linux : install
	chown -R :www-data $(DASHBOARD_ETC)/config
clean-linux : clean
	rm -rf $(PKG_BUILD_DIR)

launchpad-linux : source-packages-linux
	@echo "Pushing packages to launchpad.net ppa"
	# TODO run lintian & linda
	dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/afrimesh-dashboard_$(VERSION)-$(RELEASE)_source.changes

packages-linux : source-packages-linux binary-packages-linux

source-packages-linux : prep-linux
	@echo "Building Debian/Ubuntu source packages"
	cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; debuild -S

binary-packages-linux : prep-linux
	@echo "Building Debian/Ubuntu packages"
	rm -f ~/.pbuilderrc
	echo "HOOKDIR=$(PKG_BUILD_DIR)/hook.d" >> ~/.pbuilderrc
	mkdir -p $(PKG_BUILD_DIR)/hook.d
	cp package-scripts/debian/pbuilder-update-$(PPA)-keys.sh $(PKG_BUILD_DIR)/hook.d/D70update-ppa-keys
	cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; pdebuild 
	@echo "Built: "
	ls -al /var/cache/pbuilder/result

pbuilder-create-linux :
	sudo pbuilder create --distribution lucid \
                       --othermirror "deb http://archive.ubuntu.com/ubuntu lucid main restricted universe multiverse"
pbuilder-keys-linux :
	gpg --keyserver keyserver.ubuntu.com --recv 382AF1D2
	gpg --export --armor 382AF1D2 | sudo apt-key add -
	gpg --keyserver keyserver.ubuntu.com --recv 50CFBA3E
	gpg --export --armor 50CFBA3E | sudo apt-key add -
	sudo apt-get update
pbuilder-update-linux :
	sudo pbuilder update

prep-linux : clean-linux sources #hooks-linux
	@echo "Initializing linux package scripts for build"
	mkdir -p $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR) ; cp /tmp/afrimesh-$(VERSION).tar.gz .
	cd $(PKG_BUILD_DIR) ; tar xzvf afrimesh-$(VERSION).tar.gz
	cd $(PKG_BUILD_DIR) ; mv afrimesh-$(VERSION) afrimesh-dashboard-$(VERSION)
	cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; dh_make -c BSD -e antoine@7degrees.co.za -s --createorig
	rm -rf $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION)/debian/*
	cp -a package-scripts/debian/afrimesh-dashboard/* $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION)/debian

#hooks-linux :
#	@echo "Installing hooks to install unofficial packages needed to build Afrimesh"
#	mkdir -p $(PKG_BUILD_DIR)
#	rm -f ~/.pbuilderrc
#	echo "HOOKDIR=$(PKG_BUILD_DIR)/hook.d" >> ~/.pbuilderrc
#	mkdir -p $(PKG_BUILD_DIR)/hook.d
#	echo "#!/bin/sh" > $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "echo \"Installing unofficial dependencies for Afrimesh\"" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "cd /tmp ; wget $(DEPS_URL)/uci_0.7.5-1_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "cd /tmp ; wget $(DEPS_URL)/uci-dev_0.7.5-1_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "cd /tmp ; wget $(DEPS_URL)/json-c_0.9-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK
#	echo "cd /tmp ; wget $(DEPS_URL)/json-c-dev_0.9-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "cd /tmp ; wget $(DEPS_URL)/libmemcachedb_0.25-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "cd /tmp ; wget $(DEPS_URL)/libmemcachedb-dev_0.25-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "cd /tmp ; wget $(DEPS_URL)/libcredis_0.2.2-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "cd /tmp ; wget $(DEPS_URL)/libcredis-dev_0.2.2-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/uci_0.7.5-1_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/uci-dev_0.7.5-1_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/json-c_0.9-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/json-c-dev_0.9-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/libmemcachedb_0.25-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/libmemcachedb-dev_0.25-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/libcredis_0.2.2-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/libcredis-dev_0.2.2-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	chmod 0755 $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)

depends-packages-linux-json-c :
	wget --no-clobber -c -P $(PKG_BUILD_DIR) http://oss.metaparadigm.com/json-c/json-c-0.9.tar.gz
	rm -rf $(PKG_BUILD_DIR)/json-c-0.9
	rm -rf $(PKG_BUILD_DIR)/json-c-0.9.orig
	cd $(PKG_BUILD_DIR) ; tar xzvf json-c-0.9.tar.gz 
	@cd $(PKG_BUILD_DIR)/json-c-0.9 ; dh_make --library -c BSD -e antoine@7degrees.co.za --packagename json-c --createorig 
	rm -rf  $(PKG_BUILD_DIR)/json-c-0.9/debian/*.ex
	rm -rf  $(PKG_BUILD_DIR)/json-c-0.9/debian/*.EX
	rm -rf  $(PKG_BUILD_DIR)/json-c-0.9/debian/*.install
	cp package-scripts/debian/json-c/* $(PKG_BUILD_DIR)/json-c-0.9/debian
	# Ugly workaround for launchpad lameness
	#rm -rf $(PKG_BUILD_DIR)/json-c-0.9.orig.tar.gz
	#wget --no-clobber -P $(PKG_BUILD_DIR) http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu/pool/main/j/json-c/json-c_0.9.orig.tar.gz
	@cd $(PKG_BUILD_DIR)/json-c-0.9 ; debuild -S
	# to build binaries
	#@cd $(PKG_BUILD_DIR)/json-c-0.9 ; pdebuild
	#@cd $(PKG_BUILD_DIR)/json-c-0.9 ; fakeroot dpkg-buildpackage -b -uc

depends-packages-linux-uci :
	wget --no-clobber -P $(PKG_BUILD_DIR) http://mirror2.openwrt.org/sources/uci-0.7.5.tar.gz
	rm -rf $(PKG_BUILD_DIR)/uci-0.7.5
	rm -rf $(PKG_BUILD_DIR)/uci-0.7.5.orig
	cd $(PKG_BUILD_DIR) ; tar xzvf uci-0.7.5.tar.gz 
	@cd $(PKG_BUILD_DIR)/uci-0.7.5 ; dh_make --library -c LGPL -e antoine@7degrees.co.za --packagename uci --createorig 
	rm -rf  $(PKG_BUILD_DIR)/uci-0.7.5/debian/*.ex
	rm -rf  $(PKG_BUILD_DIR)/uci-0.7.5/debian/*.EX
	rm -rf  $(PKG_BUILD_DIR)/uci-0.7.5/debian/*.install
	cp package-scripts/debian/uci/changelog    $(PKG_BUILD_DIR)/uci-0.7.5/debian
	cp package-scripts/debian/uci/control      $(PKG_BUILD_DIR)/uci-0.7.5/debian
	cp package-scripts/debian/uci/copyright    $(PKG_BUILD_DIR)/uci-0.7.5/debian
	cp package-scripts/debian/uci/rules        $(PKG_BUILD_DIR)/uci-0.7.5/debian
	cp package-scripts/debian/uci/*.install    $(PKG_BUILD_DIR)/uci-0.7.5/debian
	cp package-scripts/debian/uci/Makefile     $(PKG_BUILD_DIR)/uci-0.7.5
	cp package-scripts/debian/uci/Makefile.inc $(PKG_BUILD_DIR)/uci-0.7.5
	# Ugly workaround for launchpad lameness
	#rm -rf $(PKG_BUILD_DIR)/uci-0.7.5.orig.tar.gz
	#wget --no-clobber -P $(PKG_BUILD_DIR) http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu/pool/main/u/uci/uci_0.7.5.orig.tar.gz
	@cd $(PKG_BUILD_DIR)/uci-0.7.5 ; debuild -S
  # to build binaries:
	#@cd $(PKG_BUILD_DIR)/uci-0.7.5 ; pdebuild
	#@cd $(PKG_BUILD_DIR)/uci-0.7.5 ; fakeroot dpkg-buildpackage -b -uc

depends-packages-linux-libmemcached :
	svn checkout http://memcachedb.googlecode.com/svn/clients/libmemcached-0.25-patch $(PKG_BUILD_DIR)/libmemcachedb-0.25
	rm -rf $(PKG_BUILD_DIR)/libmemcachedb-0.25/debian
	@cd $(PKG_BUILD_DIR)/libmemcachedb-0.25 ; dh_make --library -c BSD -e antoine@7degrees.co.za --packagename libmemcachedb --createorig
	rm -rf  $(PKG_BUILD_DIR)/libmemcachedb-0.25/debian/*.ex
	rm -rf  $(PKG_BUILD_DIR)/libmemcachedb-0.25/debian/*.EX
	rm -rf  $(PKG_BUILD_DIR)/libmemcachedb-0.25/debian/*.install
	cp package-scripts/debian/libmemcachedb/* $(PKG_BUILD_DIR)/libmemcachedb-0.25/debian	
	# Ugly workaround for launchpad lameness
	#rm -rf $(PKG_BUILD_DIR)/libmemcachedb-0.25.orig.tar.gz
	#wget -P $(PKG_BUILD_DIR) http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu/pool/main/l/libmemcachedb/libmemcachedb-0.25.orig.tar.gz
	@cd $(PKG_BUILD_DIR)/libmemcachedb-0.25 ; debuild -S
	# to build binaries:
	#@cd $(PKG_BUILD_DIR)/libmemcachedb-0.25 ; pdebuild
	#@cd $(PKG_BUILD_DIR)/libmemcachedb-0.25 ; fakeroot dpkg-buildpackage -b -uc 

depends-packages-linux-libcredis :
	wget --no-clobber -P $(PKG_BUILD_DIR) http://credis.googlecode.com/files/credis-0.2.2.tar.gz
	rm -rf $(PKG_BUILD_DIR)/libcredis-0.2.2/debian
	tar xzvf $(PKG_BUILD_DIR)/credis-0.2.2.tar.gz -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/credis $(PKG_BUILD_DIR)/libcredis-0.2.2
	@cd $(PKG_BUILD_DIR)/libcredis-0.2.2 ; dh_make --library -c BSD -e antoine@7degrees.co.za --packagename libcredis --createorig
	rm -rf  $(PKG_BUILD_DIR)/libcredis-0.2.2/debian/*.ex
	rm -rf  $(PKG_BUILD_DIR)/libcredis-0.2.2/debian/*.EX
	rm -rf  $(PKG_BUILD_DIR)/libcredis-0.2.2/debian/*.install
	cp package-scripts/debian/libcredis/* $(PKG_BUILD_DIR)/libcredis-0.2.2/debian	
	# Ugly workaround for launchpad lameness
	#rm -rf $(PKG_BUILD_DIR)/libcredis-0.2.2.orig.tar.gz
	#wget -P $(PKG_BUILD_DIR) http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu/pool/main/l/libcredis/libcredis-0.2.2.orig.tar.gz
	@cd $(PKG_BUILD_DIR)/libcredis-0.2.2 ; debuild -S
	# to build binaries:
	#@cd $(PKG_BUILD_DIR)/libcredis-0.2.2 ; pdebuild
	#@cd $(PKG_BUILD_DIR)/libcredis-0.2.2 ; fakeroot dpkg-buildpackage -b -uc 


depends-launchpad-linux : #depends-packages-linux-json-c depends-packages-linux-uci depends-packages-linux-libmemcached depends-packages-linux-libcredis
	@echo "Uploading/Refreshing packages to launchpad.net ppa"
	# TODO run lintian & linda
	# TODO read package release # from changelogs
	#dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/json-c_0.9-1_source.changes
	#dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/uci_0.7.5-1_source.changes
	#dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/libmemcachedb_0.25-1_source.changes
	dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/libcredis_0.2.2-1_source.changes	


# - freebsd ----------------------------------------------------------------
freebsd : village-bus
install-freebsd : freebsd install-www
	#chown -R :www $(DASHBOARD_ETC)/config/


# - openwrt ----------------------------------------------------------------
# DEPRECATED
# If you want to build packages for OpenWRT you need to set this to the
# location of a copy of the kamikaze sources or SDK
# KAMIKAZE=/Volumes/afrimesh-dev/ext/kamikaze
#prep-openwrt :
#	rm -f $(KAMIKAZE)/package/afrimesh
#	ln -s $(shell pwd)/package-scripts/openwrt/afrimesh $(KAMIKAZE)/package/afrimesh
#	rm -f $(KAMIKAZE)/target/linux/atheros-mp
#	ln -s $(shell pwd)/upstream/openwrt.meshpotato/target $(KAMIKAZE)/target/linux/atheros-mp

#packages-openwrt :
#	cd $(KAMIKAZE) ; \
#	make package/afrimesh-compile V=99
#	cd $(KAMIKAZE) ; \
#	make package/index

#clean-openwrt :
#	cd $(KAMIKAZE) ; \
#	make package/afrimesh-clean V=99

#distclean-openwrt : clean-openwrt
#	rm -f $(KAMIKAZE)/package/afrimesh
