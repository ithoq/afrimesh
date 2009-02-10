#!/bin/sh

# TODO - hasn't anyone written a nice, portable, secure ajax-proxy ?

# track down wget and curl
WGET=wget
CURL=curl
if [ -f /usr/bin/wget ]; then
    WGET=/usr/bin/wget
fi
if [ -f /usr/bin/curl ]; then
    CURL=/usr/bin/curl
fi
if [ -f /usr/local/bin/wget ]; then
    WGET=/usr/local/bin/wget
fi
if [ -f /usr/local/bin/curl ]; then
    CURL=/usr/local/bin/curl
fi

    

QUERY=$QUERY_STRING
if [ "$REQUEST_METHOD" = "POST" ]; then
    read QUERY
    QUERY=`echo "$QUERY" | sed 's/\"/\\\"/g'`
fi
#echo "ajax-proxy.cgi - QUERY: $QUERY" >> /tmp/village-bus.log

URL=`echo "$QUERY_STRING" | grep -oE "(^|[?&])url=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`

#echo "ajax-proxy.cgi - QUERY_STRING: $QUERY_STRING" >> /tmp/village-bus.log
echo "ajax-proxy.cgi - URL: |$URL|" >> /tmp/village-bus.log

COMMAND="$WGET -q -O - $URL"
if [ "$REQUEST_METHOD" = "POST" ]; then
#    COMMAND="wget --post-data="$QUERY" -q -O - $URL"
    COMMAND="$CURL -d \"$QUERY\" $URL"
fi

#echo "ajax-proxy.cgi - REQUEST_METHOD: $REQUEST_METHOD" >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - QUERY_STRING: $QUERY_STRING" >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - QUERY: $QUERY" >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - COMMAND: $COMMAND" >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - Started Fetching: $URL" >> /tmp/village-bus.log

echo "Content-type: text/html"
echo
#result=`$COMMAND`
#echo "RESULT: |$result|" >> /tmp/village-bus.log
#echo $result
eval $COMMAND

#echo "ajax-proxy.cgi - Finished Fetching: $URL" >> /tmp/village-bus.log
