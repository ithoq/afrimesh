**Installing libmemcached**
  * Check out libmemcached source from http://memcachedb.googlecode.com/svn/clients/libmemcached-0.25-patch/
_It is important to use this patched version of libmemcached, as it is currently the only release to support the "rget" command_
```
svn checkout http://memcachedb.googlecode.com/svn/clients/libmemcached-0.25-patch/
```

  * In the libmemcached-0.25-patch directory, run the configuration
```
./configure
```

  * In the libmemcached-0.25-patch directory, "make install" to install libmemcached
```
sudo make install
```

**Installing memcachedb and setting up a server**

  * Ensure that Berkeley DB 4.7 or later and
> libevent 1.3e or later are installed

  * Check out memcachedb source from http://memcachedb.googlecode.com/svn/trunk/
```
svn checkout http://memcachedb.googlecode.com/svn/trunk/ memcachedb-read-only
```

  * In the memcachedb-read-only directory, run the configuration
```
./configure
```

  * In the memcachedb-read-only directory, "make install" to install memcachedb
```
sudo make install
```

  * To run a memcachedb server:
```
sudo su
kilall memcachedb
memcachedb -p11211 -u root -r -H/tmp/11211 -N -v
```