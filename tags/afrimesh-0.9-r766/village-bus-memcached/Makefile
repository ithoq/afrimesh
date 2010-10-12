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


# json
JSON_INCLUDES=-I$(DEPROOT)/include
JSON_LDFLAGS=$(DEPROOT)/lib/libjson.a

MEMCACHED_INCLUDES=-I$(DEPROOT)/include
MEMCACHED_LDFLAGS=$(DEPROOT)/lib/libmemcached.a -lm

CC=gcc -std=c99 
INCLUDES=-I. -I../common -I/usr/local/include $(JSON_INCLUDES) $(MEMCACHED_INCLUDES)
DEFINES=
CFLAGS=$(INCLUDES) $(DEFINES) -g
LDFLAGS=$(JSON_LDFLAGS) $(MEMCACHED_LDFLAGS) -L/usr/local/lib


village-bus-memcached : json_cgi.o village-bus-memcached.o main.o
	$(CC) -o village-bus-memcached *.o $(LDFLAGS) 

json_cgi.o : ../common/json_cgi.h ../common/json_cgi.c
	$(CC) $(CFLAGS) -c ../common/json_cgi.c

village-bus-memcached.o : village-bus-memcached.c village-bus-memcached.h
	$(CC) $(CFLAGS) -c village-bus-memcached.c

main.o : village-bus-memcached.h main.c
	$(CC) $(CFLAGS) -c main.c 

clean : 
	rm -f *.o *~ village-bus-memcached

test-get : village-bus-memcached
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"servers\" : \"localhost\", \"get\" : { \"k1\" : \"k1\", \"key2\" : \"k2\", \"k3\" : \"k3\" } }" ; \
	./village-bus-memcached

test-set : village-bus-memcached
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"servers\" : \"localhost\", \"set\" : { \"k1\" : \"v1\", \"k2\" : \"v2\", \"k3\" : \"v3\" } }" ; \
	./village-bus-memcached

test-both : village-bus-memcached
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"servers\" : \"localhost\", \"set\" : { \"k1\" : \"both1\", \"a funky\" : \"both2\", \"a.funky.key\" : \"both3\" }, \"get\" : { \"k1\" : \"k1\", \"key2\" : \"k2\", \"k3\" : \"k3\" } }" ; \
	./village-bus-memcached


# CFLAGS=-I/opt/local/include/db47 ./configure --with-libevent=/opt/local --with-bdb=/opt/local --enable-threads
MEMCACHEDB=/Volumes/afrimesh-dev/ext-sources/memcachedb-svn/memcachedb
MEMCACHEDB_OPTIONS=-p11211 -r -H /tmp/11211 -N -v
start-memcachedb :
	$(MEMCACHEDB) $(MEMCACHEDB_OPTIONS)