/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */
   
   
/** 
 * afrimesh settings bootstrap
 *
 * @arg parent   - afrimesh root
 * @arg address  - override default settings object with the config settings from the network node at this address
 */
function BootSettings(parent, address) {


  /**
   * Map our afrimesh.settings object to UCI for persistent storage
   *
   * TODO - do a formal abstraction of this whole mapping mechanism.
   * TODO - include a way to specify type parsers as we're storing everything as strings on the backend 
   *        e.g. see construction in network.map.html
   *
   * Given a settings object:
   *  . Populate it with the defaults from our map
   * Given a node:
   *  . get position in object hierarchy
   *  . get the value of the node
   *  . set the value of the node
   * Given a position in object hierarcy
   *  . get the value of the node at that position
   *  . set the value of the node at that position
   */
  var map = { 
    "afrimesh|settings|network_name" : { remote : "afrimesh|dashboard|network_name", init : "mesh testbed", explain : "the name of the network" },
    "afrimesh|settings|locale"       : { remote : "afrimesh|settings|locale",        init : "en_US.UTF-8"  },
    "afrimesh|settings|ajax_proxy"   : { remote : "afrimesh|settings|ajax_proxy",    init : "/cgi-bin/ajax-proxy.cgi?url=" },

    "afrimesh|settings|hosts|dashboard_server"  : { remote : "afrimesh|dashboard|dashboard_server",  init : "afrimesh.7degrees.co.za"  },
    "afrimesh|settings|hosts|batman_vis_server" : { remote : "afrimesh|dashboard|visualisation_srv", init : "dashboard.7degrees.co.za" },

    "afrimesh|settings|location|longitude" : {  remote : "afrimesh|location|longitude", init : "18.339733" },
    "afrimesh|settings|location|latitude"  : {  remote : "afrimesh|location|latitude",  init : "-34.138061" },

    "afrimesh|settings|map|server" : {  remote : "afrimesh|map|server", init : "openstreetmaps.org" },
    "afrimesh|settings|map|extent" : {  remote : "afrimesh|map|extent", init : "0.025" },
    "afrimesh|settings|map|zoom"   : {  remote : "afrimesh|map|zoom",   init : "16" },
    "afrimesh|settings|map|aerial" : {  remote : "afrimesh|map|aerial", init : "false" },

    "afrimesh|settings|internet_gateway|address"        : {  remote : "afrimesh|gateway|address",        init : "192.168.20.1" },
    "afrimesh|settings|internet_gateway|snmp|community" : {  remote : "afrimesh|gateway|snmp_community", init : "public" },
    "afrimesh|settings|internet_gateway|snmp|interface" : {  remote : "afrimesh|gateway|snmp_interface", init : "4" },
    "afrimesh|settings|internet_gateway|bandwidth|down" : {  remote : "afrimesh|gateway|bandwidth_down", init : "512" },
    "afrimesh|settings|internet_gateway|bandwidth|up"   : {  remote : "afrimesh|gateway|bandiwdth_up",   init : "256" }
  };


  /** - create dynamic settings object & apply parent.settings ------------ */
  // TODO - get rid of parent.settings and apply defaults from above map object
  var settings = function() {
  };
  settings.address = address;



  /** - utility functions ------------------------------------------------- */
  settings.load = function(local, remote, key) {
    if (local && remote && remote[key]) {
      local[key] = remote[key];
    } else {
      console.warn("afrimesh.settings.load: " + address + " has no value for '" + key + "'");
    }
    return 0;
  };

  settings.save = function(selector, value) {
    var remote = Qsplit(map[selector].remote);
    if (remote.length != 3 || remote[0] != "afrimesh") {
      console.error("afrimesh.settings: '" + selector + "' has invalid remote selector '" + map[selector].remote + "'");
      return;
    }
    Qset(parent, selector, value);                         // apply to local settings object
    parent.villagebus.uci.set.async(function (response) {  // apply to remote UCI
        console.debug("afrimesh.settings.save(uci://" + settings.address + "/" + map[selector].remote + ", " + value + ") -> " + response);
      },
      settings.address, 
      [ { config: "afrimesh", // hardcode it for security until we have better solutions
          section : remote[1], 
          option  : remote[2], 
          value   : value } ]);
  };



  /** - apply persistent settings ------------------------------------------ */
  var load_remote = function() {
    var config = parent.villagebus.uci.get.sync(address, "afrimesh");
    //console.debug("REMOTE CONFIG: " + rpretty_print(config));
    for (var local in map) {
      var value = Q(config, map[local].remote, "config");
      if (value) {
        Qset(parent, local, value);
      }
    }

    for (var setting in parent.settings) {
      settings[setting] = parent.settings[setting];
    }
    
    /*
      parent.villagebus.uci.get.async(function (config) {
        console.debug("CONFIG:\n" + rpretty_print(config));
        },
        address, "afrimesh");
    */
  }
  load_remote();



  return settings;
};
