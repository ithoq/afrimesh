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
JSON_CFLAGS=
JSON_LDFLAGS=$(DEPROOT)/lib/libjson.a

# uci
UCI_CFLAGS=
UCI_LDFLAGS=$(DEPROOT)/lib/libuci.a

# net-snmp
SNMP_CFLAGS=
SNMP_LDFLAGS=-lnetsnmp

#-std=c99 
CC=gcc 
INCLUDES+=-I. -I../common -I../village-bus-snmp -I../village-bus-uci -I$(DEPROOT)/include 
DEFINES+=
CFLAGS+=-g $(DEFINES) $(JSON_CFLAGS) $(UCI_CFLAGS) $(SNMP_CFLAGS)
LDFLAGS+=-L$(DEPROOT)/lib $(JSON_LDFLAGS) $(UCI_LDFLAGS) $(SNMP_LDFLAGS)


objects=json_cgi.o json_rpc.o village-bus-snmp.o village-bus-uci.o village-bus-sys.o village-bus.o main.o 
ifeq ($(TARGET),LINUX)
objects+=fgetln.o
endif

# targets
village-bus : $(objects)
	$(CC) $(CFLAGS) -o village-bus $(objects) $(LDFLAGS) 

# objects
fgetln.o : ../common/fgetln.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/fgetln.c

json_cgi.o : ../common/json_cgi.h ../common/json_cgi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/json_cgi.c

json_rpc.o : json_rpc.h json_rpc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c json_rpc.c

village-bus.o : village-bus.c village-bus.h
	$(CC) $(INCLUDES) $(CFLAGS) -c village-bus.c

village-bus-snmp.o : ../village-bus-snmp/village-bus-snmp.h ../village-bus-snmp/village-bus-snmp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../village-bus-snmp/village-bus-snmp.c

village-bus-uci.o : ../village-bus-uci/village-bus-uci.h ../village-bus-uci/village-bus-uci.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../village-bus-uci/village-bus-uci.c

village-bus-sys.o : ../village-bus-sys/village-bus-sys.h ../village-bus-sys/village-bus-sys.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../village-bus-sys/village-bus-sys.c

main.o : village-bus.h main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c main.c 


# util
clean : 
	rm -f *.o *~ village-bus

# TODO - tests
test : village-bus
	export REQUEST_METHOD="POST" ; \
  echo "{ foo : 'bar' }" | ./village-bus

test-old : village-bus
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"servers\" : \"localhost\", \"set\" : { \"k1\" : \"both1\", \"a funky\" : \"both2\", \"a.funky.key\" : \"both3\" }, \"get\" : { \"k1\" : \"k1\", \"key2\" : \"k2\", \"k3\" : \"k3\" } }" ; \
	./village-bus

test-json :

test-jsonp :

RPC="id" : 123, "version" : "2.0"
SET1={"config":"afrimesh","section":"potato","option":"asterisk","value":"127.0.0.2"}
SET2={"config":"afrimesh","section":"potato","option":"trunkcalls","value":"true"}

test-jsonrpc-uci :
	#./village-bus /uci '{ $(RPC), "method" : "show", "params" : [ "" ] }'	
	./village-bus /uci '{ $(RPC), "method" : "show", "params" : [ "afrimesh.location" ] }'
	#./village-bus /uci '{ $(RPC), "method" : "set", "params" : [ [ $(SET1), $(SET2) ] ] }'	


HOST="192.168.20.1", "public"
test-jsonrpc-snmp-walk : village-bus
	#snmpwalk -v 2c -c public 192.168.20.1 1.3.6.1.2.1.1.1
	#./village-bus /snmp '{ $(RPC), "method" : "walk", "params" : [ $(HOST), "1.3.6.1.2.1.1.1" ] }'
	./village-bus /snmp '{ $(RPC), "method" : "walk", "params" : [ $(HOST), ".1.3.6.1.2.1.2.2.1.2" ] }'

test-jsonrpc-snmp-get : village-bus
	#./village-bus /snmp '{ $(RPC), "method" : "get", "params" : [ $(HOST), ["SNMPv2-MIB::sysDescr"] ] }'		
	#./village-bus /snmp '{ $(RPC), "method" : "get", "params" : [ $(HOST), [".1.3.6.1.2.1.1.1"] ] }'	
	./village-bus /snmp '{ $(RPC), "method" : "get", "params" : [ $(HOST), [".1.3.6.1.2.1.2.2.1.10.1",".1.3.6.1.2.1.2.2.1.16.1"] ] }'				

test-jsonrpc-syslog :
	./village-bus /sys '{ $(RPC), "method" : "syslog", "params" : [ 3 ] }'

test-jsonrpc-uname :
	./village-bus /sys '{ $(RPC), "method" : "uname", "params" : [ ] }'


# run /syslog '{ "id" : 123, "version" : "2.0", "method" : "read", "params" : [ 3 ] }'