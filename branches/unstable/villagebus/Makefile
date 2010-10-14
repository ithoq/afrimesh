# Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
# Copyright (C) 2008-2010 Meraka Institute of the CSIR
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


# - version ----------------------------------------------------------------
ifndef VERSION
REVISION=$(shell svnversion . -n)
	ifeq (,$(REVISION))
	REVISION="unknown"
	endif
	VERSION=$(REVISION)-i-think
endif


# - core -------------------------------------------------------------------
core=safelib.o util.o object.o fexpression.o


# - modules ----------------------------------------------------------------
modules=mod_config.o mod_sys.o mod_telephony.o mod_accounting.o mod_http.o 
modules+=mod_package.o
modulelibs=$(DEPROOT)/lib/libuci.a 

ifeq ($(ENABLE_MOD_PROVISION),1)
modules+=mod_provision.o 
CFLAGS+=-DENABLE_MOD_PROVISION
ENABLE_MOD_REDIS=1
endif

ifeq ($(ENABLE_MOD_REDIS),1)
modules+=mod_db.o 
modulelibs+=$(DEPROOT)/lib/libcredis.a
CFLAGS+=-DENABLE_MOD_REDIS
endif

ifeq ($(ENABLE_MOD_SNMP),1)
modules+=mod_snmp.o 
modulelibs+=-lnetsnmp
CFLAGS+=-DENABLE_MOD_SNMP
endif


# - build flags ------------------------------------------------------------
ECHO=/bin/echo -n
CC=gcc 
INCLUDES+=-I. -I.. -I../common -I modules -I$(DEPROOT)/include 
DEFINES+=
CFLAGS+=-g $(DEFINES) -DAFRIMESH_VERSION="\"$(VERSION)\"" 
# -std=c99
LDFLAGS+=-L$(DEPROOT)/lib $(modulelibs) $(DEPROOT)/lib/libjson.a


# - objects ----------------------------------------------------------------
objects=$(core) cgi.o jsonrpc.o $(modules) villagebus.o main.o 

ifeq ($(FLAVOR),LINUX)
objects+=fgetln.o
endif


# - targets ----------------------------------------------------------------
all : villagebus

villagebus : $(objects)
	$(CC) $(CFLAGS) -o villagebus $(objects) $(LDFLAGS) 

examples : $(core) examples/types.c
	$(CC) $(INCLUDES) $(CFLAGS) -o examples/types examples/types.c $(core) $(LDFLAGS)

clean : 
	rm -f *.o *~ villagebus urldecode


# - objects ----------------------------------------------------------------
safelib.o : ../common/safelib.c ../common/safelib.h
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/safelib.c

fgetln.o : ../common/fgetln.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/fgetln.c

util.o : ../common/util.h ../common/util.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/util.c

object.o : ../common/object.h ../common/object.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/object.c

fexpression.o : ../common/fexpression.h ../common/fexpression.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/fexpression.c

cgi.o : ../common/cgi.h ../common/cgi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/cgi.c

jsonrpc.o : ../common/jsonrpc.h ../common/jsonrpc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/jsonrpc.c

mod_db.o : modules/mod_db.h modules/mod_db.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_db.c

mod_http.o : modules/mod_http.h modules/mod_http.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_http.c

mod_provision.o : modules/mod_provision.h modules/mod_provision.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_provision.c

mod_config.o : modules/mod_config.h modules/mod_config.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_config.c

mod_sys.o : modules/mod_sys.h modules/mod_sys.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_sys.c

mod_telephony.o : modules/mod_telephony.h modules/mod_telephony.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_telephony.c

mod_accounting.o : modules/mod_accounting.h modules/mod_accounting.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_accounting.c

mod_package.o : modules/mod_package.h modules/mod_package.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_package.c

mod_snmp.o : modules/mod_snmp.h modules/mod_snmp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_snmp.c

villagebus.o : villagebus.c villagebus.h
	$(CC) $(INCLUDES) $(CFLAGS) -c villagebus.c

main.o : villagebus.h main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c main.c 


# - test-db ----------------------------------------------------------------
version="r133"
# TODO uci get mesh address
self=192.168.20.105
gateway=192.168.20.1
# timestamp="\/Date($(shell date +%s)000)\/"
timestamp=$(shell date +%s)000
uname=`uname`
ping=$(shell ping -c 1 192.168.20.1 | grep from | awk '{ print $$7 }' | cut -d = -f 2)
path="/root/db/test/$(self)"
db_json="{ \
  'timestamp' : $(timestamp),	 \
  'version' : '$(version)',	\
  'uname' : '$(uname)', \
  'gateway' : { '$(gateway)' : $(ping) } }"

test-db :
	$(ECHO) $(db_json) | ./villagebus POST "$(path)"
	./villagebus GET $(path)

test-http-db : villagebus	
	@echo
	export REQUEST_METHOD="PUT" ; \
	export PATH_INFO=$(path)   ; \
  $(ECHO) $(db_json) | ./villagebus 
	export REQUEST_METHOD="GET" ; \
	export PATH_INFO=$(path)   ; \
  ./villagebus 


# - test-config ------------------------------------------------------------
config_json="[ { 'config' : 'network', 'section' : 'wifi0', 'option' : 'ipaddr', 'value' : '1.1.1.2' } ]"

test-config :
	#$(ECHO) $(config_json) | ./villagebus PUT "/config"
	@#./villagebus GET "/root/config/network/wifi0/ipaddr"
	./villagebus GET "/root/config"


# DEPROOT=/Volumes/afrimesh-dev/ext CFLAGS=-m32 make clean villagebus test-config
# DEPROOT=/usr/local gmake clean villagebus test-config
# DEPROOT=/usr gmake clean villagebus test-config



