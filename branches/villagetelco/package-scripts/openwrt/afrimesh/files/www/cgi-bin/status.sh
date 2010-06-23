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
root=`uci get system.@system[0].log_ip`
mac=`ifconfig $interface | grep HWaddr | awk '{ print $5 }'`
path="$root/db/status/$self"

# misc information
timestamp=`date +%s`000
uname=`uname`
uptime=`cat /proc/uptime | awk '{ print $1 }'`
idle=`cat /proc/uptime | awk '{ print $2 }'`


# traffic stats - TODO wifi0 as well
buffer=`cat /proc/net/dev|grep $interface`
bytes_tx=`echo $buffer | awk '{ print $2 }'`
bytes_rx=`echo $buffer | awk '{ print $10 }'`
bytes="{ \
  'tx' : $bytes_tx, \
  'rx' : $bytes_rx }"

# query radio signal levels
levels=`iwconfig $interface |grep Quality`
link_quality=`echo $levels | cut -d = -f 2 | awk '{ print $1 }'`
signal_level=`echo $levels | cut -d = -f 3 | awk '{ print $1 " " $2 }'`
noise_level=`echo $levels | cut -d = -f 4 | awk '{ print $1 " " $2 }'`
radio="{ \
  'rssi'   : '$link_quality', \
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
        entry="{ 'gateway'  : '$gateway_address', \
                 'score'    : $score, \
                 'nexthop'  : '$nexthop_address', \
                 'failures' : $failures, \
                 'ping'     : { 'gateway' : $gateway_ping, 'nexthop' : $nexthop_ping }, \
                 'radio'    : { 'rate' : '$radio_rate', 'rssi' : '$radio_rssi/70', 'signal' : '$radio_signal dBm'  } }"
        if [ "$gateways" != "" ]; then
            gateways="$gateways, $entry"
        else 
            gateways="$entry"
        fi
    done
    echo $gateways
})

# TODO - neighbors

# assemble final message
json="{ \
  'timestamp' : $timestamp, \
  'mac'       : '$mac', \
  'self'      : '$self', \
  'root'      : '$root', \
  'version'   : '$version', \
  'uname'     : '$uname', \
  'uptime'    : $uptime, \
  'idle'      : $idle, \
  'radio'     : $radio, \
  'gateways'  : [ $gateways ],
  'bytes'     : $bytes \
}"

# construct HTTP request
HTTP="POST $villagebus/$path HTTP/1.0\n"
CONTENT_TYPE="Content-Type: text/plain\n"
CONTENT_LENGTH="Content-Length: ${#json}\n"
REQUEST="\
$HTTP\
$CONTENT_TYPE\
$CONTENT_LENGTH\
\n\
$json"

# TODO - eventually we want _this_:
#
#   */5 * * * * /www/cgi-bin/status 2> /dev/null | /www/cgi-bin/villagebus POST /root/db/status/self
echo -n -e $REQUEST | nc $root 80 >& /dev/null

# TODO - print header if being called from Web, else just echo json
#echo "Content-type: application/json"
#echo "Content-type: text/plain"
#echo
#echo $json

