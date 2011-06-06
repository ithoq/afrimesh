/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootNetwork(parent) {

  var network = function() { return this.network.routes(); };


  /* - network device info ---------------------------------------------- */

  /**
   * Returns a list of all devices on the network annotated w/ instantaneous device information
   * BUFFERED
   */
  network.status = function(continuation) {
    var name = afrimesh.villagebus.Name("/@self/db/keys/device:*/status");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null);  // TODO - return Fail(error, continuation) maybe ?
        response.map(function(key) {
            return afrimesh.villagebus.GET(afrimesh.villagebus.Bind("/@self/db/" + key, function(error, response) {
                return continuation(error, response.self, response);
              }));
          });
      });
    name = afrimesh.villagebus.GET(name);
    return name;
  };


  /**
   * Deprovisions the given device from the network
   */
  network.release = function(device, continuation) {
    var name = afrimesh.villagebus.Name("/@self/provision/release/" + device.ip); 
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.DELETE(name);
    return name;
  };


  /* - network monitoring ----------------------------------------------- */

  /**
   * Returns the network log
   */
  network.log = function(continuation, count) {
    var name = afrimesh.villagebus.Name("/@self/sys/syslog");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name, { count : (count?count:10) });
    return name;
  };




  /* - network accounting ----------------------------------------------- */

  /**
   * Raw accounting info as reported by pmacct
   */
  network.accounting = function(continuation) {
    var name = afrimesh.villagebus.Name("/@pmacct/accounting/gateway"); 
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.GET(name);
    return name;
  };

  /** 
   * decorates a router with network accounting information 
   *
   * TODO - network.accounting should a) be a map keyed w/ IP and b) cached 
   */
  network.accounting.router = function(router, continuation) { 
    var temp = 0;
    router.traffic = { bytes : 0, packets : 0 };
    try { 
      return network.accounting(function(error, entries) {
          if (error) return console.error("Could not retrieve network accounting information: " + error);
          entries.map(function(entry) {		
              if (router.address == entry.DST_IP) {
                if (typeof router.mac == "undefined") {	router.mac = entry.DST_MAC; } 
                router.traffic.bytes   += parseInt(entry.BYTES);
                router.traffic.packets += parseInt(entry.PACKETS);
              }
            });
          continuation(null, router);
        });
    } catch (error) {
      console.debug("pmacct error " + error);
    }
  };

  /** 
   * decorates a list of routers with network accounting information 
   */
  network.accounting.routers = function(routers, continuation) {
    var targets = {};
    routers.map(function(router) {
        router.traffic = { bytes : 0, packets : 0 };
        targets[router.address] = router;
        console.debug("Added: " + router.address);
      });
    return network.accounting(function(error, entries) { 
        if (error) return console.error("Could not retrieve network accounting information: " + error);
        entries.map(function(entry) {
            var router = targets[entry.DST_IP];
            if (router) {
              console.debug(entry.DST_IP + " (" + entry.DST_MAC + ") -> " + entry.PACKETS + " packets " + entry.BYTES + " bytes");
              if (typeof router.mac == "undefined") {	router.mac = entry.DST_MAC; } 
              router.traffic.bytes   += parseInt(entry.BYTES);
              router.traffic.packets += parseInt(entry.PACKETS);
            }
          });
        routers = [];
        for (var router in targets) {
          router = targets[router];
          routers.push(router);
        }
        continuation(null, routers);
      });
  };


  /* - network topology ------------------------------------------------- */

  network.routes = function(continuation) {
    var name = afrimesh.villagebus.Name("/@topology");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.jsonp_to_json(name, null, true); // vis server outputs raw JSON
    name = afrimesh.villagebus.GET(name);
    return name;
  };

  network.routers = function(continuation) {
    return network.routes(function(error, routes) {
        if (error) return continuation(error, null);
        var routers  = [];
        var included = {};
        routes.map(function(route) {
            if (!included[route.router]) { 
              router = { address : route.router,
                         routes  : [ route ]     };
              routers.push(router);
              included[router.address] = router;
            } else {
              included[router.address].routes.push(route);
            }
          });
        return continuation(null, routers);
      });
  };
  
  return network;
};
exports.BootNetwork = BootNetwork;
console.debug("loaded afrimesh.network.js");


