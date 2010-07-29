#!/usr/bin/env sh

# - Configuration ----------------------------------------------------------
VILLAGEBUS="./villagebus"
UCI="/usr/local/bin/uci-static"
PROVISIOND_TMP=/tmp/provisiond.tmp
BUNDLE_DIR="./provisiond-bundles/mp01.handset"
LOG=1
LOGFILE="/tmp/provisiond.log"

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
handset_id=${PATH_INFO#/provision/handset/}
# a2billing_id="" TODO 
TARBALL_DIR="$PROVISIOND_TMP/handset-${handset_id//:/_}.$$"


# Log client request
if [ "$LOG" = "1" ] ; then
    #env >> "$BUFFER"
    log "METHOD:         $REQUEST_METHOD"
    log "REMOTE_ADDR:    $REMOTE_ADDR"
    log "PATH_INFO:      $PATH_INFO"
    log "QUERY_STRING:   $QUERY_STRING"
    log "CONTENT_TYPE:   $CONTENT_TYPE"
    log "CONTENT_LENGTH: $CONTENT_LENGTH"
    log "POST_DATA:      $POST_DATA"
    log "BUNDLE_DIR:     $BUNDLE_DIR"
    log "TARBALL_DIR:    $TARBALL_DIR"
fi


# - forward request to villagebus & ask for handset settings ---------------
log "- provisioning handset --"
REQUEST_METHOD=CONSOLE
provisioned=(`$VILLAGEBUS GET "$PATH_INFO?$QUERY_STRING"`)
provisioned_id=${provisioned[0]} 
provisioned_trunk=${provisioned[1]} 
provisioned_username=${provisioned[2]}
provisioned_secret=${provisioned[3]} 
log "Provisioned handset:  $provisioned_id"
log "Provisioned trunk:    $provisioned_trunk"
log "Provisioned username: $provisioned_username"
log "Provisioned secret:   $provisioned_secret"


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

# configure base bundle w/ provisioned values
$UCI set asterisk.sippotato.host="$provisioned_trunk"
$UCI set asterisk.sippotato.username="$provisioned_username"
$UCI set asterisk.sippotato.secret="$provisioned_secret"

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
