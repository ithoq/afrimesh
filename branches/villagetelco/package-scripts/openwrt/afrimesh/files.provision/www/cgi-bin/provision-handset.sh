#!/usr/bin/env sh

# - die if initial device provisioning has not taken place -----------------
[ -z `uci get afrimesh.settings.deviceid 2> /dev/null` ] && exit


# - factory setup ----------------------------------------------------------
factory_provisiond="/cgi-bin/provisiond-handset"


# - read network device configurations -------------------------------------
wifi0_device=`uci get wireless.@wifi-iface[0].device`
wifi0_interface=`uci get network.$wifi0_device.ifname`
wifi0_address=`uci get network.$wifi0_device.ipaddr`


# - villagebus configuration -----------------------------------------------
self=$wifi0_address
root=`uci get afrimesh.settings.root`
device=`uci get afrimesh.settings.deviceid`
a2billing=""  # TODO - when the user punches in a2billing code, is it saved on phone
              #        or will it be saved by the remote and waiting for us?


# - 1. Build handset provisioning request ----------------------------------
json="{ 'self' : '$self', 'a2billing' : '$a2billing' }"
name="/provision/handset/$device"


# - 2. Send handset provisioning request to mesh root ----------------------
BUNDLE="/tmp/provision-handset.tar.gz"
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
echo -n "$REQUEST" | nc $root 80 >& $BUNDLE
echo

# strip content out of HTTP reply
file_size=`wc -c $BUNDLE | awk '{ print $1 }'` 
content_length=`head -n 10 $BUNDLE | grep "Content-Length: " | awk '{ print $2 }'`
header_size=`expr $file_size - $content_length`
dd if=$BUNDLE of=$BUNDLETMP bs=1 skip=$header_size 2> /dev/null
mv $BUNDLETMP $BUNDLE


# 3. - Execute provisiond reply --------------------------------------------
#
# TODO The provisioning daemon sends back an executable file which performs 
#      all router-side provisioning operations. Usually this file is simply
#      a self extracting tarfile.
#   
#tar xvzf /tmp/provision.tar.gz -C /


# 4. - Restart telephony ---------------------------------------------------
# /etc/init.d/asterisk restart


# 5. - Send an ack to the provisioning server ------------------------------
# 
# TODO use handset id
name="/cgi-bin/villagebus/telephony/callme/$self" # Blondie!!!
REQUEST="GET $name HTTP/1.0

"
echo "- sending callme request -------------------------"
echo "$REQUEST"
echo "--------------------------------------------------------"
echo
echo "- reply ------------------------------------------------"
echo -n "$REQUEST" | nc $root 80 
echo "--------------------------------------------------------"
echo


# 6. Die. Server will phone back w/ confirmation
#
#   TODO - should we keep trying periodically till we are phoned back?
#          

echo "fin"



