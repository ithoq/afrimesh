/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */
(function() {

  var platform = {
    browser  : {},
    shell    : {},
    includes : {}
  };
  // TODO - investigate http://code.google.com/p/ajaxsoft/wiki/xLazyLoader
  platform.browser.load = function(uri) {
    if (platform.includes.hasOwnProperty(uri)) {
      console.debug("Attempting to load " + uri + " multiple times.");
      return platform.includes[uri];
    }
    var data = $.ajax({ 
        url: uri,
        dataType: "script",
        async: false,
        success: function(js) {
          platform.includes[uri] = true;
          /**if (jQuery.browser.safari) { eval(js);  }*/
        }
      });
    console.debug("utilities.loaded " + uri);
    return data;
  };

  platform.browser.load.async = function(uri, f) {
    if (platform.includes.hasOwnProperty(uri)) {
      console.debug("Attempting to load " + uri + " multiple times.");
      f("jquery suckz", "success");
      return platform.includes[uri];
    }
    $.getScript(uri, function(data, status) {
        platform.includes[uri] = true;
        console.debug("utilities.loaded.async " + uri);
        f("jquery suckz", "success");
      });
  };
  
  platform.browser.sleep = function() {
  };

  /** - capability tests ---------------------------------------------------- */
  try { load; } catch (e) {
    load = platform.browser.load;
  }
  try { sleep; } catch (e) {
    sleep = platform.browser.sleep;
  }

  // TODO move contents of unit.shell to here  
  // TODO move console from utilities to here

})();

console.debug("loaded afrimesh.platform.js");


