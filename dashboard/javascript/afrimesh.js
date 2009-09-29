/*
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
    console.log("Location: " + window.location.hostname);
    this.settings = { 
      address      : window.location.hostname ? window.location.hostname : "196.211.3.106",
      //address      : window.location.hostname ? window.location.hostname : "192.168.1.20",
      ajax_proxy   : "/cgi-bin/ajax-proxy.cgi?url="
    };

    this.storage    = BootStorage(this);
    this.network    = BootNetwork(this);
    this.customers  = BootCustomers(this);
    this.villagebus = BootVillageBus(this);

    // override default settings with live settings from the server
    console.debug("Booting from: " + window.location.hostname ? window.location.hostname : "local-" + this.settings.address);
    this.settings = BootSettings(this, this.settings.address); 

  };
  afrimesh = new Afrimesh();
})();
console.debug("loaded afrimesh.js - completed afrimesh node bootstrap");

// check namespace pollution
// dump_object(this);


