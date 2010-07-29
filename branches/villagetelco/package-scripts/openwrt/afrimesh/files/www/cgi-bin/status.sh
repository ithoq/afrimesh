#!/usr/bin/env sh

# config
villagebus="/cgi-bin/villagebus"
version='r133'
timeout=2

# router information
wlanmode=`uci get wireless.@wifi-iface[0].mode`
device=`uci get wireless.@wifi-iface[0].device`
interface=`uci get network.$device.ifname`
self=`uci get network.$device.ipaddr`
#root=`uci get system.@system[0].log_ip`
root=`uci get afrimesh.settings.root`
device_id=`uci get afrimesh.settings.deviceid`
mac=`ifconfig $interface | grep HWaddr | awk '{ print $5 }'`
path_info="$root/db/device:$self:status"
path_stat="$root/db/interface:$self:history"

# don't report status info for unprovisioned devices
[ -z "$device_id" ] && exit

# misc information
timestamp=`date +%s`000
uname=`uname -a`
uptime=`cat /proc/uptime | awk '{ print $1 }'`
idle=`cat /proc/uptime | awk '{ print $2 }'`
longitude=`uci get afrimesh.location.longitude`
latitude=`uci get afrimesh.location.latitude`

# traffic stats - TODO wifi0 as well
buffer=`cat /proc/net/dev|grep $interface`
bytes_tx=`echo $buffer | awk '{ print $2 }'`
bytes_rx=`echo $buffer | awk '{ print $10 }'`
bytes="{ 'tx' : $bytes_tx, \
         'rx' : $bytes_rx }"

# query radio signal levels
levels=`iwconfig $interface |grep Quality`
link_quality=`echo $levels | cut -d = -f 2 | awk '{ print $1 }'`
signal_level=`echo $levels | cut -d = -f 3 | awk '{ print $1 " " $2 }'`
noise_level=`echo $levels | cut -d = -f 4 | awk '{ print $1 " " $2 }'`
radio="{ 'rssi'   : '$link_quality', \
         'signal' : '$signal_level', \
         'noise'  : '$noise_level' }"

# ping gateways - TODO mark default gw
gateways=$(batmand -cbd2 | grep gateway |{
    while read line; do
        gateway_address=`echo $line | awk '{print \$2 }`
        nexthop_address=`echo $line | awk '{print \$4 }`
        score=`echo $line | awk 'BEGIN{FS="(";RS=")"}/\(/{print $2}'`
        failures=`echo $line | awk '{print \$13 }`
        #gateway_reply=`arping -f -w 2 -I $interface $gateway_address | grep reply`
        gateway_ping=`ping -w $timeout -c 1 $gateway_address 2> /dev/null | grep from | awk '{ print \$7 }' | cut -d = -f 2`
        [ -z $gateway_ping ] && gateway_ping="-1.0"
        nexthop_ping=`ping -w $timeout -c 1 $nexthop_address 2> /dev/null | grep from | awk '{ print \$7 }' | cut -d = -f 2`
        [ -z $nexthop_ping ] && nexthop_ping="-1.0"
        nexthop_arp=`cat /proc/net/arp |grep $nexthop_address | awk '{ print $4 }'`
        radio=`wlanconfig $interface list|grep $nexthop_arp`
        radio_rate=`echo $radio | awk '{ print $4 }'`
        radio_rssi=`echo $radio | awk '{ print $5 }'`
        radio_signal=`echo $radio | awk '{ print $5 }'`
        entry="{ 'address'  : '$gateway_address', \
                 'nexthop'  : '$nexthop_address', \
                 'score'    : $score, \
                 'ping'     : { 'gateway' : $gateway_ping, 'nexthop' : $nexthop_ping }, \
                 'radio'    : { 'rate' : '$radio_rate', 'rssi' : '$radio_rssi/70', 'signal' : '$radio_signal dBm'  } }"
        fails="{ 'address'  : '$gateway_address', \
                 'failures' : $failures }"
        if [ "$gateways" != "" ]; then
            gateways="$gateways, $entry"
            failures="$failures, $fails"
        else 
            gateways="$entry"
            failures="$fails"
        fi
    done
    echo "$gateways|$failures"
})
failures=${gateways#*|}
gateways=${gateways%|*}

# TODO - neighbors

# construct & send HTTP request for instantaneous data
json="{ 'timestamp' : $timestamp, \
        'mac'       : '$mac', \
        'self'      : '$self', \
        'root'      : '$root', \
        'version'   : '$version', \
        'uname'     : '$uname', \
        'uptime'    : $uptime, \
        'idle'      : $idle, \
        'location'  : { 'longitude' : '$longitude', 'latitude' : '$latitude' }, \
        'bytes'     : $bytes, \
        'gateways'  : [ $failures ] }"
HTTP="PUT $villagebus/$path_info HTTP/1.0\n"
CONTENT_TYPE="Content-Type: text/plain\n"
CONTENT_LENGTH="Content-Length: ${#json}\n"
REQUEST="\
$HTTP\
$CONTENT_TYPE\
$CONTENT_LENGTH\
\n\
$json"
echo -n -e $REQUEST | nc $root 80 >& /dev/null


# construct & send HTTP request for temporal data
json="{ 'self'      : '$self', \
        'timestamp' : $timestamp, \
        'radio'     : $radio, \
        'gateways'  : [ $gateways ] }"
HTTP="POST $villagebus/$path_stat HTTP/1.0\n"
CONTENT_TYPE="Content-Type: text/plain\n"
CONTENT_LENGTH="Content-Length: ${#json}\n"
REQUEST="\
$HTTP\
$CONTENT_TYPE\
$CONTENT_LENGTH\
\n\
$json"
echo -n -e $REQUEST | nc $root 80 >& /dev/null


# TODO - eventually we want _this_:
#
#   */5 * * * * /www/cgi-bin/status 2> /dev/null | /www/cgi-bin/villagebus POST /root/db/status/self

# TODO - print header if being called from Web, else just echo json
#echo "Content-type: application/json"
#echo "Content-type: text/plain"
#echo
#echo $json

