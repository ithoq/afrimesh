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

# version
ifndef VERSION
REVISION=$(shell svnversion . -n)
	ifeq (,$(REVISION))
	REVISION="unknown"
	endif
	VERSION=$(REVISION)-i-think
endif

# modules
modules=mod_db.o mod_config.o
modulelibs=$(DEPROOT)/lib/libcredis.a $(DEPROOT)/lib/libuci.a 

# build flags
CC=gcc 
INCLUDES+=-I. -I.. -I../common -I modules -I$(DEPROOT)/include 
DEFINES+=
CFLAGS+=-g $(DEFINES) -DAFRIMESH_VERSION="\"$(VERSION)\""
LDFLAGS+=-L$(DEPROOT)/lib $(modulelibs) $(DEPROOT)/lib/libjson.a

# tools
ECHO=/bin/echo -n

# objects
objects=safelib.o util.o object.o fexpression.o cgi.o jsonrpc.o $(modules) villagebus.o main.o 

# build configuration
ifeq ($(FLAVOR),LINUX)
objects+=fgetln.o
endif


# targets
all : villagebus

villagebus : $(objects)
	$(CC) $(CFLAGS) -o villagebus $(objects) $(LDFLAGS) 


# objects
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

jsonrpc.o : ../common/cgi.h ../common/jsonrpc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/jsonrpc.c

mod_db.o : modules/mod_db.h modules/mod_db.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_db.c

mod_config.o : modules/mod_config.h modules/mod_config.c
	$(CC) $(INCLUDES) $(CFLAGS) -c modules/mod_config.c

villagebus.o : villagebus.c villagebus.h
	$(CC) $(INCLUDES) $(CFLAGS) -c villagebus.c

main.o : villagebus.h main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c main.c 


# util
clean : 
	rm -f *.o *~ villagebus urldecode


# - test-http --------------------------------------------------------------
test-http : villagebus
	export REQUEST_METHOD="POST" ; \
	export PATH_INFO=$(path)   ; \
	export QUERY_STRING="plink=plonk&foo=bar" ; \
  $(ECHO) $(json_db) | ./villagebus 
	export REQUEST_METHOD="GET" ; \
	export PATH_INFO=$(path)   ; \
  ./villagebus 


# - test-db ----------------------------------------------------------------
version="r133"
# TODO uci get mesh address
self=192.168.20.105
gateway=192.168.20.1
# timestamp="\/Date($(shell date +%s)000)\/"
timestamp=$(shell date +%s)000
uname=`uname`
ping=$(shell ping -c 1 192.168.20.1 | grep from | awk '{ print $$7 }' | cut -d = -f 2)
path="/root/db/status/$(self)"
db_json="{ \
  'timestamp' : $(timestamp),	 \
  'version' : '$(version)',	\
  'uname' : '$(uname)', \
  'gateway' : { '$(gateway)' : $(ping) } }"

test-db :
	$(ECHO) $(db_json) | ./villagebus POST "$(path)"
	./villagebus GET $(path)


# - test-config ------------------------------------------------------------
config_json="[ { 'config' : 'network', 'section' : 'wifi0', 'option' : 'ipaddr', 'value' : '1.1.1.2' } ]"

test-config :
	$(ECHO) $(config_json) | ./villagebus PUT "/config"
	@#./villagebus GET "/root/config/network/wifi0/ipaddr"
	./villagebus GET "/root/config"



# DEPROOT=/Volumes/afrimesh-dev/ext CFLAGS=-m32 make clean villagebus test-http
# DEPROOT=/usr/local gmake clean villagebus test-jsonrpc-acct

