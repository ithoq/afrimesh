Configuring an afrimesh development environment.

# Required Dependencies #

  * gcc
  * subversion
  * Any httpd
    * I'm using apache for a Linux or FreeBSD install and busybox when running under OpenWRT
  * net-snmp
  * uci
  * json-c
  * bash   (to be deprecated)
  * curl     (to be deprecated)
  * wget   (to be deprecated)


# Optional Dependencies #

  * openwrt
    * you'll need either a copy of the source or the latest toolchain release to build OpenWRT packages
  * mysql
    * you'll need this if you plan to work with the radius support


# Platform Specific Instructions #

  * [DevelopmentEnvironmentOpenWRT](DevelopmentEnvironmentOpenWRT.md)
  * DevelopmentEnvironmentUbuntu
  * [DevelopmentEnvironmentFreeBSD](DevelopmentEnvironmentFreeBSD.md)


# Source #

  * Run:
```
  svn checkout http://afrimesh.googlecode.com/svn/branches/unstable/ afrimesh
```

  * Check the following variables in Makefile
```
  DEPROOT
  DASHBOARD_WWW
  DASHBOARD_CGI
```

# Compile #

In the top-level afrimesh source directory type:

```
  make 
```


# Install #

```
  make install
  chmod a+rw /tmp/*.log 
  chmod -R a+rw /etc/config
```


# Test Installation #
  * Check /etc/config/afrimesh