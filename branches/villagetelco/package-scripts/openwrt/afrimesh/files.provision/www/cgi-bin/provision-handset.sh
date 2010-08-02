#!/usr/bin/env sh

# - factory setup ----------------------------------------------------------
factory_provisiond="/cgi-bin/provisiond-handset"


# - read network device configurations -------------------------------------
wifi0_device=`uci get wireless.@wifi-iface[0].device`
wifi0_interface=`uci get network.$wifi0_device.ifname`
wifi0_address=`uci get network.$wifi0_device.ipaddr`
wifi0_mac=`ifconfig $wifi0_interface | grep HWaddr | awk '{ print $5 }'`


# - villagebus configuration -----------------------------------------------
echo "- initialize device provisioning ------------------------------------"
self=$wifi0_address
root=`uci get afrimesh.settings.root`
device=`uci get afrimesh.settings.deviceid`
a2billing=""  # TODO - when the user punches in a2billing code, is it saved on phone
              #        or will it be saved by the remote and waiting for us?
echo "root:      $root"
echo "self:      $self"
echo "device:    $device"
echo "a2billing: $a2billing"


# - die if initial device provisioning has not taken place -----------------
[ -z `uci get afrimesh.settings.deviceid 2> /dev/null` ] && { 
    logger "$wifi0_mac ($self) has not yet been provisioned, exiting."
    echo "$wifi0_mac ($self) has not yet been provisioned, exiting." 
    echo
    exit  
}


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
echo -n "$REQUEST" | nc $root 80 >& $BUNDLE
logger "device# $device ($self) sent handset provisioning request"
echo "- sent provisioning request -----------------------------------------"
echo "name: $name"
echo "request: $REQUEST"
echo


# - strip content out of HTTP reply ----------------------------------------
file_size=`wc -c $BUNDLE | awk '{ print $1 }'` 
content_length=`head -n 10 $BUNDLE | grep "Content-Length: " | awk '{ print $2 }'`
header_size=`expr $file_size - $content_length`
dd if=$BUNDLE of=$BUNDLETMP bs=1 skip=$header_size 2> /dev/null
mv $BUNDLETMP $BUNDLE
echo "- processing handset provisioning bundle ----------------------------"
echo "content_length: $content_length"
echo


# 3. - Execute provisiond reply --------------------------------------------
#
# TODO The provisioning daemon sends back an executable file which performs 
#      all router-side provisioning operations. Usually this file is simply
#      a self extracting tarfile.
#   
echo "- unpacked provisioning bundle -------------------------------------"
tar xvzf $BUNDLE -C /
rm $BUNDLE
echo


# 4. - Restart telephony ---------------------------------------------------
logger "device# $device ($self) handset provisioning complete. Restarting." 
echo "device# $device ($self) handset provisioning complete. Restarting." 
echo
#/etc/init.d/asterisk restart
killall -9 asterisk ; sleep 2; /etc/init.d/asterisk start


# 5. - Send an ack to the provisioning server ------------------------------
# 
# TODO use handset id
name="/cgi-bin/villagebus/telephony/callme/$self" 
REQUEST="GET $name HTTP/1.0

"
echo -n "$REQUEST" | nc $root 80 
logger "device# $device sent callback request"
echo "- handset provisioned, sent callback request ------------------------"
echo "name:     " $name
echo "trunk:    " `uci get asterisk.sippotato.host`
echo "username: " `uci get asterisk.sippotato.username`
echo "secret:   " `uci get asterisk.sippotato.secret`


# 6. Die. Server will phone back w/ confirmation
#
#   TODO - should we keep trying periodically till we are phoned back?
#   

# Give asterisk plenty of time to wake up
sleep 30

# TODO - for demo I'll just originate the call locally for now
/usr/sbin/asterisk -rx "originate MP/1 application Playback callme"

echo "fin"
echo
echo


