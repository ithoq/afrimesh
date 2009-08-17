#!/bin/sh

#village-bus-olsr script

#echo "Content-type: text/html"

#echo "village-bus-meshtop.cgi: user IP is $REMOTE_ADDR" >> /tmp/village-bus.log
#echo "village-bus-meshtop.cgi: user is $HTTP_USER_AGENT" >> /tmp/village-bus.log

echo "Content-type: application/json"
echo ""
netcat 127.0.0.1 2005 > /tmp/mesh_viz_json.log
cat /tmp/mesh_viz_json.log | grep -v HTTP | grep -v Content






