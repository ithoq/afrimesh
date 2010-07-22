#!/usr/bin/env sh


# - Configuration ----------------------------------------------------------
#VILLAGEBUS="export REQUEST_METHOD=CONSOLE ; /Library/WebServer/CGI-Executables/villagebus"
VILLAGEBUS="./villagebus"
PROVISIOND_ROOT=./provisiond-router
PROVISIOND_TMP=/tmp/provisiond.tmp
LOG=1


# - Logging ----------------------------------------------------------------
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
    log "=================================================================="
fi


# - Read client request ----------------------------------------------------
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
client_address=`echo "$QUERY_STRING" | grep -oE "(^|[?&])address=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`
client_mac=${PATH_INFO#/provision/ip/}

PAYLOAD_DIR="$PROVISIOND_ROOT".payload
TARBALL_DIR="$PROVISIOND_TMP/router-${client_mac//:/_}.$$"

# Log client request
if [ "$LOG" = "1" ] ; then
    #env >> "$BUFFER"
    log "provisiond-router.cgi - METHOD:         $REQUEST_METHOD" >> "$BUFFER"
    log "provisiond-router.cgi - REMOTE_ADDR:    $REMOTE_ADDR"    >> "$BUFFER"
    log "provisiond-router.cgi - PATH_INFO:      $PATH_INFO"      >> "$BUFFER"
    log "provisiond-router.cgi - QUERY_STRING:   $QUERY_STRING"   >> "$BUFFER"
    log "provisiond-router.cgi - CONTENT_TYPE:   $CONTENT_TYPE"   >> "$BUFFER"
    log "provisiond-router.cgi - CONTENT_LENGTH: $CONTENT_LENGTH" >> "$BUFFER"
    log "provisiond-router.cgi - POST_DATA:      $POST_DATA"      >> "$BUFFER"
    log "provisiond-router.cgi - PAYLOAD_DIR:    $PAYLOAD_DIR"    >> "$BUFFER"
    log "provisiond-router.cgi - TARBALL_DIR:    $TARBALL_DIR"    >> "$BUFFER"
fi


# - forward request to villagebus & ask for network settings ---------------
log "- provisioned setting -------------------------------- " >> "$BUFFER"
REQUEST_METHOD=CONSOLE
#prov_address=`$VILLAGEBUS GET "/provision/ip/$client_mac?address=$client_address&network=$client_network"`` 
#prov_address=`$VILLAGEBUS GET "/provision/ip/$client_mac?address=$client_address"`
prov_address=`$VILLAGEBUS GET "$PATH_INFO?$QUERY_STRING"`
log "provisiond-router.cgi - address: |$prov_address|" >> "$BUFFER"


# - Construct configuration bundle -----------------------------------------

# copy base bundle into a temporary directory
[ ! -d "$PROVISIOND_TMP" ] && mkdir -p "$PROVISIOND_TMP"
cp -r "$PAYLOAD_DIR" "$TARBALL_DIR"

# - Send provisioning bundle back to client --------------------------------
echo "Content-Type: application/x-tar"
echo
tar -C "$TARBALL_DIR" -cf - . | gzip -f
#tar -C "$TARBALL_DIR" -cf - . 


# - Clean up ---------------------------------------------------------------
[ -d "$TARBALL_DIR" ] && rm -rf "$TARBALL_DIR"
close_log 
