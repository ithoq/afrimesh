/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/** - afrimesh node bootstrap --------------------------------------------- */
var afrimesh = undefined;
(function() {
  function Afrimesh() {
    this.settings = { 
      network_name : "development testbed",
      locale       : "en_US.UTF-8",
      hosts        : {
        dashboard_server  : "bdashboard.7degrees.co.za",
        batman_vis_server : "dashboard.7degrees.co.za",
        internet_gateway  : "192.168.20.1",
        mesh_gateway      : "192.168.20.200"
      },
      community : "public"
    };
    this.network    = BootNetwork(this);
    this.customers  = undefined;
    this.villagebus = BootVillageBus(this);
  };
  afrimesh = new Afrimesh();
})();
console.debug("loaded afrimesh.js - completed afrimesh node bootstrap");

// check namespace pollution
// dump_object(this);
