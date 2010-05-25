#!/usr/bin/haserl --upload-limit=8192
content-type: text/plain

<% 
  if test -n "$FORM_file"; then
    # include openwrt helpers
    . /etc/functions.sh
    include /lib/upgrade

    # check the uploaded image file
    CHECKLOG=`platform_check_image "$FORM_file"`
    [ $? -eq 0 ] || {
      rm -f "$FORM_file"
      echo "{ \"error\" : \"$CHECKLOG\" }"
      exit 1
    }

    # return image details
    mv "$FORM_file" "$FORM_file.img"
    echo "{ \"filename\" : \"$FORM_file_name\", \"image\" : \"`basename $FORM_file`\" }"

  else
    echo "{ \"error\" : \"Image upload failed\" }"
  fi
%>
