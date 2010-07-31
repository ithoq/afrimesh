#!/bin/bash

echo
echo "Adding Afrimesh Testing PPA"
echo

# TODO - why does this not work?!
#sudo apt-add-repository http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu lucid main

gpg --keyserver keyserver.ubuntu.com --recv 382AF1D2
gpg --export --armor 382AF1D2 | sudo apt-key add -
echo "deb http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu lucid main" >> /etc/apt/sources.list
echo "deb-src http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu lucid main" >> /etc/apt/sources.list

echo
echo "Updating repositories"
echo
sudo apt-get update

echo
echo "Finished adding Afrimesh PPA"
echo 