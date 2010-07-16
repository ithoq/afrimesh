#!/bin/sh

# TODO - hasn't anyone written a nice, portable, secure ajax-proxy ?

LOG=0

# read & parse query
QUERY=$QUERY_STRING
if [ "$REQUEST_METHOD" = "POST" ] || [ "$REQUEST_METHOD" = "PUT" ]; then
    read RAW_QUERY
    #QUERY=`echo "$RAW_QUERY" | sed 's/\"/\\\"/g'`
    QUERY=`echo "$RAW_QUERY" | sed "s/\"/"\\\\\'"/g"`
fi
URL=`echo "$QUERY_STRING" | grep -oE "(^|[?&])url=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`

# parse url - see: http://stackoverflow.com/questions/27745/getting-parts-of-a-url-regex
#                  http://www.linuxjournal.com/content/bash-regular-expressions
#url_regex="http://([^/]+)(/.*)"
#if [[ $URL =~ $url_regex ]] ; then
#  [[ $URL =~ $url_regex ]]
#  n=${#BASH_REMATCH[*]}
#  REMOTE_HOST=${BASH_REMATCH[1]}
#  REMOTE_PATH=${BASH_REMATCH[2]}
#else
#  echo "ajax-proxy.cgi - Invalid URL: $URL"       >> /tmp/ajax-proxy.log
#  echo "Content-Type: $CONTENT_TYPE"
#  echo
#  echo "jsonp({ 'error' : 'invalid url - $url ' })"
#  exit
#fi

REMOTE_HOST=`echo $URL | sed -e 's/[^\/\/[0-9]*\/]*//g' | sed -e 's/[^0-9,.].*//g'`
REMOTE_PORT=`echo $URL | sed -e 's/http:\/\/[^\:]*//g'  | sed -e 's/[^0-9]*//g'`
REMOTE_PATH=`echo $URL | sed -e 's/http:\/\/[^\/]*//g'`

#[ -z $REMOTE_HOST ] || REMOTE_HOST="NOREMOTEHOST" # TODO
[ -z $REMOTE_PORT ] && REMOTE_PORT="80"
[ -z $REMOTE_PATH ] && REMOTE_PATH="/"


# log request
if [ "$LOG" = "1" ] ; then
    echo "ajax-proxy.cgi - URL:            $URL"            >> /tmp/ajax-proxy.log
    echo "ajax-proxy.cgi - REMOTE_HOST:    $REMOTE_HOST"    >> /tmp/ajax-proxy.log
    echo "ajax-proxy.cgi - REMOTE_PORT:    $REMOTE_PORT"    >> /tmp/ajax-proxy.log
    echo "ajax-proxy.cgi - REMOTE_PATH:    $REMOTE_PATH"    >> /tmp/ajax-proxy.log
    echo "ajax-proxy.cgi - METHOD:         $REQUEST_METHOD" >> /tmp/ajax-proxy.log
    #echo "ajax-proxy.cgi - CONTENT_TYPE:   $CONTENT_TYPE"   >> /tmp/ajax-proxy.log
    #echo "ajax-proxy.cgi - CONTENT_LENGTH: $CONTENT_LENGTH" >> /tmp/ajax-proxy.log
    #echo "ajax-proxy.cgi - RAW_QUERY:      $RAW_QUERY" >> /tmp/ajax-proxy.log
    #echo "ajax-proxy.cgi - QUERY:          $QUERY"     >> /tmp/ajax-proxy.log
fi

# forward request to remote server and echo reply back to client
REQUEST="$REQUEST_METHOD $REMOTE_PATH HTTP/1.0\n\
Content-Type:   $CONTENT_TYPE\n\
Content-Length: $CONTENT_LENGTH\n\
\n\
$RAW_QUERY"

# log result
if [ "$LOG" = "1" ] ; then
    echo "--"           >> /tmp/ajax-proxy.log
    echo -e "$REQUEST"  >> /tmp/ajax-proxy.log
    echo "--"           >> /tmp/ajax-proxy.log
fi

# TODO - support alternative ports
RESPONSE=`echo -e $REQUEST | nc $REMOTE_HOST 80`
echo $RESPONSE

# log result
if [ "$LOG" = "1" ] ; then
    echo RESPONSE: $RESPONSE >> /tmp/ajax-proxy.log
    #echo "ajax-proxy.cgi - Finished Fetching: $URL" >> /tmp/ajax-proxy.log
    echo >> /tmp/ajax-proxy.log
fi
