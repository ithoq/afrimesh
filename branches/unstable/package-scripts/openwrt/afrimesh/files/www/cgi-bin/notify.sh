#!/usr/bin/env sh


# - read notification ------------------------------------------------------
notify_extension="$1"
notify_channel="$2"
notify_trunk="$3"


# - read network device configurations -------------------------------------
wifi0_device=`uci get wireless.@wifi-iface[0].device`
wifi0_interface=`uci get network.$wifi0_device.ifname`
wifi0_address=`uci get network.$wifi0_device.ipaddr`


# - villagebus configuration -----------------------------------------------
echo "- initializing villagebus -------------------------------------------"
self=$wifi0_address
root=`uci get afrimesh.settings.root`
[ -z $root ] && root=$factory_root
echo "root: $root"
echo "self: $self"


# - post a VillageBus notification -----------------------------------------
name="/cgi-bin/villagebus/db/provision:$self:mac"
json="{ 'self'      : '$self', \
        'extension' : $notify_extension, \
        'channel'   : '$notify_channel', \
        'trunk'     : '$notify_trunk' }"
json=`echo -e $json` # echo -e on Linux seems to be nuking whitespace, which is throwing off the
                     # Content-Length calc :-/ Ffffffffffuuuuuuuuuuuuuuuuuuu!!!!!!!!!!
HTTP="PUT /cgi-bin/villagebus/db/message:handset:$self:call HTTP/1.0\n"
CONTENT_TYPE="Content-Type: text/plain\n"
CONTENT_LENGTH="Content-Length: ${#json}\n"
REQUEST="\
$HTTP\
$CONTENT_TYPE\
$CONTENT_LENGTH\
\n\
$json"
echo -n -e $REQUEST | nc $root 80 >& /dev/null
echo "- sent notification --------------------------------------------------"
echo -e $REQUEST
echo "- fin --"
echo

# exit success
exit 0

