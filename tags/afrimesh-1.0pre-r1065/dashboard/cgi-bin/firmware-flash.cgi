#!/usr/bin/haserl --accept-all
content-type: text/plain

<%
  if test -n "$FORM_image"; then
    # sysupgrade options
    OPTIONS="-v"
    
    # check that the firmware image exists
    IMAGEFILE="/tmp/$FORM_image.img"
    [ -f "$IMAGEFILE" ] || {
      echo "{ \"error\" : \"Could not find image: $FORM_image\" }"
      exit 1
    }

    # perform the actual upgrade 
    /sbin/sysupgrade $OPTIONS $IMAGEFILE 2>&1
    [ $? -eq 0 ] || {
      rm -f "$IMAGEFILE"
      echo "{ \"error\" : \"sysupgrade failed\" }"
      exit 1
    }
    rm -f "$IMAGEFILE"
    echo "GREAT SUCCESS!"
    echo "Upgrade completed"
    echo "Rebooting system..."

  else
    echo "{ \"error\" : \"No firmware image specified\" }"
  fi
%>




