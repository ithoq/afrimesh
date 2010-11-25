
var villagebus = function() {
  var ret = [];
  for (var p in this) { if (p != "prototype") ret.push(p); }
  return ret;
};


villagebus.proxy = function(prefix) {
  return prefix + "/cgi-bin/villagebus.lua/http/";
};

villagebus.xhr = function() {
  try { return new XMLHttpRequest(); } catch(e) { }
  try { return new ActiveXObject("Msxml2.XMLHTTP"); } catch (e) { }
  try { return new ActiveXObject("Microsoft.XMLHTTP"); } catch (e) { }
  alert("XMLHttpRequest not supported");
  return null;
};

villagebus.http = function(request) { // { verb, path, query, data }
  var xhr = villagebus.xhr();
  xhr.open(request.verb, request.path, request.continuation != null);
  xhr.onreadystatechange = function() {
    if (xhr.readyState != 4) return; // TODO handle all error states
    var response = JSON.parse(xhr.responseText);
    if (response.error) {
      return request.continuation(response.error, null);
    }
    return request.continuation(null, response);
  };
  if (request.data) {
    xhr.send(JSON.stringify(request.data));
  } else {
    xhr.send(null);
  }
  return xhr;
};


// HTTP Verbs: http://www.w3.org/Protocols/rfc2616/rfc2616-sec9.html
// . Safe: No action except retrieval
// . Idempotent: Side effects of N > 0 identical requests is same as for a single request


// idempotent, safe
villagebus.GET = function(url, continuation) {
  return villagebus.http({ verb : "GET", 
                           path : url, 
                           continuation : continuation });
};

// idempotent, safe
villagebus.HEAD = function() {
};

// idempotent, unsafe
villagebus.PUT = function() {
};

// idempotent, unsafe
villagebus.DELETE = function() {
};

// unsafe
villagebus.POST = function(url, data, continuation) {
  return villagebus.http({ verb : "POST", 
                           path : url, 
                           data : data,
                           continuation : continuation });
};

// no side-effects
villagebus.TRACE = function() {
};

// no side-effects
villagebus.OPTIONS = function() {
};

// ? "This specification reserves the method name CONNECT for use with a 
//    proxy that can dynamically switch to being a tunnel (e.g. SSL 
//    tunneling [44])."
villagebus.CONNECT = function() {
};

