#!/usr/bin/lua

--[[ includes ]]------------------------------------------------------------
require "luarocks.require"
require "json"
-- http://wiki.openwrt.org/doc/devel/uci-lua
package.cpath = package.cpath .. ";/Users/antoine/afrimesh/ext/darwin/lib/lua5.1/?.so"
package.cpath = package.cpath .. ";/usr/share/lua/5.1/?.so"
uci = require("uci")
uci = uci.cursor()


--[[ utils ]]------------------------------------------------------------
require "common"


--[[ logging ]]------------------------------------------------------------
require "logging"
require "logging.file"
logname = "/tmp/provision.log"
os.pexecute("touch " .. logname)
os.pexecute("chmod 0666 " .. logname)
log = logging.file(logname)
if not log then
  log = logging.file("/dev/null")
end
-- log:setLevel(logging.INFO)
log:setLevel(logging.DEBUG)
log:info("provision.lua starting")





--[[ definitions ]]---------------------------------------------------------
function readconf()
  local factory = { 
    mode       = "ahdemo",
    ssid       = "potato",
    bssid      = "01:CA:FF:EE:BA:BE",
    channel    = "1",
    network    = "10.130.1.0",
    netmask    = "255.255.255.0",
    root       = "10.130.1.1",
  }
  local wireless = { }
  uci:foreach("wireless", "wifi-iface", 
              function(entry)
                local wifi = { 
                  device = entry.device,
                  ssid   = entry.ssid,
                  bssid  = entry.bssid,
                  mode   = entry.mode
                }
                wifi.interface = uci:get("network", wifi.device, "ifname")
                wifi.address   = uci:get("network", wifi.device, "ipaddr")
                wifi.mac       = os.pexecute("ifconfig " .. wifi.interface .. " | grep HWaddr | awk '{ print $5 }'")
                wifi.mac       = string.upper(wifi.mac)
                wifi.type      = uci:get("wireless", wifi.device, "type")
                wifi.channel   = uci:get("wireless", wifi.device, "channel") 
                table.insert(wireless, wifi)
              end)
  local ethernet = { }
  ethernet.interface = uci:get("network.lan.ifname")
  ethernet.address   = uci:get("network.lan.ipaddr")
  ethernet.mac       = os.pexecute("ifconfig " .. ethernet.interface .. " | grep HWaddr | awk '{ print $5 }'")
  ethernet.mac       = string.upper(ethernet.mac)
  local conf = {
    provision = "/cgi-bin/villagebus.lua/provision/router",
    deviceid  = uci:get("afrimesh.settings.deviceid") or "",
    root      = uci:get("afrimesh.settings.root") or factory.root,
    wireless  = wireless,
    ethernet  = ethernet,
    factory   = factory
  }
  return conf
end


--[[ main ]]----------------------------------------------------------------
function main(arg) 
  local conf = readconf();
  log:info("root: "    .. conf.root)
  log:info("id: "      .. conf.deviceid)
  log:info("mac: "     .. conf.wireless[1].mac)
  log:info("address: " .. conf.wireless[1].address)
  log:debug("conf : "  .. json.encode(conf))

  -- check if device has been provisioned
  common.REST({ host  = conf.root,
                verb  = "GET",
                path  = conf.provision,
                query = { 
                  deviceid = conf.deviceid,
                  mac      = conf.wireless[1].mac,
                  address  = conf.wireless[1].address 
                } 
              },
              function(error, data) 
                if error then  -- TODO
                  print("Fail: " .. error)
                  return
                end
                print("Got: " .. data)
              end)


--[[
name="/cgi-bin/villagebus/db/provision:$self:mac"
REQUEST="GET $name HTTP/1.0

"
provisioned_mac=`echo -n "$REQUEST" | nc $root 80 | grep jsonp | cut -d\" -f 2`

echo "provisioned: $provisioned_mac"
echo
[ "$wifi0_mac" == "$provisioned_mac" ] && { 
    logger "$wifi0_mac ($self) already provisioned, exiting."
    echo "$wifi0_mac ($self) already provisioned, exiting."
    echo
    exit  
}

# nuke any existing deviceid
uci del afrimesh.settings.deviceid
uci commit
]]--
  

end
main(arg)



