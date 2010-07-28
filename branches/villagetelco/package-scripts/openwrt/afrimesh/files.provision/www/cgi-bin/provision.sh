#!/usr/bin/env sh

# - factory setup ----------------------------------------------------------
factory_ssid=batman
factory_channel=5
factory_network=10.0.0.0
factory_netmask=255.0.0.0
factory_roots="10.0.0.1 10.0.0.254"
factory_provisiond="/cgi-bin/provisiond"


# - read network device configurations -------------------------------------
wifi0_device=`uci get wireless.@wifi-iface[0].device`
wifi0_interface=`uci get network.$wifi0_device.ifname`
wifi0_address=`uci get network.$wifi0_device.ipaddr`
wifi0_mac=`ifconfig $wifi0_interface | grep HWaddr | awk '{ print $5 }'`
#wifi0_type=`uci get wireless.@wifi0_device.type`
#wifi0_channel=`uci get wireless.@wifi0_device.channel`
#wifi0_ssid=`uci get wireless.@wifi-iface[0].ssid`
#wifi0_bssid=`uci get wireless.@wifi-iface[0].bssid`
#wifi0_mode=`uci get wireless.@wifi-iface[0].mode`
eth0_interface=`uci get network.lan.ifname`
eth0_address=`uci get network.lan.ipaddr`
eth0_mac=`ifconfig $eth0_interface | grep HWaddr | awk '{ print $5 }'`


# - villagebus configuration -----------------------------------------------
self=$wifi0_address
# TODO - get from factory root
#root=`uci get afrimesh.@settings[0].root`
root="192.168.20.105"


# - router pubkey ----------------------------------------------------------
pubkey=`dropbearkey -y -f /etc/dropbear/dropbear_rsa_host_key | head -2 | tail -1`
# ssh -i /etc/dropbear/dropbear_rsa_host_key antoine@pi.artifactual.org.za


# - 0. Build provisioning request ------------------------------------------
#
#   Router address
#   Router rsa public key
#   List of interfaces to provision
#
# TODO - actually send and use the json request :)
#
json="{ \
  'self'       : '$self', \
  'pubkey'     : '$pubkey', \
  'interfaces' : [ { 'function' : 'mesh', \
                     'media'    : 'wifi', \
                     'name'     : '$wifi0_interface', \
                     'address'  : '$wifi0_address', \
                     'mac'      : '$wifi0_mac'  }, \
                   { 'function' : 'lan', \
                     'media'    : 'ethernet', \
                     'name'     : '$eth0_interface', \
                     'address'  : '$eth0_address', \
                     'mac'      : '$eth0_mac' } ] \
}"
#search="/provision/ip/$wifi0_mac?address=$self&network=$self_network&pubkey=$pubkey"
search="/provision/ip/$wifi0_mac?address=$self"


# 1. Do we have an active network on either the wifi or ethernet interfaces?
#  
#   By default, we're assuming that we'll be provisioning from a default
#   factory starting configuration and that accessible from one of the
#   router's network interfaces is a provisioning server that can talk 
#   to that configuration. 
# 
# 1a. TODO - If configuration differs form factory, try again w/ factory
#            config?
#
# 1b. TODO - Find likely SSID's in adhoc mode and try to climb onto
#            the network?
#
# 2. Look for a provisiond on the factory roots 
# 2a. Ask batmand for the mesh gateway



# - 3. Send provisioning request to mesh root ------------------------------

REQUEST="POST $factory_provisiond$search HTTP/1.0
Content-Type:   application/json
Content-Length: ${#json}

$json"

echo "- sending provisioning request -------------------------"
echo "$REQUEST"
echo "--------------------------------------------------------"
echo
echo "- provisiond reply -------------------------------------"
echo -n "$REQUEST" | nc $root 80 | sed '/HTTP.*OK/,/Content-Type: application\/x-tar/d; 1d' >& /tmp/provision.tar.gz
echo "--------------------------------------------------------"
echo

# 4. - Execute provisiond reply --------------------------------------------
#
# TODO The provisioning daemon sends back an executable file which performs 
#      all router-side provisioning operations. Usually this file is simply
#      a self extracting tarfile.
#
#   Network rsa public key
#   Configuration tarball
#   Checksum?

# 5. Configure
#
#   Check checksum
#   Unpack configuration
#   Reboot

#tar xvzf /tmp/provision.tar.gz -C /

echo "fin"



