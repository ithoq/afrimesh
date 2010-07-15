/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootSystem(parent) {

  var system = function(address, continuation) { return this.system.uname(address, continuation); };


  /**
   * Returns the system log
   */
  system.log = function(continuation, count) {
    var name = afrimesh.villagebus.Name("/root/sys/syslog");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name, { count : (count?count:10) });
    return name;
  };

  
  return system;
};
console.debug("loaded afrimesh.systemy.js");

