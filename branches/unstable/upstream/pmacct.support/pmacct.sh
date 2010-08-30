#!/bin/bash

# config settings
PMACCT=/home/afrimesh/pmacct-0.11.6/src/pmacctd
CONFIGFILE=pmacctd.conf

# nuke all pmacctd
killall -9 pmacctd

# start pmacct with specified config file
$PMACCT -f $CONFIGFILE
