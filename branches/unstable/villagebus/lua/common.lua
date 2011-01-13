
--[[ includes ]]------------------------------------------------------------
require "socket"
http = require "socket.http"
ltn12 = require("ltn12")


--[[ helpers ]]-------------------------------------------------------------
require "urlcode"
function getcgi(name)
  local value = os.getenv(name)
  return value and value or ""
end
function fail(message, module)
  if module then
    log:error("module " .. module .. ": " .. message)
  else
    log:error(message)
  end
  return json.encode({ error = message })
end

module("common", package.seeall)


-- Return the output of a shell command as a string
function os.pexecute(cmd, raw)
  local f = assert(io.popen(cmd, 'r'))
  local s = assert(f:read('*a'))
  f:close()
  if raw then return s end
  s = string.gsub(s, '^%s+', '')
  s = string.gsub(s, '%s+$', '')
  s = string.gsub(s, '[\n\r]+', ' ')
  return s
end


-- Perform requests against REST API's
function REST(request, continuation) -- { host, port, verb, path, query, data }

  -- build url
  if not request.port then
    request.port = 80
  end
  local url  = "http://" .. request.host .. ":" .. request.port .. request.path
  if request.query then
    url = url .. "?" .. urlcode.encodetable(request.query)
  end

  print(request.verb .. " " .. url)

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
  -- http.USERAGENT = "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_5; en-us)"
  result, code, headers, reply = http.request(body)
  if not headers then
    return fail("request returned no headers", "http")
  end
  
  -- output response
  if not headers["content-length"] then 
    log:debug("Adding 'content-length : " .. string.len(content) .. "' header")
    headers["content-length"] = string.len(content)
  end
  if headers["transfer-encoding"] == "chunked" then 
    log:debug("Stripping transfer-encoding header")
    headers["transfer-encoding"] = nil
  end
  log:debug("------------------------------------------------")
  for key, val in pairs(headers) do
    print(key .. ": " .. val)
    log:debug(key .. ": " .. val)
  end
  print()
  print(content)
  log:debug("")
  log:debug(content)
  log:debug("------------------------------------------------")
  return nil


end
