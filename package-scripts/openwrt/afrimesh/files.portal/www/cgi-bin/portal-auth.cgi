#!/bin/sh

echo "Content-type: text/javascript"
# echo "Content-type: text/html"
echo


# get username, password, challenge from query

# call freeradius with details and get pap password back



# encode pap password

PAP_PASSWORD=55ac9f17a22b8abfc7201d9fa24239d0
#PAP_PASSWORD=123

echo "chilliJSON.reply({'response':'"$PAP_PASSWORD"'})"
echo


echo $QUERY_STRING >> /tmp/auth.log

# echo "<HTML><HEAD>"
# echo "<TITLE>test</TITLE>"
# echo "</HEAD><BODY>"
# echo "<pre>"
# echo QUERY_STRING : "$QUERY_STRING"
#
# URL=`echo $QUERY_STRING | sed s/url=//`
# echo URL: $URL
# wget -q -O - http://www.google.com
# echo "</pre>"
# echo "</BODY></HTML>"
