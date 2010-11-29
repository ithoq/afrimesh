#!/usr/bin/env xpcshell

/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/** Detect platform and set up test execution environment ----------------- */
var platform = undefined;
if (XMLHttpRequest === undefined) {
  var XMLHttpRequest = null;
  load("unit/shell.js");
  platform = Shell;
} else {
  platform = Object;
  main = function() {};
}
load("unit.js");


with (platform) with (Unit) {
  
  NetworkingTests = {
    /** Perform a XMLHttpRequest ------------------------------------------ */
    xmlhttprequest : function() {
      xml = XMLHttpRequest();
      xml.open("GET", "http://dashboard.7degrees.co.za:2004/", true);
      //xml.open("GET", "http://www.google.com", true);
      xml.onreadystatechange = function() { 
        log("readyState: ", xml.readyState);
        if (xml.readyState == 4) {
          log("responseXML: ", xml.responseXML);
          log("responseText: ", xml.responseText);
        }
      };
      xml.onload     = function(event) { log("onload     : ", event); };
      xml.onerror    = function(event) { log("onerror    : ", event); };
      xml.onprogress = function(event) { log("onprogress : ", event); };
      xml.send("{}");
      
      /** Enter main event loop */
      setTimeout(function() { exit(); }, 2000);
      main();
      return true;
    },

    /** Perform some test --------------------------------------------------*/
    sometest : function() {
      throw("TODO");
    }
  }; /** NetworkingTests */
  
  
  FooTests = {
    plonk : function() { return true; }
  };

  
  /** Run tests ------------------------------------------------------------ */
  /**log = function() { 
    var s = "";
    for (arg = 0; arg < arguments.length; arg++) {
      s += arguments[arg] + "";
    }
    print("\tMYLOG: ", s);
  };*/
  //dump_log = function() {};
  TestRunner.run(NetworkingTests, FooTests);
  
}; /** with(platform) with(Unit) */


/** Check namespace pollution */
/** print("\n---------------------------------------------------------------");
print("Namespace contains: ");
for (i in this) {
  print("\t" + i + " : " + this[i]);
}*/
