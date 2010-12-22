
--module("villagebus.provision", package.seeall)
module("modules.provision", package.seeall)
--module(..., package.seeall)


-- http://wiki.openwrt.org/doc/devel/uci-lua
package.cpath = package.cpath .. ";/Users/antoine/afrimesh/ext/darwin/lib/lua5.1/?.so"
package.cpath = package.cpath .. ";/usr/share/lua/5.1/?.so"
uci = require("uci")
uci = uci.cursor("/tmp/foo")

function configure()

end


--[[ interface ]]-----------------------------------------------------------
function interface(request) -- request = { verb, path, query, data }

  return request;
end
