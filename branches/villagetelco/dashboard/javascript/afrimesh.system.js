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
    name = afrimesh.villagebus.Send(name, { count : (count?count:10) });
    return name;
  };

  /**
   *
   */
  system.uname = function(address, continuation) {
    //var name = afrimesh.villagebus.Name("//" + address + "/cgi-bin/villagebus/sys/uname");
    var name = afrimesh.villagebus.Name("/@" + address + "/sys/uname");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.Send(name);
    return name;
  };

  /**
   *
   */
  system.version = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/sys/version");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.Send(name);
    return name;
  };

  /**
   *
   */
  system.service = function(address, service, continuation, command) {
    var name = afrimesh.villagebus.Name("/@" + address + "/sys/service/" + service);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.Send(name, { command : (command?command:"status") });
    return name;
  };
  system.service.status = function(address, service, continuation) {
    return system.service(address, service, continuation, "status");
  };
  system.service.reload = function(address, service, continuation) {
    return system.service(address, service, continuation, "reload");
  };

  
  return system;
};
console.debug("loaded afrimesh.systemy.js");

