#!/bin/bash

echo
echo "Adding Afrimesh Release PPA"
echo

# TODO - why does this not work?!
#sudo apt-add-repository http://ppa.launchpad.net/afrimesh/ppa/ubuntu lucid main

gpg --keyserver keyserver.ubuntu.com --recv 50CFBA3E
gpg --export --armor 50CFBA3E | sudo apt-key add -
echo "deb http://ppa.launchpad.net/afrimesh/ppa/ubuntu lucid main" >> /etc/apt/sources.list
echo "deb-src http://ppa.launchpad.net/afrimesh/ppa/ubuntu lucid main" >> /etc/apt/sources.list

echo
echo "Updating repositories"
echo
sudo apt-get update

echo
echo "Finished adding Afrimesh PPA"
echo 
