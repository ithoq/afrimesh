#!/bin/bash

# update pbuilder with public keys to be able to use launchpad PPA's to resolve depends

# pubkey for afrimesh PPA
echo
echo "Fetching public key for Afrimesh PPA"
echo
#whoami
#echo
#mkdir -p /home/$USER/.gnupg
#mkdir -p /home/antoine/.gnupg
gpg --keyserver keyserver.ubuntu.com --recv 382AF1D2
gpg --export --armor 382AF1D2 | sudo apt-key add -

echo "deb http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu hardy main" >> /etc/apt/sources.list
echo "deb-src http://ppa.launchpad.net/antoine-7degrees/ppa/ubuntu hardy main" >> /etc/apt/sources.list

echo
echo "Updating repositories"
echo

sudo apt-get update
# sudo apt-get --yes install uci-dev json-c-dev
echo
echo "Finished adding Afrimesh PPA"
echo 