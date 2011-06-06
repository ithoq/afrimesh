#!/usr/bin/env bash

# config
self=192.168.20.105 # TODO uci get mesh address
gateway=192.168.20.1
villagebus="../villagebus"
version='r133'

# build message
timestamp=`date +%s`000
uname=`uname`
ping=`ping -c 1 $gateway | grep from | awk '{ print \$7 }' | cut -d = -f 2`
path="/root/db/status/$self"
# TODO - check return value
json="{ \
  'timestamp' : $timestamp, \
  'version'   : '$version',
  'uname'     : '$uname', \
  'gateway'   : { '$gateway' : $ping } }"

echo -n $json | $villagebus POST "$path"
# TODO - check return value
$villagebus GET "$path"

