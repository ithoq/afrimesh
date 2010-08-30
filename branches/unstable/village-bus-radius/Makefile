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



JSON_LDFLAGS=$(DEPROOT)/lib/libjson.a
UCI_LDFLAGS=$(DEPROOT)/lib/libuci.a
MEMCACHEDB_LDFLAGS=$(DEPROOT)/lib/libmemcached.a


MYSQL_ADMIN_USERNAME=root
MYSQL_ADMIN_PASSWORD=1nvene0
RADIUS_DATABASE=radius
RADIUS_DATABASE_USERNAME=radius
RADIUS_DATABASE_PASSWORD=radpass
MYSQL_ADMIN=--user=$(MYSQL_ADMIN_USERNAME) --password=$(MYSQL_ADMIN_PASSWORD) 
MYSQL_RADIUS=--user=$(RADIUS_DATABASE_USERNAME) --password=$(RADIUS_DATABASE_PASSWORD)

CC=gcc -std=c99 
INCLUDES=-I. -I../common -I$(DEPROOT)/include
DEFINES=
CFLAGS=$(INCLUDES) $(DEFINES) `mysql_config --cflags` -g
LDFLAGS=-L$(DEPROOT)/lib -lm `mysql_config --libs` $(JSON_LDFLAGS) $(UCI_LDFLAGS) $(MEMCACHEDB_LDFLAGS)

OBJECTS=radius_delete_mysql.o radius_modify_mysql.o radius_new_mysql.o radius_list_mysql.o \
        radius_delete_memcachedb.o radius_modify_memcachedb.o radius_new_memcachedb.o radius_list_memcachedb.o \
        radius_who.o json_cgi.o uci_util.o main.o 

village-bus-radius : $(OBJECTS) 
	$(CC) -o village-bus-radius $(OBJECTS) $(LDFLAGS) 

uci_util.o : ../common/uci_util.h ../common/uci_util.c
	$(CC) $(CFLAGS) -c ../common/uci_util.c

json_cgi.o : ../common/json_cgi.h ../common/json_cgi.c
	$(CC) $(CFLAGS) -c ../common/json_cgi.c

radius_who.o : village-bus-radius.h radius_who.c 
	$(CC) $(CFLAGS) -c radius_who.c 

# mysql support
radius_list_mysql.o : village-bus-radius.h mysql/radius_list.c 
	$(CC) $(CFLAGS) -c mysql/radius_list.c -o radius_list_mysql.o

radius_new_mysql.o : village-bus-radius.h mysql/radius_new.c 
	$(CC) $(CFLAGS) -c mysql/radius_new.c -o radius_new_mysql.o

radius_modify_mysql.o : village-bus-radius.h mysql/radius_modify.c 
	$(CC) $(CFLAGS) -c mysql/radius_modify.c -o radius_modify_mysql.o

radius_delete_mysql.o : village-bus-radius.h mysql/radius_delete.c 
	$(CC) $(CFLAGS) -c mysql/radius_delete.c -o radius_delete_mysql.o

# memcachedb support
radius_list_memcachedb.o : village-bus-radius.h memcachedb/radius_list.c 
	$(CC) $(CFLAGS) -c memcachedb/radius_list.c -o radius_list_memcachedb.o

radius_new_memcachedb.o : village-bus-radius.h memcachedb/radius_new.c 
	$(CC) $(CFLAGS) -c memcachedb/radius_new.c -o radius_new_memcachedb.o

radius_modify_memcachedb.o : village-bus-radius.h memcachedb/radius_modify.c 
	$(CC) $(CFLAGS) -c memcachedb/radius_modify.c -o radius_modify_memcachedb.o

radius_delete_memcachedb.o : village-bus-radius.h memcachedb/radius_delete.c
	$(CC) $(CFLAGS) -c memcachedb/radius_delete.c -o radius_delete_memcachedb.o

main.o : village-bus-radius.h main.c 
	$(CC) $(CFLAGS) -c main.c

mysql-init : 
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

mysql-login :
	mysql $(MYSQL_RADIUS) $(RADIUS_DATABASE)

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
	export QUERY_STRING="{ \"command\" : \"modify\", \"username\": \"Joan\", \"new_username\": \"Penny\", \"new_password\": \"plink\", \"new_type\": \"flatrate\" }" ; \
	./village-bus-radius

test-delete : village-bus-radius
	export REQUEST_METHOD="GET" ; \
	export QUERY_STRING="{ \"command\" : \"delete\", \"username\" : \"testing\" }" ; \
	./village-bus-radius


clean:
	rm -f *.o *~ village-bus-radius

distclean: clean
