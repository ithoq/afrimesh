#!/usr/bin/lua

--[[ Dependencies
  -- ubuntu
  sudo apt-get install luarocks
  sudo apt-get install liblua5.1-socket2 liblua5.1-socket-dev
  sudo apt-get install liblua5.1-json 
  sudo apt-get install liblua5.1-logging liblua5.1-logging-dev

  -- mac
  sudo port install luarocks
  sudo luarocks install luasocket
  sudo luarocks install luajson
  sudo luarocks install lualogging
  sudo luarocks install redis-lua

  libuci-lua
]]--


--[[ includes ]]------------------------------------------------------------
require "luarocks.require"
require "json"

require "logging"
require "logging.file"
logname = "/tmp/villagebus.lua.log"
os.execute("touch " .. logname .. " >& /dev/null")
os.execute("chmod 0666 " .. logname  .. " >& /dev/null")
log = logging.file(logname)
if not log then
  log = logging.file("/dev/null")
end
-- log:setLevel(logging.INFO)
log:setLevel(logging.DEBUG)
log:debug("villagebus.lua starting")


--[[ modules ]]-------------------------------------------------------------
require "common"
require "modules.http"
require "modules.provision"



--[[ main ]]----------------------------------------------------------------
function main(arg) 
  log:debug("-- CGI ----------------------------------------")
  local cgi = {
    request_method  = getcgi("REQUEST_METHOD"),
    path_info       = getcgi("PATH_INFO"),
    query_string    = getcgi("QUERY_STRING"),
    content_type    = getcgi("CONTENT_TYPE"),
    content_length  = tonumber(getcgi("CONTENT_LENGTH")),
    request_uri     = getcgi("REQUEST_URI"),
    auth_type       = getcgi("AUTH_TYPE"),
    --remote_user     = getcgi("REMOTE_USER"),
    --remote_address  = getcgi("REMOTE_ADDRESS"),
    --remote_host     = getcgi("REMOTE_HOST"),
    --http_referer    = getcgi("HTTP_REFERER"),
    --path_translated = getcgi("PATH_TRANSLATED")
  }
  for key, value in pairs(cgi) do
    if value ~= "" then
      log:debug(key .. "\t: " .. value)
    end
  end  

  -- read request ----------------------------------------------------------
  log:debug("-- REQUEST ------------------------------------")
  local request = {
    verb  = cgi.request_method,
    path  = {},
    query = nil,
    data  = nil
  }
  -- for name in string.gmatch(cgi.path_info, "[^\\/]+") do
  for name in cgi.path_info:gmatch("[^\\/]+") do
    table.insert(request.path, name)
  end
  if cgi.query_string ~= "" then
    request.query = {}
    urlcode.parsequery(cgi.query_string, request.query)
  end
  if cgi.content_length then
    request.data = json.decode(io.read("*a"))
  end
  for key, value in pairs(request) do
    log:debug(key .. "\t: " .. json.encode(value))
  end

  -- evaluate request ------------------------------------------------------
  log:debug("-- EVAL ---------------------------------------")
  local name = table.remove(request.path, 1)
  local module = modules[name]       
  local response = nil
  if type(module) ~= "table" then
    response = fail("Could not resolve module for name: " .. (name or "nil"))
  elseif module["evaluate"] then       -- look for an 'evaluate' function
    log:info(name .. ".evaluate(" .. json.encode(request) .. ")")
    response = module["evaluate"](request)
  elseif module[request.verb] then     -- try REST verbs
    log:info(name .. "." .. request.verb .. "(" .. json.encode(request) .. ")")
    response = module[request.verb](request)
  else                                 -- search module methods
    local method = table.remove(request.path, 1)
    if module[method] then
      log:info(name .. "." .. method .. "(" .. json.encode(request) .. ")")
      response = module[method](request)
    else
      response = fail("Could not resolve name '" .. method .. "' in module: " .. name)
    end
  end

  -- print response --------------------------------------------------------
  if response then
    --print("Content-Type: text/plain\n");
    print("Content-Type: application/json\n");
    print(json.encode(response))
    log:info("RESPONSE: " .. json.encode(response))
  else
    log:info("RESPONSE: stdout")
  end

  log:info("================================================")
  log:info("")
end
main(arg)

--[[
http://lambda.7degrees.co.za/cgi-bin/villagebus.lua/http/192.168.20.108/cgi-bin/villagebus.lua/http/10.130.1.1:2005
http://lambda.7degrees.co.za/cgi-bin/villagebus.lua/http/@reginald/10.130.1.1:2005
http://lambda.7degrees.co.za/cgi-bin/villagebus.lua/@reginald/http/10.130.1.1:2005
]]--

