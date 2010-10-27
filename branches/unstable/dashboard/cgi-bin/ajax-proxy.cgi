#!/usr/bin/env bash

# TODO - OpenWRT only has sh, but sh on other platforms doesn't support '-n' flag on echo :-/

# TODO - hasn't anyone written a nice, portable, secure ajax-proxy ?

# - config -----------------------------------------------------------------
LOG=1

# Log Buffer
LOGFILE="/tmp/ajax-proxy.log"
BUFFER="$LOGFILE.$$"
touch "$BUFFER"
if [ "$LOG" = "1" ] ; then
    echo >> "$BUFFER"
    echo "=================================================================="  >> "$BUFFER"
fi

# platform issues
UNAME=`uname`
NC=nc
if [ "$UNAME" = "Linux" ]; then
    # by default Linux nc is not waiting for a reply
    # before closing the connection
    # -q has it wait up till 15 seconds to hear back
    # TODO - this is going to wreak havoc on OpenWRT *sigh*
    NC="nc -q 15"
fi

# - read client request ----------------------------------------------------
QUERY=$QUERY_STRING
if [ "$REQUEST_METHOD" = "POST" ] || [ "$REQUEST_METHOD" = "PUT" ]; then
    read RAW_QUERY
    #QUERY=`echo "$RAW_QUERY" | sed 's/\"/\\\"/g'`
    QUERY=`echo "$RAW_QUERY" | sed "s/\"/"\\\\\'"/g"`
fi
#URL=`echo "$QUERY_STRING" | grep -oE "(^|[?&])url=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`
URL=`echo "$QUERY_STRING" | sed 's/^url=*//' | sed 's/[&|?]callback=.*//' | sed "s/%20/ /g"`
# TODO - is this obsolete now?
#CALLBACK=`echo "$QUERY_STRING" | grep -oE "(^|[?&])callback=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`
CALLBACK=`echo "$QUERY_STRING" | grep -o "callback=.*" | sed 's/.*callback=*//'`

# - parse remote url -------------------------------------------------------
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
ADR=`echo $URL | grep -o http://[^/]*`
REMOTE_PORT=`echo $ADR | sed -e 's/http:\/\/.*://;s/[^0-9].*//'`
REMOTE_PATH=`echo $URL | sed -e 's/http:\/\/[^\/]*//g'`
[ -z $REMOTE_PORT ] && REMOTE_PORT="80"
[ -z $REMOTE_PATH ] && REMOTE_PATH="/"
if [ ! -z $CALLBACK ] && [ $URL != "${URL/\?/}" ]; then
    REMOTE_PATH="$REMOTE_PATH&callback=$CALLBACK"
elif [ ! -z $CALLBACK ]; then
    REMOTE_PATH="$REMOTE_PATH?callback=$CALLBACK"
fi

# log client request
if [ "$LOG" = "1" ] ; then
    echo "ajax-proxy.cgi - QUERY_STRING:   $QUERY_STRING"   >> "$BUFFER"
    echo "ajax-proxy.cgi - URL:            $URL"            >> "$BUFFER"
    echo "ajax-proxy.cgi - CALLBACK:       $CALLBACK"       >> "$BUFFER"
    echo "ajax-proxy.cgi - REMOTE_HOST:    $REMOTE_HOST"    >> "$BUFFER"
    echo "ajax-proxy.cgi - REMOTE_PORT:    $REMOTE_PORT"    >> "$BUFFER"
    echo "ajax-proxy.cgi - REMOTE_PATH:    $REMOTE_PATH"    >> "$BUFFER"
    echo "ajax-proxy.cgi - METHOD:         $REQUEST_METHOD" >> "$BUFFER"
    #echo "ajax-proxy.cgi - CONTENT_TYPE:   $CONTENT_TYPE"   >> "$BUFFER"
    #echo "ajax-proxy.cgi - CONTENT_LENGTH: $CONTENT_LENGTH" >> "$BUFFER"
    #echo "ajax-proxy.cgi - RAW_QUERY:      $RAW_QUERY" >> "$BUFFER"
    #echo "ajax-proxy.cgi - QUERY:          $QUERY"     >> "$BUFFER"
fi


# - perform remote request -------------------------------------------------
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
    echo "- making remote request --------------------------------- " >> "$BUFFER"
    echo "$REQUEST"  >> "$BUFFER"
    echo             >> "$BUFFER"
fi

# invoke remote request 
RESPONSE=`echo -n "$REQUEST" | $NC $REMOTE_HOST $REMOTE_PORT`
#echo "|$RESPONSE|" >> "$BUFFER"

# check response headers 
RESPONSE_HEADER=`echo "$RESPONSE" | head -n 1`
if [ `expr "$RESPONSE_HEADER" : '.*404 Not Found'` != 0 ] ; then
    RESPONSE_BODY="Status: 404 Not Found

404. Am disappoint."
else
    RESPONSE_BODY=`echo "$RESPONSE" | sed '1d'` 
fi


# - forward response to client ---------------------------------------------
echo -n "$RESPONSE_BODY"
if [ "$LOG" = "1" ] ; then
    echo "- remote responsed -------------------------------- " >> "$BUFFER"
    echo "$RESPONSE_HEADER" >> "$BUFFER"
    echo "--"               >> "$BUFFER"
    echo "$RESPONSE_BODY"   >> "$BUFFER"
    echo                    >> "$BUFFER"
    # Append log buffer to actual log & nuke buffer
    cat "$BUFFER" >> "$LOGFILE"
    rm "$BUFFER"
fi

