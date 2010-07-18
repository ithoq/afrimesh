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


  /* - DB Buffered -------------------------------------------------------- */

  /**
   * Returns instantaneous DB data for the device at address
   * BUFFERED
   * TODO - we need a clear way to distinguish between info coming from
   *        direct query of device & buffered info coming from db
   */
  device.info = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@root/db/deviceinfo/" + address);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null);
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name);
    return name;
  };


  /** 
   * Returns temporal DB data for the device at address
   * BUFFERED
   */
  device.stat = function(address, continuation, range) {
    var name = afrimesh.villagebus.Name("/@root/db/lrange/devicestat/" + address);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null);  // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name, (range ? range : { start : 0, end : -1 }));
    return name;
  };


  /* - Direct ------------------------------------------------------------- */

  /**
   * Access device configuration
   * DIRECT
   */
  device.configuration = function(address, key, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/config/" + key);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response[0]);
      });
    name = afrimesh.villagebus.GET(name);
    return name;
  };
  device.configuration.set = function(address, entries, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/config");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.PUT(name, entries);
    return name;
  };


  /**
   * Device location
   * DIRECT
   */
  device.location = function(address, continuation) {
    // TODO - cheap'n'very nasty hack for demos - remove before 1.0 release.
    //if (afrimesh.settings.network.mesh.vis_server == demo_server) {
    //  address = afrimesh.settings.network.mesh.vis_server + afrimesh.settings.ajax_proxy + "http://" + address;
    //} 
    return device.configuration(address, "afrimesh.location", function (error, config) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ? 
        return continuation(config.afrimesh.location.longitude, config.afrimesh.location.latitude);
      });
  };
  device.location.set = function(address, longitude, latitude, continuation) {
    return device.configuration.set(address, [
      { config: "afrimesh", section: "location", option: "longitude", value: longitude.toString() }, 
      { config: "afrimesh", section: "location", option: "latitude",  value: latitude.toString()  } ], continuation);
  };


  /**
   * Return OS uname for device
   * DIRECT
   */
  device.uname = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/sys/uname");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name);
    return name;
  };

  /**
   * Return Afrimesh version for device
   * DIRECT
   */
  device.version = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/sys/version");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name);
    return name;
  };

  /**
   *  Device Services
   *  DIRECT
   */
  device.service = function(address, service, continuation, command) {
    var name = afrimesh.villagebus.Name("/@" + address + "/sys/service/" + service);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name, { command : (command?command:"status") });
    return name;
  };
  device.service.reload = function(address, service, continuation) {
    // TODO - think a bit about GET/PUT/POST semantics for various service commands
    return device.service(address, service, continuation, "reload");
  };




  return device;
};
console.debug("loaded afrimesh.device.js");


