#!/usr/bin/env bash

if [ "$REQUEST_METHOD" = "POST" ]; then
    read RAW_QUERY
fi
DIRECTION=`echo "$QUERY_STRING" | grep -oE "(^|[?&])direction=[^&]+" | sed "s/%20/ /g" | cut -f 2- -d "="`


echo "Content-type: application/json"
echo
pmacct -j -p /tmp/out.pipe

