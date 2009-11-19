#!/usr/local/bin/haserl --accept-all --upload-limit=4096 --upload-target=/tmp 
content-type: text/plain
<% if test -n "$FORM_file"; then %>
<%# TODO - check that it's a valid firmware image %>
{ "filename" : "<% echo -n $FORM_file_name %>", "tempname"  : "<% echo -n $FORM_file %>" }
<% rm -f "$FORM_file" %>
<% else %>
{ "error"   : "upload failed" }
<% fi %>
