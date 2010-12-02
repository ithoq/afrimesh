/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootDevice(parent) {

  var device = function(address, continuation) { return this.device.status(address, continuation); };


  /* - DB Buffered -------------------------------------------------------- 
     These methods are pulling buffered data from redis so are usually the
     fastest & most reliable way to pull information about particular 
     devices from the mesh */

  /**
   * Returns instantaneous DB data for the device at address
   */
  device.status = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@self/db/device:" + address + "/status");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null);
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name);
    return name;
  };


  /** 
   * Returns temporal DB data for the device at address
   */
  device.interface = {};
  // TODO - device.interface.list 
  device.interface.history = function(address, continuation, range) {
    var name = afrimesh.villagebus.Name("/@self/db/lrange/interface:" + address + "/history");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null);  // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response);
      });
    name = afrimesh.villagebus.GET(name, (range ? range : { start : 0, end : -1 }));
    return name;
  };


  /* - Direct ------------------------------------------------------------- 
     These methods are making network requests directly to the device 
     address. Their performance are thus highly dependent on mesh weather
     and available routing between the browser and the mesh nodes */

  /** 
   * SNMP access to device
   */
  device.snmp = function(address, community, oids, continuation) {
    var name = afrimesh.villagebus.Name("/@self/snmp/get/" + address + "/" + community + "/" + oids.join(","));
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name);
    return name;
  };
  device.snmp.walk = function(address, community, oid, continuation) {
    var name = afrimesh.villagebus.Name("/@self/snmp/walk/" + address + "/" + community + "/" + oid);
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name);
    return name;
  };

  
  /**
   * Access device configuration
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
    //name = afrimesh.villagebus.PUT(name, entries);
    name = afrimesh.villagebus.POST(name, entries); // Only OpenWRT webserver currently supporting PUT is my hacked mini-httpd :-/
    return name;
  };


  /**
   * Device location
   */
  device.location = function(address, continuation) {
    // set on device
    return device.configuration(address, "afrimesh.location", function (error, config) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ? 
        return continuation(null, config.afrimesh.location.longitude, config.afrimesh.location.latitude); // TODO - fix
      });
  };
  device.location.set = function(address, longitude, latitude, continuation) {
    return device.configuration.set(address, [
      { config: "afrimesh", section: "location", option: "longitude", value: longitude.toString() }, 
      { config: "afrimesh", section: "location", option: "latitude",  value: latitude.toString()  } ], continuation);
  };


  /**
   * Return OS uname for device
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


  /** 
   * Device package management
   */
  device.package = { };
  device.package.update  = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/package/update");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name);    
    return name;
  };
  device.package.list    = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/package/list");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name);    
    return name;
  };
  device.package.status  = function(address, name, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/package/status/" + name);
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name);    
    return name;
  };
  device.package.upgrade = function(address, name, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/package/upgrade/" + name);
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name);    
    return name;
  };

  return device;
};
exports.BootDevice = BootDevice;
console.debug("loaded afrimesh.device.js");


