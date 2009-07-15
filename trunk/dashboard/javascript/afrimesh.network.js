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

  network.accounting = function(router) {
    var temp = 0;
    var direction = "out";
    try { 
	afrimesh.villagebus.pmacct(direction).map(function(host) {		
	 if (router.address == host.SRC_IP) {
	   if (typeof router.macaddr == "undefined") {	
		router.macaddr = host.SRC_MAC;
		temp = parseInt(host.BYTES);
	   } else {
	     if (temp<parseInt(host.BYTES)) {
	       temp = parseInt(host.BYTES);
	     }
	   }
         }
     });
    } catch(error) {
      console.debug("pmaccto error " + error);
    }
		router.tranBytes = temp;
		temp = 0;
		var direction = "in";
    try {
	afrimesh.villagebus.pmacct(direction).map(function(host){		
    	if (router.address == host.DST_IP) {
		if (temp<parseInt(host.BYTES)){
	  	  temp = parseInt(host.BYTES);
		  }
	}
     });
    } catch(error) {
       console.debug("pmaccto error " + error);
    }
   router.recBytes = temp; 
   return router;
		
  }

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
      routes = afrimesh.villagebus.batman(); 
    } catch(error) {
      console.debug("Vis server unreachable due to unknown reason. " + error);
    }
    return routes;
  }

  network.mesh.routers.sync = function() {
    var routers  = [];
    var included = {};
    try {
      afrimesh.villagebus.batman().map(function(route) {
        if (!included[route.router]) { 
          router = { address : route.router,
                     routes  : [ route ]     };
          routers.push(router);
          included[router.address] = router;
        } else {
          included[router.address].routes.push(route);
        }
      });
    } catch(error) {
      console.debug("Vis server unreachable due to unknown reason. " + error);
    }
    return routers;
  };
  
  return network;
};
console.debug("loaded afrimesh.network.js");


