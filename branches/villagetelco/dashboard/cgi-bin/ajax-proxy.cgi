#!/usr/bin/env sh

# TODO - hasn't anyone written a nice, portable, secure ajax-proxy ?

# config
LOG=1

# Log Buffer
LOGFILE="/tmp/ajax-proxy.log"
BUFFER="$LOGFILE.$$"
touch "$BUFFER"
if [ "$LOG" = "1" ] ; then
    echo >> "$BUFFER"
    echo "======================================================================"  >> "$BUFFER"
fi


# read client request
QUERY=$QUERY_STRING
if [ "$REQUEST_METHOD" = "POST" ] || [ "$REQUEST_METHOD" = "PUT" ]; then
    read RAW_QUERY
    #QUERY=`echo "$RAW_QUERY" | sed 's/\"/\\\"/g'`
    QUERY=`echo "$RAW_QUERY" | sed "s/\"/"\\\\\'"/g"`
fi
URL=`echo "$QUERY_STRING" | grep -oE "(^|[?&])url=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`


# parse remote url 
REMOTE_HOST=`echo $URL | sed -e 's/http:\/\///;s|\/.*||;s|\:.*||'`
if [ -z $REMOTE_HOST ] ; then
    echo "Content-Type: $CONTENT_TYPE"
    echo
    echo "jsonp({ 'error' : 'invalid url - $url ' })"
    echo "ajax-proxy.cgi - Invalid URL: $URL"       >> "$BUFFER"
    cat "$BUFFER" >> "$LOGFILE"
    rm "$BUFFER"
    exit
fi
REMOTE_PORT=`echo $URL | sed -e 's/http:\/\/.*://;s/[^0-9].*//'`
REMOTE_PATH=`echo $URL | sed -e 's/http:\/\/[^\/]*//g'`
[ -z $REMOTE_PORT ] && REMOTE_PORT="80"
[ -z $REMOTE_PATH ] && REMOTE_PATH="/"


# log client request
if [ "$LOG" = "1" ] ; then
    echo "ajax-proxy.cgi - URL:            $URL"            >> "$BUFFER"
    echo "ajax-proxy.cgi - REMOTE_HOST:    $REMOTE_HOST"    >> "$BUFFER"
    echo "ajax-proxy.cgi - REMOTE_PORT:    $REMOTE_PORT"    >> "$BUFFER"
    echo "ajax-proxy.cgi - REMOTE_PATH:    $REMOTE_PATH"    >> "$BUFFER"
    echo "ajax-proxy.cgi - METHOD:         $REQUEST_METHOD" >> "$BUFFER"
    #echo "ajax-proxy.cgi - CONTENT_TYPE:   $CONTENT_TYPE"   >> "$BUFFER"
    #echo "ajax-proxy.cgi - CONTENT_LENGTH: $CONTENT_LENGTH" >> "$BUFFER"
    #echo "ajax-proxy.cgi - RAW_QUERY:      $RAW_QUERY" >> "$BUFFER"
    #echo "ajax-proxy.cgi - QUERY:          $QUERY"     >> "$BUFFER"
fi


# construct remote request 
if [ "$REQUEST_METHOD" = "POST" ] || [ "$REQUEST_METHOD" = "PUT" ]; then
    REQUEST="$REQUEST_METHOD $REMOTE_PATH HTTP/1.0
Content-Type:   $CONTENT_TYPE
Content-Length: $CONTENT_LENGTH

$RAW_QUERY"
else
    REQUEST="$REQUEST_METHOD $REMOTE_PATH HTTP/1.0

"
fi
if [ "$LOG" = "1" ] ; then
    echo "- client request --------------------------------- " >> "$BUFFER"
    echo "$REQUEST"  >> "$BUFFER"
    echo             >> "$BUFFER"
fi


# invoke remote request 
RESPONSE=`echo -n "$REQUEST" | nc $REMOTE_HOST $REMOTE_PORT`


# check response headers 
RESPONSE_HEADER=`echo "$RESPONSE" | head -n 1`
if [ `expr "$RESPONSE_HEADER" : '.*404 Not Found'` != 0 ] ; then
    RESPONSE_BODY="Status: 404 Not Found

404. Am disappoint."
else
    RESPONSE_BODY=`echo "$RESPONSE" | sed '1d'` 
fi


# forward response to client
echo -n "$RESPONSE_BODY"
if [ "$LOG" = "1" ] ; then
    echo "- remote response -------------------------------- " >> "$BUFFER"
    echo "$RESPONSE_HEADER" >> "$BUFFER"
    echo "--"               >> "$BUFFER"
    echo "$RESPONSE_BODY"   >> "$BUFFER"
    echo                    >> "$BUFFER"

    # Append log buffer to actual log & nuke buffer
    cat "$BUFFER" >> "$LOGFILE"
    rm "$BUFFER"
fi

