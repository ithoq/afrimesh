#!/usr/bin/env sh


# Configuration
PROVISIOND_ROOT=./provisiond-router
PROVISIOND_TMP=/tmp/provisiond.tmp
LOG=1


# Logging
LOGFILE="/tmp/provisiond-router.log"
BUFFER="$LOGFILE.$$"
function log {
    if [ "$LOG" = "1" ] ; then
        echo "$1" >> "$BUFFER"
    fi
}
function close_log {
    if [ "$LOG" = "1" ] ; then
        echo           >> "$BUFFER"
        cat  "$BUFFER" >> "$LOGFILE"
        rm   "$BUFFER"
    fi
}
if [ "$LOG" = "1" ] ; then
    echo >> "$BUFFER"
    log "======================================================================"
fi


# Read client request
if [ "$REQUEST_METHOD" = "POST" ]; then
    POST_DATA=$(</dev/stdin)
else
    echo "Content-Type: application/json"
    echo
    echo "{ 'error' : 'invalid provisioning request' }"
    log "provisiond-router.cgi - ERROR invalid request from: $REMOTE_ADDR"
    close_log
    exit
fi

REMOTE_MAC=`echo "$QUERY_STRING" | grep -oE "(^|[?&])mac=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`
PAYLOAD_DIR="$PROVISIOND_ROOT".payload
TARBALL_DIR="$PROVISIOND_TMP/router-${REMOTE_MAC//:/_}.$$"


# Log client request
if [ "$LOG" = "1" ] ; then
    log "provisiond-router.cgi - METHOD:         $REQUEST_METHOD" >> "$BUFFER"
    log "provisiond-router.cgi - METHOD:         $REQUEST_METHOD" >> "$BUFFER"
    log "provisiond-router.cgi - REMOTE_ADDR:    $REMOTE_ADDR"    >> "$BUFFER"
    log "provisiond-router.cgi - REMOTE_PORT:    $REMOTE_PORT"    >> "$BUFFER"
    log "provisiond-router.cgi - REMOTE_PATH:    $REMOTE_PATH"    >> "$BUFFER"
    log "provisiond-router.cgi - REMOTE_MAC:     $REMOTE_MAC"     >> "$BUFFER"
    log "provisiond-router.cgi - QUERY_STRING:   $QUERY_STRING"   >> "$BUFFER"
    log "provisiond-router.cgi - CONTENT_TYPE:   $CONTENT_TYPE"   >> "$BUFFER"
    log "provisiond-router.cgi - CONTENT_LENGTH: $CONTENT_LENGTH" >> "$BUFFER"
    log "provisiond-router.cgi - POST_DATA:      $POST_DATA"      >> "$BUFFER"
    #log "provisiond-router.cgi - QUERY:          $QUERY"          >> "$BUFFER"
    log "provisiond-router.cgi - PAYLOAD_DIR:    $PAYLOAD_DIR"    >> "$BUFFER"
    log "provisiond-router.cgi - TARBALL_DIR:    $TARBALL_DIR"    >> "$BUFFER"
fi


# Construct configuration bundle
[ ! -d "$PROVISIOND_TMP" ] && mkdir -p "$PROVISIOND_TMP"
cp -r "$PAYLOAD_DIR" "$TARBALL_DIR"


# Send Reply Header
echo "Content-Type: application/x-tar"
echo


# Send configuration bundle
CURDIR="$PWD"
#tar -C "$TARBALL_DIR" -cf - . | gzip -f
tar -C "$TARBALL_DIR" -cf - . 
cd "$CURDIR"

# Log reply
if [ "$LOG" = "1" ] ; then
    log "- provisiond reply -------------------------------- " >> "$BUFFER"
    #log "$RESPONSE_HEADER" >> "$BUFFER"
    #log "--"               >> "$BUFFER"
    #log "$RESPONSE_BODY"   >> "$BUFFER"
    #log                    >> "$BUFFER"
fi


# Clean up
[ -d "$TARBALL_DIR" ] && rm -rf "$TARBALL_DIR"
close_log 
