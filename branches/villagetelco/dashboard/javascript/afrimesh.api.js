/**
 * Afrimesh.API: easy programming for mesh networks
 *
 * Copyright (C) 2010 Antoine van Gelder
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/** - XMLHttpRequest ---------------------------------------------------- */
if (typeof XMLHttpRequest == "undefined") {
  XMLHttpRequest = function () {
    try { return new ActiveXObject("Msxml2.XMLHTTP.6.0"); } catch (e) {}
    try { return new ActiveXObject("Msxml2.XMLHTTP.3.0"); } catch (e) {}
    try { return new ActiveXObject("Msxml2.XMLHTTP");     } catch (e) {}
    throw new Error("This browser does not support XMLHttpRequest.");
  };
}


/** - require ----------------------------------------------------------- */
var exports = {};
function _require(names, async, continuation) {
  if (typeof names === "string") {
    return _getScript(names, async, continuation);
  } else if (Object.prototype.toString.call(names) != "[object Array]") {
    return console.log("Invalid name: " + names);
  }
  var pool = [];
  for (var n in names) {
    pool.push(_getScript(names[n], async));
  }
  function poll(p) {
    var done = p.every(function(request) { return request.done == true; })
    if (done) return continuation(exports);
    setTimeout(function() { poll(p); }, 100);
  };
  poll(pool);
};

// Also see: http://unixpapa.com/js/dyna.html
function _getScript(name, async, continuation) { 
  var request = new XMLHttpRequest();    
  if (!request) {
    return console.log("Can't load: " + name);
  }
  request.onreadystatechange = function() {
    if (request.readyState == 4 && request.status == 200) {
      var result = eval(request.responseText);
      request.done = true;
      if (continuation) {
        continuation(request.responseText);
      }
    } else {
      // TODO - error handling
    }
  };
  request.open("GET", name, async); 
  request.send();
  return request;
}


/** - jquery ------------------------------------------------------------ */
_require("javascript/jquery/jquery.min.js", false);


/** - afrimesh.ready() -------------------------------------------------- */
var afrimesh = {};
afrimesh._ready = function() {
  console.log("default readyfn");
};
afrimesh.ready = function(f) {
  afrimesh._ready = f;
};


/** - afrimesh bootstrap ------------------------------------------------ */
$(document).ready(function() {
    console.log("jquery ready");
    _require(["javascript/afrimesh.utilities.js",
              "javascript/afrimesh.platform.js",
              "javascript/afrimesh.villagebus.js",
              "javascript/afrimesh.storage.js", 
              "javascript/afrimesh.network.js", 
              "javascript/afrimesh.device.js",
              "javascript/afrimesh.community.js",
              "javascript/afrimesh.person.js", 
              "javascript/afrimesh.customers.js", 
              "javascript/afrimesh.telephony.js", 
              "javascript/afrimesh.settings.js" ], 
             true, 
             function(exports, data) {
                 //_require("javascript/afrimesh.js", false);
                 //console.log("BootStorage: " + BootStorage);
                 console.log(exports);
                 console.log("this.BootStorage: " + typeof exports.BootStorage);
                 afrimesh.storage = exports.BootStorage(afrimesh);
                 afrimesh._ready(); 
               });
  });
console.log("afrimesh.api.js");