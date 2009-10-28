#!/usr/bin/env bash

PMACCT=pmacct
if [ -f /usr/bin/pmacct ] ; then
    PMACCT=/usr/bin/pmacct
fi
if [ -f /opt/local/bin/pmacct ] ; then
    PMACCT=/opt/local/bin/pmacct
fi
if [ -f /usr/local/bin/pmacct ] ; then
    PMACCT=/usr/local/bin/pmacct
fi

#if [ "$REQUEST_METHOD" = "POST" ]; then
#    read RAW_QUERY
#fi
#if [ -n $RAW_QUERY ]; then
#    QUERY_STRING=$RAW_QUERY
#fi

DIRECTION=`echo "$QUERY_STRING" | grep -oE "(^|[?&])direction=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`


echo "Content-type: application/json"
echo
if [ "$DIRECTION" == "out" ]
then
$PMACCT -j -p /tmp/out.pipe
fi

if [ "$DIRECTION" == "in" ]
then
$PMACCT -j -p /tmp/in.pipe
fi
