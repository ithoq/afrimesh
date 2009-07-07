FREERADIUS_SOURCES=/Volumes/afrimesh-dev/ext-sources/freeradius-server-2.1.6
FREERADIUS_VERSION=2.1.6
FREERADIUS_DESTDIR=/usr/lib/freeradius

CC      = gcc
INSTALL = install
CFLAGS  =  -DNDEBUG -Wall -I/usr/include -I/usr/local/include \
           -I$(FREERADIUS_SOURCES)/src
LDFLAGS =  

all :  rlm_memcachedb/rlm_memcachedb.o rlm_memcachedb/rlm_memcachedb-$(FREERADIUS_VERSION).so

rlm_memcachedb/rlm_memcachedb.o : rlm_memcachedb/rlm_memcachedb.c
	cd rlm_memcachedb ; $(CC) -g -fPIC -DPIC -c $(CFLAGS) rlm_memcachedb.c 

rlm_memcachedb/rlm_memcachedb-$(FREERADIUS_VERSION).so : rlm_memcachedb/rlm_memcachedb.o
	cd rlm_memcachedb ; $(CC) -g -shared -Wl,-soname,rlm_memcachedb-$(FREERADIUS_VERSION).so \
                            -o rlm_memcachedb-$(FREERADIUS_VERSION).so rlm_memcachedb.o $(LDFLAGS)

install : all
	$(INSTALL) rlm_memcachedb/rlm_memcachedb-$(FREERADIUS_VERSION).so $(FREERADIUS_DESTDIR)
	#ln -fs rlm_memcachedb/rlm_memcachedb-$(FREERADIUS_VERSION).so $(FREERADIUS_DESTDIR)/rlm_memcachedb.so

clean:
	rm rlm_memcachedb/*~ rlm_memcachedb/rlm_memcachedb*.o rlm_memcachedb/rlm_memcachedb*.so