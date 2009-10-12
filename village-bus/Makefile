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

#-std=c99 
CC=gcc 
INCLUDES=-I. -I../common -I../village-bus-snmp $(JSON_INCLUDES) 
DEFINES=
CFLAGS+=$(DEFINES) -g
LDFLAGS=$(JSON_LDFLAGS) -lnetsnmp


# targets
village-bus : json_cgi.o json_rpc.o village-bus-snmp.o main.o 
	$(CC) $(CFLAGS) -o village-bus *.o $(LDFLAGS) 


# objects
json_cgi.o : ../common/json_cgi.h ../common/json_cgi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../common/json_cgi.c

json_rpc.o : json_rpc.h json_rpc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c json_rpc.c

#village-bus.o : village-bus.c village-bus.h
#	$(CC) $(CFLAGS) -c village-bus.c

village-bus-snmp.o : ../village-bus-snmp/village-bus-snmp.h ../village-bus-snmp/village-bus-snmp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c ../village-bus-snmp/village-bus-snmp.c

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

test-jsonrpc-uci :
	./village-bus uci '{ "id" : 123, "version" : "2.0", "method" : "show", "params" : []  }'

RPC="id" : 123, "version" : "2.0"
HOST="192.168.20.1", "public"

test-jsonrpc-snmp-walk : village-bus
	#snmpwalk -v 2c -c public 192.168.20.1 1.3.6.1.2.1.1.1
	./village-bus /snmp '{ $(RPC), "method" : "walk", "params" : [ $(HOST), ["1.3.6.1.2.1.1.1"] ] }'
	./village-bus /snmp '{ $(RPC), "method" : "walk", "params" : [ $(HOST), ["1.3.6.1.2.1.1.1", "1.3.6.1.2.1.2.2.1.10", "1.3.6.1.2.1.2.2.1.16"] ] }'	

test-jsonrpc-snmp-get : village-bus
	#./village-bus /snmp '{ $(RPC), "method" : "get", "params" : [ $(HOST), ["SNMPv2-MIB::sysDescr"] ] }'		
	#./village-bus /snmp '{ $(RPC), "method" : "get", "params" : [ $(HOST), [".1.3.6.1.2.1.1.1"] ] }'	
	./village-bus /snmp '{ $(RPC), "method" : "get", "params" : [ $(HOST), [".1.3.6.1.2.1.2.2.1.10.1",".1.3.6.1.2.1.2.2.1.16.1"] ] }'				

