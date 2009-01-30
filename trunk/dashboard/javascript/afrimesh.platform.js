/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */
(function() {

  var platform = {
    browser : {},
    shell   : {}
  };
  platform.browser.load = function(uri) {
    if (!this.includes) { this.includes = {}; }
    if (this.includes.hasOwnProperty(uri)) {
      //console.warn("Attempting to load " + uri + " multiple times.");
      return this.includes[uri];
    }
    this.includes[uri] = true;
    $.ajax({ 
        url: uri,
          dataType: "script",
          async: false,
          success: 
        function(js) {
          /**if (jQuery.browser.safari) { eval(js);  }*/
        }
      });
    console.debug("utilities.loaded " + uri);
  };
  

    /** - capability tests ---------------------------------------------------- */
  try { load; } catch (e) {
    load = platform.browser.load;
  }

  // TODO move contents of unit.shell to here  
  // TODO move console from utilities to here

})();
console.debug("loaded afrimesh.platform.js");

