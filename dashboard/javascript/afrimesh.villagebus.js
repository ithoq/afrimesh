/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/**
 * All interaction with external systems, both local and on the network
 * occur via the VillageBus
 *
 * sync calls returns any result from the request
 * async calls return the XMLHttpRequest object associated with the request
 */
var BootVillageBus = function (afrimesh) {

  var villagebus = function() {
    var ret = [];
    for (var p in this) { if (p != "prototype") ret.push(p); }
    return ret;
  };


  /** - villagebus.batman ------------------------------------------------- */
  villagebus.batman     = function() { return this.batman.vis(); };
  villagebus.batman.vis = function() { 
    return this.vis.sync(); 
  }; 

  villagebus.batman.vis.url  = "http://" + afrimesh.settings.hosts.batman_vis_server + ":2004";
  villagebus.batman.vis.urlf = function() { return "http://" + afrimesh.settings.hosts.batman_vis_server + ":2004"; };
  
  villagebus.batman.vis.async = function(f) { 
    var xml = new XMLHttpRequest();
    xml.open("POST", this.url, true); 
    xml.onreadystatechange = function() {
      if (xml.readyState == 4) {
        f(eval(xml.responseText));
      }
    };
    xml.send("{}");
    return xml;
  };
  villagebus.batman.vis.poll = function(f, frequency) {   
    this.async(f);
    setTimeout(function() { afrimesh.villagebus.batman.vis.poll(f, frequency); }, 
               frequency);
  };
  // UDE - we can't use the sleep() approach because browser javascript is single threaded
  // and gives no access to the event loop
  // INJ - Use a synchronous XMLHttpRequest
  // NB  - Now it no longer works in xpcshell!   
  villagebus.batman.vis.sync = function() { // TODO - error handling + timeout 
    var request  = new XMLHttpRequest();
    var response;
    request.open("POST", this.url, false);
    request.onreadystatechange = function() {
      if (request.readyState == 4) {
        response = eval(request.responseText);
      }
    };
    request.send("{}");
    return response;
  };
  /**villagebus.batman.vis.sync = function() { 
    var response = undefined;
    var xml = this.async(function(routes) { 
        response = routes;
      });
    while (!response) { 
      sleep(); 
    }
    return response; 
  };*/
  
  
  /** - villagebus.radius ------------------------------------------------- */
  villagebus.radius = undefined;


  /** - villagebus.snmp --------------------------------------------------- */
  villagebus.snmp = function(address, community, oids) {
    return villagebus.snmp.sync(address, community, oids);
  };

  villagebus.snmp.url = "http://" + afrimesh.settings.hosts.dashboard_server + "/cgi-bin/village-bus-snmp";

  villagebus.snmp.async = function(f, address, community, oids) {
    // TODO
  };

  villagebus.snmp.poll = function(f, frequency, address, community, oids) {
    // TODO
  };

  villagebus.snmp.sync = function(address, community, oids) {
    return make_json_request({
        url     : this.url,
        request : { package   : "snmp",
                    command   : "get",
                    address   : address,
                    community : community, 
                    oids      : oids        },
        success : make_sync_response_handler(address, "villagebus.snmp"),
        async   : false });
  };



  /** - villagebus.uci ---------------------------------------------------- */
  villagebus.uci = function(address) {
    return villagebus.uci.get.sync(address, "*");
  };

  // UDE - when dashboard_host != mesh_gateway then we need a way to be able to proxy
  //       through the mesh gateway onto the mesh
  // INJ - TODO - modify ajax_proxy.cgi to be able to chain ajax proxy calls ?
  // INJ.alt - the way open mesh deals with this is to have the mesh nodes pull requests to the
  //           dashboard rather than the dashboard pushing to the nodes. Hrmm. Must ponder.
  villagebus.uci.ajax_proxy = "http://" + afrimesh.settings.hosts.mesh_gateway + afrimesh.settings.ajax_proxy;
  villagebus.uci.url = function(address) { return villagebus.uci.ajax_proxy + "http://" + address + "/cgi-bin/village-bus-uci"; }

  villagebus.uci.get = function(address, selector) {
    return villagebus.uci.get.sync(address, selector);
  };
  villagebus.uci.set = function(address, entries) {
    return villagebus.uci.set.sync(address, entries);
  };

  villagebus.uci.get.sync = function(address, selector) {
    return make_json_request({ 
        url     : villagebus.uci.url(address), 
        request : { package: "uci", command: "show"  }, 
        success : make_sync_response_handler(address, "villagebus.uci"), 
        async   : false });
  };

  villagebus.uci.get.async = function(f, address, selector) {
    return make_json_request({ 
        url     : villagebus.uci.url(address), 
        request : { package: "uci", command: "show"  }, 
        success : make_async_response_handler(f, address, "villagebus.uci"),
        async   : true });
  };

  villagebus.uci.set.sync = function(address, entries) {
  };


  /** - helper functions -------------------------------------------------- */
  /**
   * @return XMLHttpRequest
   */
  function make_json_request(request) {
    var xml = $.ajax({
        url: request.url,
          type        : "POST",
          contentType : "application/json",
          dataType    : "json",
          async       : request.async,
          data        : $.toJSON(request.request),
          success     : request.success });
    if (request.async) {
      return xml;
    }
    return request.success.response;
  };

  function make_sync_response_handler(address, name) {
    var handler = function(data) {
      if (data.length != 1) {
        console.error(name + " failed to get data from address: " + address);
        return;
      } 
      if (data[0].error) {
        console.error(name + " failed with error: " + data[0].error);
        return;
      }
      handler.response = data[0];
    };
    return handler;
  };

  function make_async_response_handler(f, address, name) {
    var handler = function(data) {
      if (data.length != 1) {
        console.error(name + " failed to get data from address: " + address);
        return;
      } 
      if (data[0].error) {
        console.error(name + " failed with error: " + data[0].error);
        return;
      }
      f(data[0]);
    };
    return handler;
  };

  

  return villagebus;
};
console.debug("loaded afrimesh.villagebus.js");


