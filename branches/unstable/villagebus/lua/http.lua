
--[[ includes ]]------------------------------------------------------------
require "socket"
local http = require "socket.http"
local ltn12 = require("ltn12")

module("villagebus.http", package.seeall)


--[[ evaluate ]]------------------------------------------------------------
function evaluate(request) -- request = { verb, path, query, data }

  -- build url
  local hostname = table.remove(request.path, 1)
  if not hostname then
    return fail("no hostname in request", "http")
  end
  local url  = "http://" .. hostname .. "/" .. table.concat(request.path, "/") 
  if request.query then
    url = url .. "?" .. urlcode.encodetable(request.query)
  end

  -- unless I can figure out how to access header data in ltn12 we have to buffer the remote server response :-/
  local content = ""
  function sink(chunk, err)
    if chunk then content = content .. chunk end
    return 1
  end
  local chunk = true
  function source()
    if not chunk then return nil end
    chunk = false
    return request.data
  end

  -- build the request body
  log:info(request.verb .. " " .. url)
  local body = {
    method = request.verb,
    url    = url,
    sink   = sink
  }
  if request.data then
    request.data = json.encode(request.data)
    body.headers = {}
    body.headers["content-type"]   = "application/json"
    body.headers["content-length"] = string.len(request.data)
    body.source = source
  end

  -- fire the request
  local result  = 0
  local code    = 0
  local headers = {}
  local reply   = ""
  result, code, headers, reply = http.request(body)
  
  -- output response
  for key, val in pairs(headers) do
    print(key .. ": " .. val)
  end
  print()
  print(content)

  return nil
end


--[[

    sink   = ltn12.sink.file(io.stdout),
    step   = function(source, sink) 
               print(headers)
               for key, val in pairs(headers) do
                 print(key .. ": " .. val)
               end
               local chunk, src_err = source()
               local ret, snk_err = sink(chunk, src_err)
               return chunk and ret and not src_err and not snk_err, src_err or snk_err
             end

]]--