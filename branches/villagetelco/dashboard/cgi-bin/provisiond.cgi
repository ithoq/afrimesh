#!/usr/bin/env sh


# - Configuration ----------------------------------------------------------
VILLAGEBUS="./villagebus"
UCI="/usr/local/bin/uci-static"
PROVISIOND_TMP=/tmp/provisiond.tmp
BUNDLE_DIR="./provisiond-bundles/mp01.ip"
LOG=1


# - Logging ----------------------------------------------------------------
LOGFILE="/tmp/provisiond.log"
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
    log "ERROR invalid request from: $REMOTE_ADDR"
    close_log
    exit
fi
client_address=`echo "$QUERY_STRING" | grep -oE "(^|[?&])address=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`
client_mac=${PATH_INFO#/provision/ip/}

TARBALL_DIR="$PROVISIOND_TMP/router-${client_mac//:/_}.$$"

# Log client request
if [ "$LOG" = "1" ] ; then
    #env >> "$BUFFER"
    log "METHOD:         $REQUEST_METHOD"
    log "REMOTE_ADDR:    $REMOTE_ADDR"
    log "PATH_INFO:      $PATH_INFO"
    log "QUERY_STRING:   $QUERY_STRING"
    log "CONTENT_TYPE:   $CONTENT_TYPE"
    log "CONTENT_LENGTH: $CONTENT_LENGTH"
    #log "POST_DATA:      $POST_DATA"
    log "BUNDLE_DIR:     $BUNDLE_DIR"
    log "TARBALL_DIR:    $TARBALL_DIR"
fi


# - forward request to villagebus & ask for network settings ---------------
log "- provisioning device --"
REQUEST_METHOD=CONSOLE
provisioned_root="192.168.20.105" # 10.0.0.1
provisioned_id=23                 # TODO
#provisioned_address=`$VILLAGEBUS GET "/provision/ip/$client_mac?address=$client_address&network=$client_network"`` 
#provisioned_address=`$VILLAGEBUS GET "/provision/ip/$client_mac?address=$client_address"`
provisioned_address=`$VILLAGEBUS GET "$PATH_INFO?$QUERY_STRING"`
log "Provisioned root:    $provisioned_root"
log "Provisioned id:      $provisioned_id"
log "Provisioned address: $provisioned_address"


# - Construct configuration bundle -----------------------------------------

# copy base bundle into a temporary directory
[ ! -d "$PROVISIOND_TMP" ] && mkdir -p "$PROVISIOND_TMP"
cp -r "$BUNDLE_DIR" "$TARBALL_DIR" 
find "$TARBALL_DIR" -name .svn -exec rm -rf '{}' ';'
# configure UCI
UCI_CFG="$TARBALL_DIR/etc/config"
UCI_TMP="$TARBALL_DIR/.uci-provisiond"
#UCI="$UCI -c $UCI_CFG -P $UCI_TMP" # TODO - fscking UCI bug
UCI="$UCI -c $UCI_CFG"

# configure base bundle w/ provisioned values
wifi0_device=`$UCI get wireless.@wifi-iface[0].device` 
$UCI set afrimesh.@settings[0].root="$provisioned_root"
$UCI set afrimesh.@settings[0].deviceid="$provisioned_id"
#$UCI set network.$wifi0_device.ipaddr="$provisioned_address"
$UCI set network.$wifi0_device.ipaddr="10.0.0.2"

# commit configuration to provisioning bundle
log "- The following config changes were provisioned -- "
$UCI changes >> "$BUFFER" 2>&1
$UCI commit  >> "$BUFFER" 2>&1
# rm -rf $UCI_TMP


# - Send provisioning bundle back to client --------------------------------
echo "Content-Type: application/x-tar"
echo
tar -C "$TARBALL_DIR" -cf - . | gzip -f

# self-extracting bundle
#echo "Content-Type: application/x-tar"
#echo
#makeself blah blah


# - Clean up ---------------------------------------------------------------
[ -d "$TARBALL_DIR" ] && rm -rf "$TARBALL_DIR"
close_log 
