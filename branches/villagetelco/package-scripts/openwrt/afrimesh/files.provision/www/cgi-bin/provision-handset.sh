#!/usr/bin/env sh

# - die if initial device provisioning has not taken place -----------------
[ -z `uci get afrimesh.settings.deviceid 2> /dev/null` ] && exit


# - factory setup ----------------------------------------------------------
factory_provisiond="/cgi-bin/provisiond-handset"


# - villagebus configuration -----------------------------------------------
self=$wifi0_address
root=`uci get afrimesh.settings.root`
device=`uci get afrimesh.settings.deviceid`
a2billing=""  # TODO - when the user punches in a2billing code, is it saved on phone
              #        or will it be saved by the remote and waiting for us?

# - 1. Build handset provisioning request ----------------------------------
json="{ 'self' : '$self', 'a2billing' : '$a2billing' }"
name="/provision/handset/$device"


# - 2. Send handset provisioning request to mesh root ----------------------
REQUEST="POST $factory_provisiond$name HTTP/1.0
Content-Type:   application/json
Content-Length: ${#json}

$json"
echo "- sending provisioning request -------------------------"
echo "$REQUEST"
echo
echo -n "$REQUEST" | nc $root 80 | sed '/HTTP.*OK/,/Content-Type: application\/x-tar/d; 1d' >& /tmp/provision-handset.tar.gz
echo "--------------------------------------------------------"
echo


# 3. - Execute provisiond reply --------------------------------------------
#
# TODO The provisioning daemon sends back an executable file which performs 
#      all router-side provisioning operations. Usually this file is simply
#      a self extracting tarfile.
#   
#tar xvzf /tmp/provision.tar.gz -C /


# 4. - Restart telephony ---------------------------------------------------
# /etc/init.d/asterisk restart


# 5. - Send an ack to the provisioning server ------------------------------
# 
#  TODO - this should maybe be a call to asterisk/a2 using the handset config
#         to confirm that it works
name="/cgi-bin/villagebus/telephony/callme/$self" # Blondie!!!
REQUEST="GET $name HTTP/1.0

"
echo "- sending callme request -------------------------"
echo "$REQUEST"
echo "--------------------------------------------------------"
echo
echo "- reply ------------------------------------------------"
echo -n "$REQUEST" | nc $root 80 
echo "--------------------------------------------------------"
echo


# 6. Die. Server will phone back w/ confirmation
#
#   TODO - should we keep trying periodically till we are phoned back?
#          

echo "fin"



