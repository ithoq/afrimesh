#!/usr/bin/env sh

# - die if we are already provisioned --------------------------------------
[ ! -z `uci get afrimesh.settings.deviceid 2> /dev/null` ] && exit


# - factory setup ----------------------------------------------------------
factory_ssid=batman
factory_channel=5
factory_network=10.0.0.0
factory_netmask=255.0.0.0
factory_root="10.0.0.1"
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
root=`uci get afrimesh.settings.root`
[ -z $root ] && root=$factory_root


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
#name="/provision/interface/$wifi0_mac?address=$self&network=$self_network&pubkey=$pubkey"
name="/provision/interface/$wifi0_mac?address=$self"


# 1. Do we have an active network on either the wifi or ethernet interfaces?
#  
#   By default, we're assuming that we'll be provisioning from a default
#   factory starting configuration and that accessible from one of the
#   router's network interfaces is a provisioning server that can talk 
#   to that configuration. 
# 
# 1a. TODO - If configuration differs from factory, try again w/ factory
#            config?
#
# 1b. TODO - Find likely SSID's in adhoc mode and try to climb onto
#            the network?
#
# 2. Look for a provisiond on the factory roots 
# 2a. Ask batmand for the mesh gateway


# - 3. Send provisioning request to mesh root ------------------------------
BUNDLE="/tmp/provision.tar.gz"
BUNDLETMP="$BUNDLE.$$"
REQUEST="POST $factory_provisiond$name HTTP/1.0
Content-Type:   application/json
Content-Length: ${#json}

$json"
echo "- sending provisioning request -------------------------"
echo "root: $root"
echo ""
echo "$REQUEST"
echo
echo "--------------------------------------------------------"
#echo -n "$REQUEST" | nc $root 80 | sed '/HTTP.*OK/,/Content-Type: application\/x-tar/d; 1d' > $BUNDLE
#echo -n "$REQUEST" | nc $root 80 | sed '1,/^.$/d' > $BUNDLE
echo -n "$REQUEST" | nc $root 80 >& $BUNDLE
echo

# strip content out of HTTP reply
file_size=`wc -c $BUNDLE | awk '{ print $1 }'` 
content_length=`head -n 10 $BUNDLE | grep "Content-Length: " | awk '{ print $2 }'`
header_size=`expr $file_size - $content_length`
dd if=$BUNDLE of=$BUNDLETMP bs=1 skip=$header_size 2> /dev/null
mv $BUNDLETMP $BUNDLE


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

#tar xvzf $BUNDLE -C /


# 6. Kick of VOIP provisioning 
#
#    TODO - this would normally be supplied as part of 5 kicking in on reboot
#


echo "fin"
