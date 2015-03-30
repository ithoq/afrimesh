Instructions for preparing an afrimesh development environment under FreeBSD

# Dependencies #

Run as required:

  * json-c:  (Alternatively you can cd /usr/ports/devel/json-c if you don't mind half of gnome being pulled in!)
```
  cd package-scripts/freebsd/devel/json-c
  make install clean
```
  * uci:
```
  cd package-scripts/freebsd/misc/uci
  make install clean
```


Alternatively, if you don't want to clutter your system with a long list of dependencies you can compile the sources manually and install them into the ext/ directory off the afrimesh buildroot

uci : `http://downloads.openwrt.org/sources/uci-0.7.3.tar.gz`

libjson : `http://oss.metaparadigm.com/json-c/`