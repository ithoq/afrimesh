#!/bin/bash

echo
echo "Adding Afrimesh PPA"
echo

sudo apt-add-repository http://ppa.launchpad.net/afrimesh/ppa/ubuntu lucid main

## update pbuilder with public keys to be able to use launchpad PPA's to resolve depends
#gpg --keyserver keyserver.ubuntu.com --recv 50CFBA3E
#gpg --export --armor 50CFBA3E | sudo apt-key add -

#echo "deb http://ppa.launchpad.net/afrimesh/ppa/ubuntu lucid main" >> /etc/apt/sources.list
#echo "deb-src http://ppa.launchpad.net/afrimesh/ppa/ubuntu lucid main" >> /etc/apt/sources.list

echo
echo "Updating repositories"
echo

sudo apt-get update

echo
echo "Finished adding Afrimesh PPA"
echo 
