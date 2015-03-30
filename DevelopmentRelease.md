From time to time this software will try to escape into the world.

By following these instructions you can make this a painless process for everyone involved every time.


# Subversion #
**Run:
```
  version=1.0pre
  cd ~/afrimesh
  revision=`svnversion | cut -d : -f 2`
  svn copy https://afrimesh.googlecode.com/svn/branches/unstable \
           https://afrimesh.googlecode.com/svn/tags/afrimesh-$version-r$revision \
      -m "Tagged release afrimesh-$version-r$revision"
```**


# OpenWRT #
**Bump revision number:
```
  vi package-scripts/openwrt/afrimesh/Makefile
  # edit PKG_REV to reflect the correct subversion revision number
```**

**Build binary packages and firmware images
```
  cd <kamikaze directory>
  make clean world
```**

**Upload to repository
```
  scp -r bin/* downloads.afrimesh.org:/usr/local/www/apache22/data/firmware
```**


**Update OpenWRT**

While we maintain a mirror of the openwrt package scripts in the afrimesh source tree the master version lives in the openwrt repository and will need to be updated to reflect new releases.

```
```


# Ubuntu #
**bump revision number and check build PPA:
```
  # edit VERSION to reflect the correct subversion revision number & check value of PPA variable
  vi Makefile
  # update changelog
  vi ./package-scripts/debian/afrimesh-dashboard/changelog
```**

**Update pbuilder environment
```
  sudo pbuilder update
```**

**Test package build locally
```
  make packages-linux
```**

**Update Launchpad PPA with the latest source package
```
  make launchpad-linux
```**


# FreeBSD #

TBD


# Announcements #