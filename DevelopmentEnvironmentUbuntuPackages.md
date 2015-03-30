## Ubuntu Package Build Environment ##

  * dput
```
  sudo apt-get install dput
```
  * pbuilder
```
  sudo apt-get install pbuilder devscripts  fakeroot
```
**dh\_make
```
  sudo apt-get install dh-make
```
  * configure:
```
  sudo pbuilder create --distribution <ubuntu_version (e.g. hardy)> \
        --othermirror "deb http://archive.ubuntu.com/ubuntu <ubuntu_version> main restricted universe multiverse"
```
  * build:
```
  make packages-linux
```
  * update pbuilder environment
```
  sudo pbuilder update
```
  * use pbuilder-dist (of the ubuntu-dev-tools) to have several different pbuilder setups for different Ubuntu releases.
  * read:
```
  https://wiki.ubuntu.com/PbuilderHowto
  https://wiki.ubuntu.com/PackagingGuide/Complete
```**
