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
VERSION=0.1

# If you want to build packages for OpenWRT you need to set this to the
# location of a copy of the kamikaze sources
KAMIKAZE=/Volumes/afrimesh-dev/ext/kamikaze


# - binaries -----------------------------------------------------------------
VILLAGERS=village-bus-batman village-bus-radius village-bus-snmp village-bus-uci

# - commands -----------------------------------------------------------------
INSTALL=cp -rf
MKDIR=mkdir -p
MAKE=make

# - platform detection -------------------------------------------------------
DEPROOT=/usr
DASHBOARD_WWW=$(DESTDIR)/www
DASHBOARD_CGI=$(DESTDIR)/www/cgi-bin
UNAME = $(shell uname)
ifeq ($(UNAME),Linux)
DASHBOARD_WWW=$(DESTDIR)/var/www
DASHBOARD_CGI=$(DESTDIR)/usr/lib/cgi-bin
DEPROOT=/usr/local
endif
ifeq ($(UNAME),FreeBSD)
DASHBOARD_WWW=$(DESTDIR)/usr/local/www/apache22/data
DASHBOARD_CGI=$(DESTDIR)/usr/local/www/apache22/cgi-bin
DEPROOT=/usr/local
MAKE=gmake
endif



# TODO - crunch all javascript into a single file ?

# - common -------------------------------------------------------------------
all : 
	export DEPROOT=$(DEPROOT); cd village-bus-batman ; $(MAKE)
	export DEPROOT=$(DEPROOT); cd village-bus-radius ; $(MAKE)
	export DEPROOT=$(DEPROOT); cd village-bus-snmp   ; $(MAKE)
	export DEPROOT=$(DEPROOT); cd village-bus-uci    ; $(MAKE)

install : install-www

install-www:
	@echo "Installing dashboard web interface in: $(DASHBOARD_WWW)"
	#rm dashboard/www/javascript
	[ ! -d $(DASHBOARD_WWW) ] && mkdir -p $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/index.html $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/images     $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/style      $(DASHBOARD_WWW)
	$(INSTALL) dashboard/www/modules    $(DASHBOARD_WWW)
	$(INSTALL) dashboard/javascript     $(DASHBOARD_WWW)
	@echo "Installing dashboard cgi scripts in: $(DASHBOARD_CGI)"
	[ ! -d $(DASHBOARD_CGI) ] && mkdir -p $(DASHBOARD_CGI)
	$(INSTALL) ./package-scripts/openwrt/afrimesh-portal/files/www/cgi-bin/uam.pl $(DASHBOARD_CGI)
	$(INSTALL) dashboard/cgi-bin/ajax-proxy.cgi $(DASHBOARD_CGI)
	for i in $(VILLAGERS); do echo "Installing: $$i"; $(INSTALL) ./$$i/$$i $(DASHBOARD_CGI); done
	find $(DASHBOARD_WWW) -name "*~"   | xargs rm -f
	find $(DASHBOARD_WWW) -name ".svn" | xargs rm -rf
	find $(DASHBOARD_CGI) -name "*~"   | xargs rm -f
	find $(DASHBOARD_CGI) -name ".svn" | xargs rm -rf
	#cd dashboard/www ; ln -s ../javascript ./javascript # replace symlink

clean : clean-www
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
#DEPS_URL="http://afrimesh.googlecode.com/files"
DEPS_URL="http://l-cube.artifactual.org.za/~antoine/binaries"
DEPS_HOOK="A70deps"
linux : all
install-linux : install
clean-linux : clean
	rm -rf $(PKG_BUILD_DIR)

packages-linux : prep-linux
	#@echo "Building Debian/Ubuntu source packages"
	#cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; debuild -S
	#cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; pdebuild -S
	@echo "Building Debian/Ubuntu binary packages"
	#cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; sudo pbuilder build ../*.dsc
	cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; pdebuild 
	@echo "Built: "
	ls -al /var/cache/pbuilder/result
prep-linux : clean-linux sources hooks-linux
	@echo "Initializing linux package scripts for build"
	mkdir -p $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR) ; cp /tmp/afrimesh-$(VERSION).tar.gz .
	cd $(PKG_BUILD_DIR) ; tar xzvf afrimesh-$(VERSION).tar.gz
	cd $(PKG_BUILD_DIR) ; mv afrimesh-$(VERSION) afrimesh-dashboard-$(VERSION)
	cd $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION) ; dh_make -e antoine@7degrees.co.za -s --createorig
	rm -rf $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION)/debian/*
	rm -f package-scripts/debian/afrimesh-dashboard/*~
	cp -a package-scripts/debian/afrimesh-dashboard/* $(PKG_BUILD_DIR)/afrimesh-dashboard-$(VERSION)/debian
hooks-linux :
	@echo "Installing hooks to install unofficial packages needed to build Afrimesh"
	mkdir -p $(PKG_BUILD_DIR)
	rm -f ~/.pbuilderrc
	echo "HOOKDIR=$(PKG_BUILD_DIR)/hook.d" >> ~/.pbuilderrc
	mkdir -p $(PKG_BUILD_DIR)/hook.d
	echo "#!/bin/sh" > $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
	echo "echo \"Installing unofficial dependencies for Afrimesh\"" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
	echo "cd /tmp ; wget $(DEPS_URL)/libuci-0.7.3_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
	echo "cd /tmp ; wget $(DEPS_URL)/libjson-0.8_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
	echo "dpkg -i /tmp/libuci-0.7.3_i386.deb" >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
	echo "dpkg -i /tmp/libjson-0.8_i386.deb"  >> $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)
	chmod 0755 $(PKG_BUILD_DIR)/hook.d/$(DEPS_HOOK)


# - freebsd ------------------------------------------------------------------
freebsd : village-bus
install-freebsd : freebsd install-www



# - openwrt ------------------------------------------------------------------
prep-openwrt :
	ln -s $(shell pwd)/package-scripts/openwrt/afrimesh-base $(KAMIKAZE)/package/afrimesh-base
	ln -s $(shell pwd)/package-scripts/openwrt/afrimesh-config $(KAMIKAZE)/package/afrimesh-config
	ln -s $(shell pwd)/package-scripts/openwrt/afrimesh-portal $(KAMIKAZE)/package/afrimesh-portal
	ln -s $(shell pwd)/package-scripts/openwrt/afrimesh-webif $(KAMIKAZE)/package/afrimesh-webif
	#ln -s $(shell pwd)/package-scripts/openwrt/afrimesh-webif $(KAMIKAZE)/package/afrimesh-dashboard

packages-openwrt :
	cd $(KAMIKAZE) ; \
	make package/afrimesh-base-compile V=99
	cd $(KAMIKAZE) ; \
	make package/afrimesh-config-compile V=99
	cd $(KAMIKAZE) ; \
	make package/afrimesh-portal-compile V=99
	cd $(KAMIKAZE) ; \
	make package/afrimesh-webif-compile V=99
	#cd $(KAMIKAZE) ; \
	#make package/afrimesh-dashboard-compile V=99
	cd $(KAMIKAZE) ; \
	make package/index

clean-openwrt :
	cd $(KAMIKAZE) ; \
	make package/afrimesh-base-clean V=99
	cd $(KAMIKAZE) ; \
	make package/afrimesh-config-clean V=99
	cd $(KAMIKAZE) ; \
	make package/afrimesh-portal-clean V=99
	cd $(KAMIKAZE) ; \
	make package/afrimesh-webif-clean V=99
	#cd $(KAMIKAZE) ; \
	#make package/afrimesh-dashboard-clean V=99

distclean-openwrt : clean-openwrt
	rm -f $(KAMIKAZE)/package/afrimesh-base
	rm -f $(KAMIKAZE)/package/afrimesh-config
	rm -f $(KAMIKAZE)/package/afrimesh-portal
	rm -f $(KAMIKAZE)/package/afrimesh-webif
	#rm -f $(KAMIKAZE)/package/afrimesh-dashboard



# - common ------------------------------------------------------------------


