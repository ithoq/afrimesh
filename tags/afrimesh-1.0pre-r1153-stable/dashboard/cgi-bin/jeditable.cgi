#!/bin/sh


QUERY=$QUERY_STRING
if [ "$REQUEST_METHOD" = "POST" ]; then
read QUERY
fi

ID=`echo "$QUERY" | grep -oE "(^|[?&])id=[0-9]+" | cut -f 2 -d "=" | head -n1`
VALUE=`echo "$QUERY" | grep -oE "(^|[?&])value=[^&]+" | sed "s/%20/ /g" | cut -f 2 -d "="`

# unescape VALUE
VALUE=$(echo $VALUE | sed 's/+/ /g')

# log it
echo "jeditable.cgi -  $QUERY" >> /tmp/village-bus.log
echo "jeditable.cgi -  $ID : $VALUE" >> /tmp/village-bus.log

# return it
echo "Content-type: text/html"
echo
echo $VALUE

