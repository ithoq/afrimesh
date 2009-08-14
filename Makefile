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



# - configuration ------------------------------------------------------------
VERSION=r492-0.3alpha
RELEASE=1

# If you want to build packages for OpenWRT you need to set this to the
# location of a copy of the kamikaze sources or SDK
KAMIKAZE=/Volumes/afrimesh-dev/ext/kamikaze

# Ubuntu Launchpad Personal Package Archive 
#   _DO_ set this to your personal ppa if you want to build test packages
#   _DON'T_ set this to afrimesh-ppa unless you are planning on doing a
#   release !!!                                   
#PPA=antoine-7degrees-ppa
PPA=afrimesh-ppa

# Some non-standard dependencies are hosted here
#DEPS_URL="https://launchpad.net/~antoine-7degrees/+archive/ppa/+files/"

# - binaries -----------------------------------------------------------------
VILLAGERS=village-bus-batman village-bus-radius village-bus-snmp village-bus-uci



# - commands -----------------------------------------------------------------
INSTALL=cp -rf
MKDIR=mkdir -p
MAKE=make



# - platform detection -------------------------------------------------------
DEPROOT=/usr
WWW_ROOT=$(DESTDIR)/www
DASHBOARD_WWW=$(WWW_ROOT)/afrimesh
DASHBOARD_CGI=$(DESTDIR)/www/cgi-bin
DASHBOARD_ETC=$(DESTDIR)/etc
UNAME = $(shell uname)
ifeq ($(UNAME),Linux)
WWW_ROOT=$(DESTDIR)/var/www
DASHBOARD_WWW=$(WWW_ROOT)/afrimesh
DASHBOARD_CGI=$(DESTDIR)/usr/lib/cgi-bin
endif
ifeq ($(UNAME),FreeBSD)
WWW_ROOT=$(DESTDIR)/usr/local/www/apache22/data
DASHBOARD_WWW=$(WWW_ROOT)/afrimesh
DASHBOARD_CGI=$(DESTDIR)/usr/local/www/apache22/cgi-bin
DEPROOT=/usr/local
MAKE=gmake
endif



# - common -------------------------------------------------------------------
all: 
	export DEPROOT=$(DEPROOT); cd village-bus-batman ; $(MAKE)
	export DEPROOT=$(DEPROOT); cd village-bus-radius ; $(MAKE)
	export DEPROOT=$(DEPROOT); cd village-bus-snmp   ; $(MAKE)
	export DEPROOT=$(DEPROOT); cd village-bus-uci    ; $(MAKE)

install: install-www 
	@if ! test -f $(DASHBOARD_ETC)/config/afrimesh ; then $(MAKE) install-config ; fi

install-www: 
	@echo "Installing dashboard web interface in: $(DASHBOARD_WWW)"
	#rm dashboard/www/javascript
	@if ! test -d $(DASHBOARD_WWW) ; then mkdir -p $(DASHBOARD_WWW) ; fi
	$(INSTALL) dashboard/www/index.html $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/images     $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/style      $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/modules    $(DASHBOARD_WWW)
	$(INSTALL) dashboard/javascript     $(DASHBOARD_WWW) # TODO - crunch all javascript into a single file ?
	@if ! test -f $(WWW_ROOT)/index.html ; then $(INSTALL) dashboard/www/index.redirect.html $(WWW_ROOT)/index.html ; fi # redirect
	@echo "Installing dashboard cgi scripts in: $(DASHBOARD_CGI)"
	@if ! test -d $(DASHBOARD_CGI) ; then mkdir -p $(DASHBOARD_CGI) ; fi
	$(INSTALL) ./package-scripts/openwrt/afrimesh-portal/files/www/cgi-bin/uam.pl $(DASHBOARD_CGI)
	$(INSTALL) dashboard/cgi-bin/ajax-proxy.cgi $(DASHBOARD_CGI)
	$(INSTALL) dashboard/cgi-bin/village-bus-echo $(DASHBOARD_CGI)/village-bus-echo.kml
	chmod 0755 $(DASHBOARD_CGI)/village-bus-echo.kml
	$(INSTALL) dashboard/cgi-bin/village-bus-pmacct.cgi $(DASHBOARD_CGI)
	# TODO $(INSTALL) dashboard/cgi-bin/pmacct $(DASHBOARD_CGI)
	for i in $(VILLAGERS); do echo "Installing: $$i"; $(INSTALL) ./$$i/$$i $(DASHBOARD_CGI); done
	find $(DASHBOARD_WWW) -name "*~"   | xargs rm -f
	find $(DASHBOARD_WWW) -name ".svn" | xargs rm -rf
	find $(DASHBOARD_CGI) -name "*~"   | xargs rm -f
	find $(DASHBOARD_CGI) -name ".svn" | xargs rm -rf
	#cd dashboard/www ; ln -s ../javascript ./javascript # replace symlink

install-config: 
	@echo "Installing configuration files in: $(DASHBOARD_ETC)"
	mkdir -p $(DASHBOARD_ETC)/config
	cat config/dashboard      >  $(DASHBOARD_ETC)/config/afrimesh
	cat config/settings       >> $(DASHBOARD_ETC)/config/afrimesh
	cat config/location       >> $(DASHBOARD_ETC)/config/afrimesh
	cat config/map            >> $(DASHBOARD_ETC)/config/afrimesh
	cat config/gateway        >> $(DASHBOARD_ETC)/config/afrimesh
	cat config/radius         >> $(DASHBOARD_ETC)/config/afrimesh
	cat config/customer-plans >> $(DASHBOARD_ETC)/config/afrimesh
	#cat config/router         >> $(DASHBOARD_ETC)/config/afrimesh
	#cat config/batmand        >> $(DASHBOARD_ETC)/config/afrimesh
	chmod a+rw $(DASHBOARD_ETC)/config/afrimesh

clean : # clean-www
	cd village-bus-batman ; $(MAKE) clean
	cd village-bus-radius ; $(MAKE) clean
	cd village-bus-snmp   ; $(MAKE) clean
	cd village-bus-uci    ; $(MAKE) clean

clean-www: 
	@echo "Cleaning"
	rm -rf $(DASHBOARD_WWW)/index.html
	rm -rf $(DASHBOARD_WWW)/images
	rm -rf $(DASHBOARD_WWW)/style
	rm -rf $(DASHBOARD_WWW)/modules
	rm -rf $(DASHBOARD_WWW)/javascript
	rm -rf $(DASHBOARD_CGI)/*

distclean : clean
	cd village-bus-radius ; make distclean
	cd village-bus-batman ; make distclean

sources : clean
	rm -rf /tmp/afrimesh-$(VERSION).tar.gz
	cp -r ../trunk /tmp/afrimesh-$(VERSION)
	find /tmp/afrimesh-$(VERSION) -name "*~"   | xargs rm -f
	find /tmp/afrimesh-$(VERSION) -name ".svn" | xargs rm -rf
	cd /tmp ; tar cf - afrimesh-$(VERSION) > afrimesh-$(VERSION).tar
	cd /tmp ; gzip afrimesh-$(VERSION).tar
	rm -rf /tmp/afrimesh-$(VERSION)
	ls -al /tmp/afrimesh-$(VERSION).tar.gz



# - linux --------------------------------------------------------------------
# read: https://wiki.ubuntu.com/PackagingGuide/Complete
PKG_BUILD_DIR=/tmp/build
DEPS_HOOK="A70deps"
linux : all
install-linux : install
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
	sudo pbuilder create --distribution hardy \
                       --othermirror "deb http://archive.ubuntu.com/ubuntu hardy main restricted universe multiverse"
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
	rm -f package-scripts/debian/afrimesh-dashboard/*~
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
#	echo "dpkg -i /tmp/uci_0.7.5-1_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/uci-dev_0.7.5-1_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/json-c_0.9-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/json-c-dev_0.9-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/libmemcachedb_0.25-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
#	echo "dpkg -i /tmp/libmemcachedb-dev_0.25-1_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
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
	#@cd $(PKG_BUILD_DIR)/uci-0.7.5 ; pdebuild
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

depends-launchpad-linux : #depends-packages-linux-json-c depends-packages-linux-uci depends-packages-linux-libmemcached
	@echo "Uploading/Refreshing packages to launchpad.net ppa"
	# TODO run lintian & linda
	# TODO read package release # from changelogs
	#dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/json-c_0.9-1_source.changes
	#dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/uci_0.7.5-1_source.changes
	dput -c package-scripts/debian/dput.cf $(PPA) $(PKG_BUILD_DIR)/libmemcachedb_0.25-1_source.changes


# - freebsd ------------------------------------------------------------------
freebsd : village-bus
install-freebsd : freebsd install-www


# - openwrt ------------------------------------------------------------------
prep-openwrt :
	ln -s $(shell pwd)/package-scripts/openwrt/afrimesh $(KAMIKAZE)/package/afrimesh

packages-openwrt :
	cd $(KAMIKAZE) ; \
	make package/afrimesh-compile V=99
	cd $(KAMIKAZE) ; \
	make package/index

clean-openwrt :
	cd $(KAMIKAZE) ; \
	make package/afrimesh-clean V=99

distclean-openwrt : clean-openwrt
	rm -f $(KAMIKAZE)/package/afrimesh






