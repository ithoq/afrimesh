/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootDevice(parent) {

  var device = function(address, continuation) { return this.device.info(address, continuation); };


  /**
   * Returns instantaneous data for the device at address
   *
   * TODO - we need a clear way to distinguish between info coming from
   *        direct query of device & buffered info coming from db
   */
  device.info = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/root/db/deviceinfo/" + address);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null);
        return continuation(error, response);
      });
    name = afrimesh.villagebus.Send(name);
    return name;
  };


  /** 
   * Returns temporal data for the device at address
   */
  device.stat = function(address, continuation, range) {
    var name = afrimesh.villagebus.Name("/root/db/lrange/devicestat/" + address);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null);  // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.Send(name, (range ? range : { start : 0, end : -1 }));
    return name;
  };


  return device;
};
console.debug("loaded afrimesh.device.js");


