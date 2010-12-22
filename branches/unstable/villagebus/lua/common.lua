module("common", package.seeall)

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

function REST(request, continuation) -- { host, port, verb, path, query, data }
  print("GETTING: " .. json.encode(request))
end
