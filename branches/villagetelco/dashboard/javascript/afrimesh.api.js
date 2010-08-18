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
function _require(name) {
  var request = new XMLHttpRequest();    
  if (!request) {
    return console.log("Can't load: " + name);
  }
  request.open("GET", name, false); // true=async, false=sync
  request.send();
  var result = eval(request.responseText);
  console.log(name + " -> " + result);
};


/** - jquery ------------------------------------------------------------ */
_require("javascript/jquery/jquery.min.js");


/** - afrimesh ---------------------------------------------------------- */
var afrimesh = {};
afrimesh._ready = function() {
  console.log("default readyfn");
};
afrimesh.ready = function(f) {
  afrimesh._ready = f;
};




/** - fin --------------------------------------------------------------- */
$(document).ready(function() {
    afrimesh._ready();
  });
console.log("afrimesh.api.js");