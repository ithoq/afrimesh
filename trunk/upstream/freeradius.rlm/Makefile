FREERADIUS_SOURCES=/home/sprinter/Documents/freeradius-server-2.1.6
FREERADIUS_VERSION=2.1.6
FREERADIUS_DESTDIR=/usr/local/lib

JSON_INCLUDES=-I/usr/include
JSON_LDFLAGS=-L/usr/lib -ljson

CC      = gcc
INCLUDES=-I. -I$(FREERADIUS_SOURCES)/src $(JSON_INCLUDES) -I/usr/include -I/usr/local/include
INSTALL = install
CFLAGS  =  -DNDEBUG -Wall $(INCLUDES) 
LDFLAGS =  -L/usr/local/lib -lm -lmemcached $(JSON_LDFLAGS)

all :  rlm_memcachedb/rlm_memcachedb.o rlm_memcachedb/rlm_memcachedb-$(FREERADIUS_VERSION).so

rlm_memcachedb/rlm_memcachedb.o : rlm_memcachedb/rlm_memcachedb.c
	cd rlm_memcachedb ; $(CC) -g -fPIC -DPIC -c $(CFLAGS) rlm_memcachedb.c 

rlm_memcachedb/rlm_memcachedb-$(FREERADIUS_VERSION).so : rlm_memcachedb/rlm_memcachedb.o
	cd rlm_memcachedb ; $(CC) -g -shared -Wl,-soname,rlm_memcachedb-$(FREERADIUS_VERSION).so \
                            -o rlm_memcachedb-$(FREERADIUS_VERSION).so rlm_memcachedb.o $(LDFLAGS)

install : all
	$(INSTALL) rlm_memcachedb/rlm_memcachedb-$(FREERADIUS_VERSION).so $(FREERADIUS_DESTDIR)
	ln -fs $(FREERADIUS_DESTDIR)/rlm_memcachedb-$(FREERADIUS_VERSION).so $(FREERADIUS_DESTDIR)/rlm_memcachedb.so

clean:
	rm -f rlm_memcachedb/*~ rlm_memcachedb/rlm_memcachedb*.o rlm_memcachedb/rlm_memcachedb*.so
