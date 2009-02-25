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

    
# read & parse query
QUERY=$QUERY_STRING
if [ "$REQUEST_METHOD" = "POST" ]; then
    read RAW_QUERY
    #QUERY=`echo "$RAW_QUERY" | sed 's/\"/\\\"/g'`
    QUERY=`echo "$RAW_QUERY" | sed "s/\"/"\\\\\'"/g"`
fi
URL=`echo "$QUERY_STRING" | grep -oE "(^|[?&])url=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`


REQUEST="$WGET --user-agent=\"Afrimesh/1.0 XHR Proxy\" -q -O - $URL"
if [ "$REQUEST_METHOD" = "POST" ]; then
    #REQUEST="wget --post-data="$QUERY" -q -O - $URL"
    REQUEST="$CURL --user-agent \"Afrimesh/1.0 XHR Proxy\" -d \"$QUERY\" $URL"
fi

#echo "ajax-proxy.cgi - METHOD:    $REQUEST_METHOD" >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - RAW_QUERY: $RAW_QUERY" >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - QUERY:     $QUERY"     >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - REQUEST:   $REQUEST"   >> /tmp/village-bus.log
#echo "ajax-proxy.cgi - URL:       $URL"       >> /tmp/village-bus.log

echo "Content-type: text/html"
echo
RESPONSE=`eval $REQUEST`
echo $RESPONSE


#echo RESPONSE: $RESPONSE >> /tmp/village-bus.log


#echo "ajax-proxy.cgi - Finished Fetching: $URL" >> /tmp/village-bus.log