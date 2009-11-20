#!/usr/bin/haserl --accept-all 
content-type: text/plain

<%

  if test -n "$FORM_image"; then

    # sysupgrade options
    OPTIONS="-d 10 -v"
    
    # check that the firmware image exists
    IMAGEFILE="/tmp/$FORM_image.img"
    [ -f "$IMAGEFILE" ] || {
      echo "{ \"error\" : \"Could not find image: $FORM_image\" }"
      exit 1
    }

    # perform the actual upgrade
    #UPGRADELOG=`ls -al $IMAGEFILE 2>&1`
    UPGRADELOG=`/sbin/sysupgrade $OPTIONS $IMAGEFILE 2>&1`
    [ $? -eq 0 ] || {
      rm -f "$IMAGEFILE"
      echo "{ \"error\" : \"$(echo $UPGRADELOG)\" }"
      exit 1
    }
    
    # reflash firmware
    echo "{ \"output\" : \"$(echo $UPGRADELOG)\" }"

  else
    echo "{ \"error\" : \"No image found\" }"
  fi
%>
