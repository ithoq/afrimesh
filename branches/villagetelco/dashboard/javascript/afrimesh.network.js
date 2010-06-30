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

  /* - network devices -------------------------------------------------- */
  network.devices = function(continuation) {
  };

  /* - network status --------------------------------------------------- */

  /**
   * 
   */
  network.status = function(continuation) {
    var name = afrimesh.villagebus.Name("/root/db/keys/status/*");
    name = afrimesh.villagebus.Bind(name, continuation);
    channel = afrimesh.villagebus.Send(name /*, args*/);
    return channel;

    // query db for available status queues
    /*var xhr = $.ajax({ 
        type    : "GET",
        url     : "http://192.168.20.105/cgi-bin/villagebus/root/db/keys/status/*",
        contentType : "application/json",
        dataType    : "jsonp",
        context: document.body,
        success : function(data) {
          console.log("GREAT SUCCESS: ");
          console.log(data);
          data.map(function(device) {
            continuation(null, device);
          });
        },
        error   : function(data) {
          console.log("Error: ");
          console.log(data);
          continuation(data, null);
        }
      });  */
  };

  //network.device.status = function(continuation) {  };


  /* - network accounting ----------------------------------------------- */

  /** 
   * decorates a router with network accounting information 
   */
  network.accounting = function(router) { /* TODO - afrimesh.villagebus.acct a) should be a map keyed w/ IP b) cached */
    var temp = 0;
    router.traffic = { bytes : 0, packets : 0 };
    try { 
      afrimesh.villagebus.acct.gateway().map(function(entry) {		
          if (router.address == entry.DST_IP) {
            if (typeof router.mac == "undefined") {	router.mac = entry.DST_MAC; } 
            router.traffic.bytes   += parseInt(entry.BYTES);
            router.traffic.packets += parseInt(entry.PACKETS);
          }
        });
    } catch (error) {
      console.debug("pmacct error " + error);
    }
    return router;
  };

  /** 
   * decorates a list of routers with network accounting information 
   */
  network.accounting.routers = function(routers) {
    var targets = {};
    routers.map(function(router) {
        router.traffic = { bytes : 0, packets : 0 };
        targets[router.address] = router;
        console.debug("Added: " + router.address);
      });
    afrimesh.villagebus.acct.gateway().map(function(entry) {
        var router = targets[entry.DST_IP];
        if (router) {
          console.debug("IN: " + entry.DST_IP + " (" + entry.DST_MAC + ") -> " + entry.PACKETS + " packets  " + entry.BYTES + " bytes");          
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
    return routers;
  };

  network.routes  = function() { return this.backbone().concat(this.mesh()); };
  network.routers = function() { return this.backbone.routers().concat(this.mesh.routers()); };

  network.backbone = function() { return this.backbone.routes(); }
  network.backbone.routes  = function() { return [ ] };
  network.backbone.routers = function() { return [ { address : afrimesh.settings.internet_gateway.address, routes : [] } ] };

  network.mesh = function() { return this.mesh.routes(); }; 
  network.mesh.routes  = function() { return this.routes.sync();  };
  network.mesh.routers = function() { return this.routers.sync(); };

  network.mesh.routes.sync  = function() { 
    var routes  = [];
    try {
      routes = afrimesh.villagebus.mesh_topology();
    } catch (error) {
      console.debug("Vis server unreachable due to unknown reason. " + error);
    }
    return routes;
  }

  network.mesh.routers.sync = function() {
    var routers  = [];
    var included = {};
    try {
      afrimesh.villagebus.mesh_topology().map(function(route) {
        if (!included[route.router]) { 
          router = { address : route.router,
                     routes  : [ route ]     };
          routers.push(router);
          included[router.address] = router;
        } else {
          included[router.address].routes.push(route);
        }
      });
    } catch (error) {
      console.debug("Vis server unreachable due to unknown reason. " + error);
    }
    return routers;
  };

  network.mesh.routers.async = function(f) {
    function on_complete(routes) {
      var routers = [];
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
      f(routers);
    };
    afrimesh.villagebus.mesh_topology.async(on_complete);
  };
  
  return network;
};
console.debug("loaded afrimesh.network.js");


