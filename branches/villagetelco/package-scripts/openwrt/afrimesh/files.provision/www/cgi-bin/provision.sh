#!/usr/bin/env sh

# factory setup
factory_ssid=batman
factory_channel=5
factory_network=10.0.0.0
factory_netmask=255.0.0.0
factory_roots=192.168.1.9 10.0.0.1 10.0.0.254

# read network configuration


# 1. Do we have an active network on either the wifi or ethernet interfaces?
#  
#   By default, we're assuming that we'll be provisioning from a default
#   factory starting configuration and that accessible from one of the
#   router's network interfaces is a provisioning server that can talk 
#   to that configuration. 
# 


# 1a. TODO - Find likely SSID's in adhoc mode and try to climb onto
#            the network.
#


# 2. Look for a provisiond on the factory roots


# 2a. Ask batmand for the mesh gateway


# 3. Send provisioning request to provisiond
#  
#   MAC address
#   IP address
#   Router ssl public key


# 4. Receive provisiond reply
#
#   Network ssl public key
#   Configuration tarball
#   Checksum


# 5. Configure
#
#   Check checksum
#   Unpack configuration
#   Reboot
