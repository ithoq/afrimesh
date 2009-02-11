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


CC=gcc 
#-std=c99 breaks linux build
INCLUDES=-I. -I../common -I/usr/local/include $(JSON_INCLUDES)
DEFINES=
CFLAGS=$(INCLUDES) $(DEFINES) -g
LDFLAGS=$(JSON_LDFLAGS) -L/usr/local/lib


village-bus-snmp : json_cgi.o village-bus-snmp.o
	$(CC) -o village-bus-snmp *.o $(LDFLAGS) -lnetsnmp

json_cgi.o : ../common/json_cgi.h ../common/json_cgi.c
	$(CC) $(CFLAGS) -c ../common/json_cgi.c

village-bus-snmp.o : village-bus-snmp.c
	$(CC) -c village-bus-snmp.c $(INCLUDES)


clean : 
	rm -f *.o *~ village-bus-snmp

