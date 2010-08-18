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
// Also see: http://unixpapa.com/js/dyna.html
function _require(names, async, continuation) {
  if (typeof names === "string") {
    return _getScript(names, async, continuation);
  } else if (Object.prototype.toString.call(names) === "[object Array]") {
    console.log("Loading multiple names: " + names);
  } else {
    console.log("Invalid name: " + names);
    return;
  }
  var pool = [];
  for (var n in names) {
    pool.push(_getScript(names[n], async));
  }
  function poll(p) {
    var done = p.every(function(request) { return request.done == true; })
    if (done) return continuation();
    setTimeout(function() { poll(p); }, 100);
  };
  poll(pool);

};

function _getScript(name, async, continuation) {
  var request = new XMLHttpRequest();    
  if (!request) {
    return console.log("Can't load: " + name);
  }
  request.onreadystatechange = function() {
    /*console.log("readyState: " + request.readyState);
    if (request.readyState > 1) {
      console.log("status: " + request.status);
    }*/
    if (request.readyState == 4 && request.status == 200) {
      var result = eval(request.responseText);
      request.done = true;
      if (continuation) {
        continuation(request.responseText);
      }
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
             function(data) {
                 //_require("javascript/afrimesh.js", false);
                 //afrimesh.storage = BootStorage(afrimesh);
                 afrimesh._ready(); 
               });
  });
console.log("afrimesh.api.js");