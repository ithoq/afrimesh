#!/usr/bin/env bash

# - Configuration ----------------------------------------------------------
VILLAGEBUS="./villagebus"
#UCI="/usr/local/bin/uci-static"
UCI="uci" # TODO search for uci in /usr/bin/uci /usr/local/bin/uci-static etc.
PROVISIOND_TMP=/tmp/provisiond.tmp
BUNDLE_DIR="./provisiond-bundles/mp01.interface"
LOG=1
LOGFILE="/tmp/provisiond.log"


# - Platform insanity ------------------------------------------------------
TAR=tar
[ `uname` == "Darwin" ] && TAR=/opt/local/bin/gnutar 
# there's this crazy busybox bug you see... https://dev.openwrt.org/ticket/6649
# TODO - so it effects FreeBSD too 

# - Logging ----------------------------------------------------------------
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
device_ip=`echo "$QUERY_STRING" | grep -oE "(^|[?&])address=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`
device_mac=${PATH_INFO#/provision/interface/}

TARBALL_DIR="$PROVISIOND_TMP/device-${device_mac//:/_}.$$"

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
provisioned=(`$VILLAGEBUS GET "$PATH_INFO?$QUERY_STRING"`)
provisioned_id=${provisioned[0]} 
provisioned_mac=${provisioned[1]} 
provisioned_ip=${provisioned[2]} 
#provisioned_root=${provisioned[3]} # TODO - pull from provisioning module once module no longer hardcodes it
provisioned_root=`$UCI get afrimesh.settings.root`
provisioned_lon=`$UCI get afrimesh.location.longitude`
provisioned_lat=`$UCI get afrimesh.location.latitude`
log "Provisioned device:    $provisioned_id"
log "Provisioned interface: $provisioned_mac"
log "Provisioned ip:        $provisioned_ip"
log "Provisioned root:      $provisioned_root"
log "Provisioned lon:       $provisioned_lon"
log "Provisioned lat:       $provisioned_lat"
# TODO - check return values


# - Construct configuration bundle -----------------------------------------

# configure UCI
UCI_CFG="$TARBALL_DIR/etc/config"
UCI_TMP="$TARBALL_DIR/.uci-provisiond"
#UCI="$UCI -c $UCI_CFG -P $UCI_TMP" # TODO - fscking UCI bug
UCI="$UCI -c $UCI_CFG"

# copy base bundle into a temporary directory
[ ! -d "$PROVISIOND_TMP" ] && mkdir -p "$PROVISIOND_TMP"
cp -r "$BUNDLE_DIR" "$TARBALL_DIR" 
find "$TARBALL_DIR" -name .svn -exec rm -rf '{}' ';'
# TODO - only provisioning /etc/config/afrimesh for the purposes of the demo
#mkdir -p "$TARBALL_DIR/etc/config" 
#cp -r "$BUNDLE_DIR/etc/config/afrimesh" "$TARBALL_DIR/etc/config" 
#cp -r "$BUNDLE_DIR/etc/config/system"   "$TARBALL_DIR/etc/config" 

# configure base bundle w/ provisioned values
wifi0_device=`$UCI get wireless.@wifi-iface[0].device` 
$UCI set network.$wifi0_device.ipaddr="$provisioned_ip"
$UCI set afrimesh.settings.deviceid="$provisioned_id"
$UCI set afrimesh.settings.root="$provisioned_root"
$UCI set afrimesh.location.longitude="$provisioned_lon"
$UCI set afrimesh.location.latitude="$provisioned_lat"

# commit configuration to provisioning bundle
log "- The following config changes were provisioned -- "
$UCI changes >> "$BUFFER" 2>&1
$UCI commit  >> "$BUFFER" 2>&1
# rm -rf $UCI_TMP

# tar it up and calculate size
$TAR -C "$TARBALL_DIR" -cf - . | gzip -f > "$TARBALL_DIR.tar.gz"
content_length=`wc -c $TARBALL_DIR.tar.gz | awk '{ print $1 }'`


# - Send provisioning bundle back to client --------------------------------
echo "Content-Type: application/x-tar"
echo "Content-Length: $content_length"
echo
cat "$TARBALL_DIR.tar.gz"


# TODO - make a self-extracting bundle -------------------------------------
#echo "Content-Type: application/x-tar"
#echo
#makeself blah blah


# - Clean up ---------------------------------------------------------------
[ -d "$TARBALL_DIR" ] && rm -rf "$TARBALL_DIR" "$TARBALL_DIR.tar.gz"
close_log 
