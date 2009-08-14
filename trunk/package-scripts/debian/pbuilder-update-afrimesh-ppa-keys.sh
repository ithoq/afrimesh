#!/bin/bash

# update pbuilder with public keys to be able to use launchpad PPA's to resolve depends

echo
echo "Fetching public key for Afrimesh PPA"
echo
gpg --keyserver keyserver.ubuntu.com --recv 50CFBA3E
gpg --export --armor 50CFBA3E | sudo apt-key add -

echo "deb http://ppa.launchpad.net/afrimesh/ppa/ubuntu hardy main" >> /etc/apt/sources.list
echo "deb-src http://ppa.launchpad.net/afrimesh/ppa/ubuntu hardy main" >> /etc/apt/sources.list

echo
echo "Updating repositories"
echo

sudo apt-get update

echo
echo "Finished adding Afrimesh PPA"
echo 
