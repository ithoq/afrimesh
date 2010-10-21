#!/usr/bin/env bash

# - Configuration ----------------------------------------------------------
VILLAGEBUS="./villagebus"
UCI="uci"  # TODO search for uci in /usr/bin/uci /usr/local/bin/uci-static etc.
PROVISIOND_TMP=/tmp/provisiond.tmp
BUNDLE_DIR="./provisiond-bundles/mp01.handset"
LOG=1
LOGFILE="/tmp/provisiond.log"


# - Platform insanity ------------------------------------------------------
TAR=tar
[ `uname` == "Darwin" ] && TAR=/opt/local/bin/gnutar 
# there's this crazy busybox bug you see... https://dev.openwrt.org/ticket/6649


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
# TODO should be POSTing the request body too plz so we can do id/mac validation
log "- provisioning handset --"
REQUEST_METHOD=CONSOLE
REPLY=`$VILLAGEBUS GET "$PATH_INFO?$QUERY_STRING"`
log "Raw villagebus reply: $REPLY"
provisioned=($REPLY)
provisioned_id=${provisioned[0]} 
provisioned_trunk=${provisioned[1]} 
provisioned_did=${provisioned[2]}
provisioned_username=${provisioned[3]}
provisioned_secret=${provisioned[4]} 
provisioned_codec=${provisioned[5]} 
log "Provisioned handset:  $provisioned_id"
log "Provisioned trunk:    $provisioned_trunk"
log "Provisioned did:      $provisioned_did"
log "Provisioned username: $provisioned_username"
log "Provisioned secret:   $provisioned_secret"
log "Provisioned codec:    $provisioned_codec"
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

# configure base bundle w/ provisioned values 
# TODO set handset.id - anyone notice a flaw w/ tarballs & multiple provisioning phases? :)
$UCI set asterisk.villagetelco.host="$provisioned_trunk"
$UCI set asterisk.villagetelco.permit="$provisioned_trunk"
$UCI set asterisk.villagetelco.username="$provisioned_username"
$UCI set asterisk.villagetelco.secret="$provisioned_secret"
$UCI set asterisk.villagetelco.allow="$provisioned_codec"

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


# self-extracting bundle
#echo "Content-Type: application/x-tar"
#echo
#makeself blah blah


# - Clean up ---------------------------------------------------------------
[ -d "$TARBALL_DIR" ] && rm -rf "$TARBALL_DIR" "$TARBALL_DIR.tar.gz"
close_log 
