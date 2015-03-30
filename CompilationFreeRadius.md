## Installing freeradius ##

The followings were tested on ubuntu 8.04.

## Install the necessary packages ##

# sudo apt-get install libssl-dev debhelper libgdbm-dev libiodbc2-dev libkrb5-dev libldap2-#dev libltdl3-dev libmysqlclient15-dev libpam0g-dev libpcap-dev libperl-dev libpq-dev #libsasl2-dev libsnmp-dev python-dev fakeroot snmp libltdl3 libpcap0.8  libperl5.8 #libdb4.5  libsqlite3-0 build-essential autotools-dev libtool dpatch python2.5 python2.5-minimal

## install debhelper/quilt ##

edit /etc/apt/sources.list
  1. uncomment deb http://us.archive.ubuntu.com/ubuntu/ hardy-backports main restricted universe multiverse
  1. Save changes and quit
  1. apt-get update
  1. apt-get install debhelper quilt

## download freeradius ##

download the following version:
  1. wget [ftp://ftp.freeradius.org/pub/freeradius/freeradius-server-2.1.8.tar.gz](ftp://ftp.freeradius.org/pub/freeradius/freeradius-server-2.1.8.tar.gz)

Unpack the source:
  1. tar zxvf freeradius-server-2.1.8.tar.gz

Edit debian/rules to make few changes
  1. --with-rlm\_eap\_tls \
  1. --with-rlm\_eap\_ttls \
  1. --with-rlm\_eap\_peap \
  1. --without-rlm\_eap\_tnc \
  1. --with-rlm\_otp \
  1. --with-openssl \
  1. --with-snmp \

Type the following command to build the packages
  1. dpkg-buildpackage -rfakeroot

## Installing freeradius ##

Hold the packages you install from over written:
  1. echo "freeradius hold" | dpkg --set-selections
  1. echo "freeradius-mysql hold" | dpkg --set-selections
  1. echo "libfreeradius2 hold" | dpkg --set-selections
  1. echo "freeradius-common hold" | dpkg --set-selections

Install the packages (only the one you need):
  1. dpkg -i freeradius-common\_2.1.8+git\_all.deb
  1. dpkg -i libfreeradius2\_2.1.8+git\_i386.deb
  1. dpkg -i freeradius\_2.1.4-0\_i386.deb
  1. dpkg -i freeradius-mysql\_2.1.4-0\_i386.deb

To see if the package actually on hold
  1. dpkg --get-selections freer