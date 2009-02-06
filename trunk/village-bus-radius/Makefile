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
DEPROOT=../ext/
JSON_INCLUDES=-I$(DEPROOT)/include
JSON_LDFLAGS=$(DEPROOT)/lib/libjson.a

MYSQL_ADMIN_USERNAME=root
MYSQL_ADMIN_PASSWORD=1nvene0
RADIUS_DATABASE=radius
RADIUS_DATABASE_USERNAME=radius
RADIUS_DATABASE_PASSWORD=radpass
MYSQL_ADMIN=--user=$(MYSQL_ADMIN_USERNAME) --password=$(MYSQL_ADMIN_PASSWORD) 
MYSQL_RADIUS=--user=$(RADIUS_DATABASE_USERNAME) --password=$(RADIUS_DATABASE_PASSWORD)

CC=gcc -std=c99 
INCLUDES=-I. -I../common $(JSON_INCLUDES)
DEFINES=
CFLAGS=$(INCLUDES) $(DEFINES) `mysql_config --cflags` -g
LDFLAGS=`mysql_config --libs` $(JSON_LDFLAGS)


village-bus-radius : json_cgi.o radius_delete.o radius_modify.o radius_new.o radius_list.o radius_who.o main.o 
	$(CC) -o village-bus-radius main.o radius_delete.o radius_modify.o radius_new.o radius_who.o radius_list.o json_cgi.o $(LDFLAGS) 

json_cgi.o : ../common/json_cgi.h ../common/json_cgi.c
	$(CC) $(CFLAGS) -c ../common/json_cgi.c

radius_who.o : village-bus-radius.h radius_who.c 
	$(CC) $(CFLAGS) -c radius_who.c

radius_list.o : village-bus-radius.h radius_list.c 
	$(CC) $(CFLAGS) -c radius_list.c

radius_new.o : village-bus-radius.h radius_new.c 
	$(CC) $(CFLAGS) -c radius_new.c

radius_modify.o : village-bus-radius.h radius_modify.c 
	$(CC) $(CFLAGS) -c radius_modify.c

radius_delete.o : village-bus-radius.h radius_delete.c 
	$(CC) $(CFLAGS) -c radius_delete.c


main.o : village-bus-radius.h main.c 
	$(CC) $(CFLAGS) -c main.c

mysql : 
	# create radius user
	-cat sql/admin-drop.sql  | mysql $(MYSQL_ADMIN)
	-cat sql/admin.sql  | mysql $(MYSQL_ADMIN)
	# create radius database
	-mysqladmin $(MYSQL_ADMIN) drop $(RADIUS_DATABASE)
	-mysqladmin $(MYSQL_ADMIN) create $(RADIUS_DATABASE)
	cat sql/schema.sql | mysql $(MYSQL_RADIUS) $(RADIUS_DATABASE)

mysql-populate : 
	cat sql/defaults.sql  | mysql $(MYSQL_RADIUS) $(RADIUS_DATABASE)
	cat sql/test-data.sql | mysql $(MYSQL_RADIUS) $(RADIUS_DATABASE)

all : village-bus-radius

test-who : village-bus-radius
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"command\" : \"who\" }" ; \
	./village-bus-radius

test-list : village-bus-radius
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"command\" : \"list\" }" ; \
	./village-bus-radius

test-new : village-bus-radius
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"command\" : \"new\", \"username\" : \"token\", \"type\" : \"prepaid\" }" ; \
	./village-bus-radius

test-modify : village-bus-radius
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"command\" : \"modify\" }" ; \
	./village-bus-radius

test-delete : village-bus-radius
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"command\" : \"delete\", \"username\" : \"testing\" }" ; \
	./village-bus-radius


clean:
	rm -f *.o *~ village-bus-radius

distclean: clean
