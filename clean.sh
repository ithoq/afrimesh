#!/bin/sh

echo "Cleaning: "
find . -name "*~" 
find . -name .DS_Store 
find . -name "._*" 

find . -name "*~" | xargs rm
find . -name .DS_Store | xargs rm
find . -name "._*" | xargs rm 