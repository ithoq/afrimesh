/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/**
 * Storage via:
 *
 *  cookies
 *  uci
 *  memcachedb
 *  browser databases?
 *  memcacheq?
 *  mysql?
 */
function BootStorage(parent) {

  //console.debug("COOKIES: |" + document.cookie + "|");
  //var storage = function() { return this.storage.roots(); };

  var storage = function() { return this.storage.root(arguments[0], arguments[1], arguments[2]); };
  /*var storage = function() { return this.storage.root((arguments.length >= 1) ? arguments[0] : undefined, 
                                                      (arguments.length >= 2) ? arguments[1] : undefined, 
                                                      (arguments.length == 3) ? arguments[2] : 0); };*/

  storage.roots  = function() { return { cookies : storage.cookies }; };

  storage.root = function(key, value, expiry) {
    //console.debug("storage.root(" + key + ", " + value + ", " + expiry + ")");
    return this.cookie(key, value, expiry);
  };

  storage.cookie = function(key, value, expiry) {
    //console.debug("storage.cookie(" + key + ", " + value + ", " + expiry + ")");
    if (typeof value == 'undefined') {
      return this.cookie.read(key);
    } 
    return this.cookie.write(key, value, expiry);
  };
  storage.cookie.read  = function(key) { 
    //console.debug("storage.cookie.read(|" + key + "|)");
    var matches = document.cookie.split(";").filter(function(cookie) {
        cookie = cookie.trim();
        return (cookie.substring(0, key.length + 1) == (key + "="));
      });
    return (!matches.length ? "" : decodeURIComponent(matches[0].substring(key.length + 1)));
  };

  storage.cookie.write = function(key, value, expiry) { 
    //console.debug("storage.cookie.write(|" + key + "|, |" + value + "|, |" + expiry + "|)");
    if (typeof expiry == "undefined") {
      expiry = "";
    } else {
      var date = new Date();
      date.setTime(date.getTime() + (expiry * 24 * 60 * 60 * 1000));
      expiry = "; expires=" + date.toGMTString();
    }
    document.cookie = key + "=" + value + expiry + "; path=/";
    return value;
  };


  return storage;
};
exports.BootStorage = BootStorage;
console.debug("loaded afrimesh.storage.js");
