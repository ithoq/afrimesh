Instructions for preparing an afrimesh development environment under Ubuntu

# Dependencies #

Run as required:

  * httpd:
```
  apt-get install apache2
```
  * net-snmp:
```
  apt-get install snmp
  apt-get install snmpd
  apt-get install libsnmp-dev 
```
  * pmacct
```
  apt-get install pmacct
```
  * redis
```
  apt-get install redis-server
```


...and some custom packages:

```
  apt-add-repository ppa:antoine-7degrees/ppa
  apt-get update
  apt-get install json-c-dev 
  apt-get install uci-dev 
  apt-get install libcredis-dev
```


Finally:

  * Lua:
```
    apt-get install lua5.1 luarocks liblua5.1-socket2 liblua5.1-json liblua5.1-logging liblua5.1-uci
```


# 64-bit Ubuntu #
Afrimesh will compile on 64-bit Ubuntu, but it won't work. A workaround to get it working, is to install the 32-bit libraries and compile afrimesh as 32-bit binaries.

First, install the 32-bit libraries that is already in the Ubuntu 64-bit repository:
```
  sudo apt-get install ia32-libs libc6-dev-i386
```

Next, install getlibs to make the installation of 32-bit libraries that is not included in the Ubuntu 64-bit repository, a bit easier:
```
   wget http://frozenfox.freehostia.com/cappy/getlibs-all.deb
   sudo dpkg -i getlibs-all.deb
```

Install the 32-bit libraries:
```
   sudo getlibs -p libsnmp-dev
   sudo getlibs -p libsnmp15
   sudo getlibs -w https://launchpad.net/~antoine-7degrees/+archive/ppa/+files/json-c-dev_0.9-2_i386.deb
   sudo getlibs -w https://launchpad.net/~antoine-7degrees/+archive/ppa/+files/uci-dev_2010-09-28-1_i386.deb
   sudo getlibs -w https://launchpad.net/~antoine-7degrees/+archive/ppa/+files/libcredis-dev_0.2.2-1_i386.deb
   sudo getlibs -w https://launchpad.net/~antoine-7degrees/+archive/ppa/+files/uci_2010-09-28-1_i386.deb
```

Pull afrimesh from svn (if you don't already have it):
```
   svn checkout http://afrimesh.googlecode.com/svn/branches/unstable/ afrimesh
   cd afrimesh
```


In the makefile of villagebus (villagebus/Makefile), change all occurrences of "$(DEPROOT)/lib/" to "$(DEPROOT)/lib32/", else the compiler will still use some of the 64-bit libraries:
```
   sed -i 's/$(DEPROOT)\/lib\//$(DEPROOT)\/lib32\//g' villagebus/Makefile
```