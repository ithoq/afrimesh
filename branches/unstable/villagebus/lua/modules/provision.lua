
--module("villagebus.provision", package.seeall)
module("modules.provision", package.seeall)
--module(..., package.seeall)

function router(request) -- request = { verb, path, query, data }
  return request;
end
