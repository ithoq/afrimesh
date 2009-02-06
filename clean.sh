#!/bin/sh

echo "Cleaning: "
find . -name "*~" 
find . -name .DS_Store 
find . -name "._*" 

find . -name "*~" | xargs rm -f
find . -name .DS_Store | xargs rm -f
find . -name "._*" | xargs rm -f