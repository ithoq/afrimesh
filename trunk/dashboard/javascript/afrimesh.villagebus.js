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
  villagebus.ajax_proxy = function() { 
    return "http://" + afrimesh.settings.address + afrimesh.settings.ajax_proxy; 
  };


  /** - villagebus.mesh_topology ------------------------------------------------- */
  villagebus.mesh_topology     = function() { return this.batman.vis();};
  villagebus.mesh_topology.vis = function() { 
    return this.vis.sync(); 
  }; 

  villagebus.mesh_topology.vis.url  = function() { 
    if (afrimesh.settings.network.mesh.vis_server == afrimesh.settings.address) {
      return "http://" + afrimesh.settings.address + ":2005"; 
    }
    return villagebus.ajax_proxy() + "http://" + afrimesh.settings.network.mesh.vis_server + ":2005"; 
    //return "http://" + afrimesh.settings.hosts.batman_vis_server + ":2005?callback=foo"; 
  };
  
  villagebus.mesh_topology.vis.async = function(handler) { 
    var xml = make_json_request({
        url     : this.url(),
        request : {},
        success : handler,
        async   : true });
    return xml;
  };
  villagebus.mesh_topology.vis.poll = function(f, frequency) {   
    this.async(f);
    setTimeout(function() { afrimesh.villagebus.mesh_topology.vis.poll(f, frequency); }, 
               frequency);
  };
  villagebus.mesh_topology.vis.sync = function() { 
    var handler  = function(data) { 
      handler.response = data;  
    };
    return make_json_request({
        url     : this.url(),
        request : {},
        success : handler,
        async   : false });
  };


  /** - villagebus.pmacct bytes data ------------------------------------------------- */
  villagebus.pmacct = function(direction) { 
    return this.pmacct.sync(direction); 
  }; 

  villagebus.pmacct.url  = function() { 
   if (afrimesh.settings.internet_gateway.address == afrimesh.settings.address) {	
      return "http://" + afrimesh.settings.address+ "/cgi-bin/village-bus-pmacct.cgi"; 
    }
    return villagebus.ajax_proxy() + "http://" + afrimesh.settings.internet_gateway.address + "/cgi-bin/village-bus-pmacct.cgi"; 
  };
  
  villagebus.pmacct.async = function(handler, direction) { 
    var xml = make_json_request({
        url     : this.url() + "?direction=" + direction,
        request : {},
        success : handler,
        async   : true });
    return xml;
  };
  villagebus.pmacct.poll = function(f, frequency) {   
    this.async(f);
    setTimeout(function() { afrimesh.villagebus.pmacct.poll(f, frequency); }, 
               frequency);
  };
  villagebus.pmacct.sync = function(direction) { 
    var handler  = function(data) { 
      handler.response = data;  
    };
    return make_json_request({
        url     : this.url() + "?direction=" + direction,
        request : {},
        success : handler,
        async   : false });
  };

  /** - villagebus.radius ------------------------------------------------- */
  villagebus.radius        = function() { return villagebus.radius.who(); };
  villagebus.radius.url    = function() {
    if (afrimesh.settings.radius.server == afrimesh.settings.address) {
      return "http://" + afrimesh.settings.radius.server + "/cgi-bin/village-bus-radius";
    }
    return villagebus.ajax_proxy() + "http://" + afrimesh.settings.radius.server + "/cgi-bin/village-bus-radius";
  };
  villagebus.radius.who    = function() { return villagebus.radius.who.sync(); };
  villagebus.radius.select = function() { return villagebus.radius.select.sync(); };
  villagebus.radius.insert = function(username, type, seconds) { return villagebus.radius.insert.sync(username, type, seconds); };
  villagebus.radius.remove = function(username) { return villagebus.radius.remove.sync(username); };
  villagebus.radius.update = function(username, new_username, new_password, new_type) { return villagebus.radius.update.sync(username, new_username, new_password, new_type); };

  villagebus.radius.who.sync = function() {
    var handler = function(data) { handler.response = data; }; // TODO - extend sync handler to handle array data
    return make_json_request({
        url     : villagebus.radius.url(),
        request : { package  : "radius",
                    command  : "who" },
        success : handler,
        async   : false });
  };
  villagebus.radius.select.sync = function() {
    var handler = function(data) { handler.response = data; }; // TODO - extend sync handler to handle array data
    return make_json_request({
        url     : villagebus.radius.url(),
        request : { package : "radius",
                    command : "list" },
        success : handler,
        async   : false });
  };
  villagebus.radius.insert.sync = function(username, type, seconds) {
    return make_json_request({
        url     : villagebus.radius.url(),
        request : { package  : "radius",
                    command  : "new",
                    username : username,
                    type     : type,
                    seconds  : seconds },  
        success : make_sync_response_handler(villagebus.radius.url(), "villagebus.radius.insert"),
        async   : false });
  };
  villagebus.radius.update.sync = function(username, new_username, new_password, new_type) {
    return make_json_request({
        url     : villagebus.radius.url(),
        request : { package      : "radius",
                    command      : "modify",
                    username     : username,
                    new_username : new_username,
                    new_password : new_password,
                    new_type     : new_type },
        success : make_sync_response_handler(villagebus.radius.url(), "villagebus.radius.update"),
        async   : false });
  };
  villagebus.radius.remove.sync = function(username) {
    return make_json_request({
        url     : villagebus.radius.url(),
        request : { package  : "radius",
                    command  : "delete",
                    username : username },
        success : make_sync_response_handler(villagebus.radius.url(), "villagebus.radius.remove"),
        async   : false });
  };


  /** - villagebus.snmp --------------------------------------------------- */
  // snmpwalk -v 2c -c public 196.211.3.106 SysDescr
  villagebus.snmp = function(address, community, oids) {
    return villagebus.snmp.get(address, community, oids);
  };

  villagebus.snmp.get = function(address, community, oids) {
    return villagebus.snmp.sync("get", address, community, oids);
  };

  villagebus.snmp.walk = function(address, community, oids) {
    return villagebus.snmp.sync("walk", address, community, oids);
  };

  villagebus.snmp.url = function() {
    return "http://" + afrimesh.settings.hosts.dashboard_server + "/cgi-bin/village-bus-snmp";
  };

  villagebus.snmp.async = function(f, address, community, oids) {
    // TODO
  };

  villagebus.snmp.poll = function(f, frequency, address, community, oids) {
    // TODO
  };

  villagebus.snmp.sync = function(command, address, community, oids) {
    return make_json_request({
        url     : this.url(),
        request : { package   : "snmp",
                    command   : command,
                    address   : address,
                    community : community, 
                    oids      : oids        },
        success : make_sync_response_handler(address, "villagebus.snmp"),
        async   : false });
  };



  /** - villagebus.uci ---------------------------------------------------- */
  // UDE - sometimes there are problems with permissions on /etc/config & /tmp/.uci 
  villagebus.uci = function(address) {
    return villagebus.uci.get.sync(address, "");
  };

  // UDE - when dashboard_host != mesh_gateway then we need a way to be able to proxy
  //       through the mesh gateway onto the mesh
  // INJ - TODO - modify ajax_proxy.cgi to be able to chain ajax proxy calls ?
  // INJ.alt - the way open mesh deals with this is to have the mesh nodes pull requests to the
  //           dashboard rather than the dashboard pushing to the nodes. Hrmm. Must ponder.
  //
  // e.g.  return afrimesh.villagebus.ajax_proxy()  + "http://" + afrimesh.settings.hosts.mesh_gateway + 
  //              afrimesh.settings.ajax_proxy      + "http://" + address + "/cgi-bin/village-bus-uci";
  villagebus.uci.url = function(address) { 
    if (address == afrimesh.settings.address) {
      return "http://" + address + "/cgi-bin/village-bus-uci";
    }
    return afrimesh.villagebus.ajax_proxy() + "http://" + address + "/cgi-bin/village-bus-uci";
  };

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
    return make_json_request({ 
        url     : villagebus.uci.url(address), 
        request : { package: "uci", command: "set", arguments: entries }, 
        success : make_sync_response_handler(address, "villagebus.uci"),
        async   : false });
  };

  villagebus.uci.set.async = function(f, address, entries) {
    return make_json_request({ 
        url     : villagebus.uci.url(address), 
        request : { package: "uci", command: "set", arguments: entries }, 
        success : make_async_response_handler(f, address, "villagebus.uci"),
        async   : true });
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

  function make_jsonp_request(request) {
    console.log("Async: " + request.async);
    console.log("URL: " + request.url);
    var xml = $.ajax({
        url         : request.url,
        type        : "GET",
        contentType : "application/json",
        dataType    : "jsonp",
        //jsonp       : "callback",
        async       : request.async,
        data        : "", //{ json : $.toJSON(request.request) },
        success     : request.success });
    console.log("Made request");
    if (request.async) {
      return xml;
    }
    return request.success.response;
  };

  function make_sync_response_handler(address, name) {
    var handler = function(data) {
      if (data.length == 0) {
        console.error(name + " failed to get data from address: " + address);
        console.error(data);
        return;
      }
      handler.response = data[0]; 
      if (handler.response.error) {
        console.error(name + " failed with error: " + data[0].error);
      }
    };
    return handler;
  };

  function make_async_response_handler(f, address, name) {
    var handler = function(data) {
      if (data.length == 0) {
        console.error(name + " failed to get data from address: " + address);
        console.error(data);
        return;
      } 
      handler.response = data[0]; 
      if (handler.response.error) {
        console.error(name + " failed with error: " + data[0].error);
      }
      f(data[0]);
    };
    return handler;
  };

  

  return villagebus;
};
console.debug("loaded afrimesh.villagebus.js");


