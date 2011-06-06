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

  // Network roots are nodes with some level of savvy about the network. 
  // Generally, if they can't process a message they should be able to tell you who can
  // TODO - check if 'address' is a valid root, fallback to other roots otherwise
  var roots = [ address, "10.130.1.1" ];
  var default_root = "6.6.6.6"; //roots[0];

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
  var afrimesh2uci = { 
    "afrimesh|settings|root"         : { remote : "afrimesh|settings|root",          init : default_root },
    "afrimesh|settings|edge"         : { remote : "afrimesh|settings|edge",          init : undefined },

    "afrimesh|settings|network_name" : { remote : "afrimesh|dashboard|network_name", init : "mesh testbed" },
    "afrimesh|settings|locale"       : { remote : "afrimesh|settings|locale",        init : "en_US.UTF-8"  },
    //"afrimesh|settings|ajax_proxy"   : { remote : "afrimesh|settings|ajax_proxy",    init : "/cgi-bin/ajax-proxy.cgi?url=" }
    "afrimesh|settings|ajax_proxy"   : { remote : "afrimesh|settings|ajax_proxy",    init : "/cgi-bin/villagebus.lua" },
    "afrimesh|settings|a2billing"    : { remote : "afrimesh|settings|a2billing",     init : "10.130.1.1"  },
    "afrimesh|settings|redis_server" : { remote : "afrimesh|settings|redis_server",  init : address  },

    "afrimesh|settings|support|howto" : { remote : "afrimesh|support|howto", init : "afrimesh"  },
    "afrimesh|settings|support|group" : { remote : "afrimesh|support|group", init : "afrimesh"  },
    "afrimesh|settings|support|irc"   : { remote : "afrimesh|support|irc",   init : "http://webchat.freenode.net/?channels=afrimesh"  },

    "afrimesh|settings|voip|iax|enable"  : { remote : "afrimesh|voip|iax",  init : "off" },
    "afrimesh|settings|voip|iax|address" : { remote : "asterisk|@sip[0]|host",   init : ""       },

    "afrimesh|settings|voip|sip|enable"   : { remote : "afrimesh|voip|sip",    init : "off" },
    "afrimesh|settings|voip|sip|address"  : { remote : "asterisk|@sip[0]|host",     init : ""    },
    "afrimesh|settings|voip|sip|username" : { remote : "asterisk|@sip[0]|username", init : ""    },
    "afrimesh|settings|voip|sip|password" : { remote : "asterisk|@sip[0]|secret",   init : ""    },

    "afrimesh|settings|network|wireless|address"  : { remote : "network|(wireless|@wifi-iface[0]|device)|ipaddr",  init : "?" },
    "afrimesh|settings|network|wireless|netmask"  : { remote : "network|(wireless|@wifi-iface[0]|device)|netmask", init : "?" },
    "afrimesh|settings|network|wireless|channel"  : { remote : "wireless|(wireless|@wifi-iface[0]|device)|channel",        init : "?" },
    "afrimesh|settings|network|wireless|ssid"     : { remote : "wireless|@wifi-iface[0]|ssid",  init : "?" },
    "afrimesh|settings|network|wireless|bssid"    : { remote : "wireless|@wifi-iface[0]|bssid", init : "??:??:??:??:??:??" },

    "afrimesh|settings|network|mesh|accounting_server" : { remote : "afrimesh|settings|accounting_server", init : "10.130.1.1" },  
    "afrimesh|settings|network|mesh|vis_server"        : { remote : "batmand|general|visualisation_srv", init : "10.130.1.1" },  
    "afrimesh|settings|network|mesh|routing_class"     : { remote : "batmand|general|routing_class",     init : "" },  
    "afrimesh|settings|network|mesh|gateway_class"     : { remote : "batmand|general|gateway_class",     init : "" },  

    //"afrimesh|settings|location|longitude" : {  remote : "afrimesh|location|longitude", init : "18.339733" },
    //"afrimesh|settings|location|latitude"  : {  remote : "afrimesh|location|latitude",  init : "-34.138061" },
    "afrimesh|settings|location|longitude" : {  remote : "afrimesh|location|longitude", init : undefined },
    "afrimesh|settings|location|latitude"  : {  remote : "afrimesh|location|latitude",  init : undefined },

    "afrimesh|settings|map|server" : {  remote : "afrimesh|map|server", init : "openstreetmap.org" },
    "afrimesh|settings|map|extent" : {  remote : "afrimesh|map|extent", init : "0.025" },
    "afrimesh|settings|map|zoom"   : {  remote : "afrimesh|map|zoom",   init : "16" },
    "afrimesh|settings|map|google" : {  remote : "afrimesh|map|google", init : "false" },

    "afrimesh|settings|internet_gateway|address"        : {  remote : "afrimesh|gateway|address",        init : "" },
    "afrimesh|settings|internet_gateway|snmp|community" : {  remote : "afrimesh|gateway|snmp_community", init : "public" },
    "afrimesh|settings|internet_gateway|snmp|interface" : {  remote : "afrimesh|gateway|snmp_interface", init : "4" },
    "afrimesh|settings|internet_gateway|bandwidth|down" : {  remote : "afrimesh|gateway|bandwidth_down", init : "512" },
    "afrimesh|settings|internet_gateway|bandwidth|up"   : {  remote : "afrimesh|gateway|bandwidth_up",   init : "256" },
    
    "afrimesh|settings|radius|server"   : {  remote : "afrimesh|radius|server",   init : "10.130.1.1" },
    "afrimesh|settings|radius|radtype"  : {  remote : "afrimesh|radius|radtype",  init : "1" },
    "afrimesh|settings|radius|database" : {  remote : "afrimesh|radius|database", init : "radius" },
    "afrimesh|settings|radius|username" : {  remote : "afrimesh|radius|username", init : "radius" },
    "afrimesh|settings|radius|password" : {  remote : "afrimesh|radius|password", init : "radpass" }
  };

  /** - create dynamic settings object & apply parent.settings ------------ */
  // TODO - get rid of parent.settings and apply defaults from above afrimesh2uci object
  var settings = function() {
  };
  settings.address = address;


  /** - functions on selectors -------------------------------------------- */
  settings.save = function(selector, value) {
    //console.debug("Saving: " + selector + " = " + value);
    var remote = Qsplit(parent.settings.uci_config, afrimesh2uci[selector].remote);
    //console.debug("Remote: " + afrimesh2uci[selector].remote + " = " + remote);
    if (remote.length != 3) { 
      console.error("afrimesh.settings: '" + selector + "' has invalid remote selector '" + afrimesh2uci[selector].remote + "'");
      return false;
    }
    Qset(parent, selector, value);                         // apply to local settings object

    afrimesh.device.configuration.set(address, [{ 
      config  : remote[0],
      section : remote[1],
      option  : remote[2],
      value   : value }], function(error, response) {
        if (error) { return console.error("Could not save configuration: " + error); }
        console.debug("PUT /" + address + "/config/" + afrimesh2uci[selector].remote + " " + value + " -> " + show(response));
      });

    return true;
  };


  /** - apply persistent settings ------------------------------------------ */
  var load_remote = function() {   
    // This is the initial remote call which bootstraps the environment.
    // Until it is complete we don't have enough information about the mesh
    // to guarantee the availability of the API.
    //
    // We're going to access VillageBus directly to be able to perform
    // this call synchronously so that we can block further initialization
    // until it has completed.
    var name = parent.villagebus.Name("/@self/config/*");
    name = parent.villagebus.GET(name);
    var config = parent.villagebus.Read(name)[0];
    //console.debug("REMOTE CONFIG: \n" + rshow(config));    
    parent.settings.uci_config = config;
    for (var local in afrimesh2uci) {
      var value = "";
      try { 
        value = (afrimesh2uci[local] ? Q(config, afrimesh2uci[local].remote, "config") : ""); 
      } catch (e) { 
        console.error("Failed to load configuration item: " + e);
      }
      if (value) {
        Qset(parent, local, value);
      } else {
        Qset(parent, local, afrimesh2uci[local].init);
        //console.debug ("Missing configuration entry '" + local + "'; reverting to default: " + afrimesh2uci[local].init);
      }
    }
    for (var setting in parent.settings) {
      settings[setting] = parent.settings[setting];
    }
  }; load_remote();
  

  /* TODO - cleanup this evilness to deal w/ hosting OpenLayers elsewhere in order to save space */
  settings.map.openlayers_url = "http://" + settings.map.server + "/openlayers/";
  if (settings.map.server == "openstreetmap.org") { 
    /* TODO very very evil hack - we assume that if
       openstreetmap.org is specced that we have access to
       openlayers.org as a workaround for the fact that the
       OpenLayers.js on openstreetmap has serious issues */
    settings.map.openlayers_url = "http://www.openlayers.org/api/OpenLayers.js";
    /* TODO scratch that - openlayers wants to install google gears - so we're going
       to assume that if you have access to openstreetmap.org then you have access to 
       Google */
    //settings.map.openlayers_url = "http://afrimesh.googlecode.com/svn/trunk/dashboard/javascript/openlayers/OpenLayers.js";
  }
  //settings.map.openlayers_url = "http://192.168.20.2/afrimesh/javascript/openlayers/OpenLayers.js";
  //settings.map.openlayers_url = "http://192.168.1.20/afrimesh/javascript/openlayers/OpenLayers.js";
  settings.map.openlayers_url = "javascript/openlayers/OpenLayers.js"; // <- TODO - add some config option to host this locally
  settings.map.osm_url = "javascript/openlayers/OpenStreetMap.js";

  return settings;
};
exports.BootSettings = BootSettings;
console.debug("loaded afrimesh.settings.js");