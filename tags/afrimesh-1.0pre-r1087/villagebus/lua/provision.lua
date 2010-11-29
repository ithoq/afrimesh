
module("villagebus.provision", package.seeall)

function router(request) -- request = { verb, path, query, data }
  return request;
end
