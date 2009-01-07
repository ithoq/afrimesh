#!/bin/sh


QUERY=$QUERY_STRING
if [ "$REQUEST_METHOD" = "POST" ]; then
    read QUERY
    echo "ajax-proxy.cgi - QUERY: $QUERY" >> /tmp/village-bus.log
    QUERY=`echo "$QUERY" | sed 's/\"/\\\"/g'`
fi

URL=`echo "$QUERY_STRING" | grep -oE "(^|[?&])url=[^&]+" | sed "s/%20/ /g" | cut -f 2 -d "="`
COMMAND="wget -q -O - $URL"
if [ "$REQUEST_METHOD" = "POST" ]; then
#    COMMAND="wget --post-data="$QUERY" -q -O - $URL"
    COMMAND="curl -d \"$QUERY\" $URL"
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
