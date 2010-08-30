#!/bin/sh

TRUNK=`dirname $0`

echo "Cleaning: $TRUNK "
find $TRUNK -name "*~" 
find $TRUNK -name .DS_Store 
find $TRUNK -name "._*" 
find $TRUNK -name "*~" | xargs rm -f
find $TRUNK -name .DS_Store | xargs rm -f
find $TRUNK -name "._*" | xargs rm -f

cd $TRUNK ; make clean
