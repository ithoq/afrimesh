# Overview #

> To build Afrimesh nodes you will be maintaining two buckets of source: openwrt and afrimesh.  You will then wire up the afrimesh source into the openwrt toolchain.  Organizationally, you'll probably want to maintain a source directory with one folder for openwrt and one for afrimesh.  **All commands below are run from the perspective of the top level source directory unless otherwise noted**

# 1. Checkout OpenWRT #

  * Run:
```
  svn co svn://svn.openwrt.org/openwrt/branches/8.09 kamikaze
```
> This will create the kamikaze directory

# 2. Checkout Afrimesh #

  * Run:
```
  svn checkout http://afrimesh.googlecode.com/svn/trunk/ afrimesh
```
> this will create the afrimesh directory

# 3. Setup OpenWRT #

  * Add this line to the top of feeds.conf.default in the kamikaze directory:
```
  src-cpy afrimesh ../afrimesh/package-scripts/openwrt
```

  * Run from kamikaze directory:
```
  ./scripts/feeds update -a
```


# 4. Activate Packages #

  * Run from kamikaze directory:
```
  ./scripts/feeds install batmand 
  ./scripts/feeds install libjson 
  ./scripts/feeds install afrimesh-dashboard
```

  * Run:
```
  make menuconfig
  # Under Administration select with a *:
  # afrimesh-base for basic node (participates in the mesh and lets hosts connect)
  # TODO for gateway node (adds visualization server, Administration server?)
```


# Build Packages #

  * Run
```
  make world
```