/**
 * Afrimesh.API: easy programming for mesh networks
 */


/** - Console ----------------------------------------------------------- */
if (typeof(window.console) == "undefined") {  // Some browsers really have no console at all
  window.console = { debug   : function(){},
                     warning : function(){},
                     error   : function(){},
                     log     : function(){} };
  console = window.console;
}



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
    /*console.log("state: " + request.readyState + (request.readyState > 1 
                                                  ? " status: " + request.status 
                                                  : ""));*/
    if (request.readyState == 4) { // && request.status == 200) { TODO - when loading file:// resources you get 0 for status ret
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


/** - afrimesh ---------------------------------------------------------- */
var afrimesh = {};
console.log("Window Location: " + window.location.hostname);
afrimesh.settings = { 
  //address      : window.location.hostname ? window.location.hostname : "196.211.3.106",
  //address      : window.location.hostname ? window.location.hostname : "192.168.20.225",
  //address      : window.location.hostname ? window.location.hostname : "10.0.0.4",
  address      : window.location.hostname ? window.location.hostname : "192.168.20.108",
  //ajax_proxy   : "/cgi-bin/ajax-proxy.cgi?url="
  ajax_proxy   : "/cgi-bin/villagebus.lua",
  //edge         : "/http/192.168.20.108/cgi-bin/villagebus.lua"
};
console.log("afrimesh.settings.address : " + afrimesh.settings.address);


/* - afrimesh.ready() --------------------------------------------------- */
afrimesh._ready = function() {
  console.log("default readyfn");
};
afrimesh.ready = function(f) {
  afrimesh._ready = f;
};


/** - bootstrap --------------------------------------------------------- */
_require("javascript/jquery/jquery.min.js",  true, function() {
  $(document).ready(function() {
    jQuery.getScript("javascript/json.org/json2.min.js"); // TODO
    jQuery.getScript("javascript/afrimesh.utilities.js"); // TODO 
    jQuery.getScript("javascript/afrimesh.platform.js");  // TODO

    _require(["javascript/afrimesh.villagebus.js",
              "javascript/afrimesh.storage.js", 
              "javascript/afrimesh.network.js", 
              "javascript/afrimesh.device.js",
              "javascript/afrimesh.community.js",
              "javascript/afrimesh.person.js", 
              "javascript/afrimesh.customers.js", // TODO deprecate
              "javascript/afrimesh.telephony.js", // TODO s/telephony/handset
              "javascript/afrimesh.billing.js",
              "javascript/afrimesh.settings.js"], 
             true, 
             function(exports, data) {
               // register API modules
               afrimesh.storage    = exports.BootStorage(afrimesh);
               afrimesh.network    = exports.BootNetwork(afrimesh);
               afrimesh.device     = exports.BootDevice(afrimesh); 
               afrimesh.community  = exports.BootCommunity(afrimesh);
               afrimesh.person     = exports.BootPerson(afrimesh);
               afrimesh.customers  = exports.BootCustomers(afrimesh);
               afrimesh.telephony  = exports.BootTelephony(afrimesh); 
               afrimesh.billing    = exports.BootBilling(afrimesh);      
               afrimesh.villagebus = exports.BootVillageBus(afrimesh);
               
               // override default settings with live settings from the server
               console.log("Booting config from: " + afrimesh.settings.address);
               afrimesh.settings = exports.BootSettings(afrimesh, afrimesh.settings.address); 
               console.log("afrimesh.settings.address : " + afrimesh.settings.address);
               console.debug("completed afrimesh node bootstrap");
               afrimesh._ready(); 
             });
  });
});
console.debug("loaded afrimesh.js");
