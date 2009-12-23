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

  /** - villagebus.login --------------------------------------------------------- */
  // TODO - support multiple authentication mechanisms e.g.  luci, htaccess, cert, ldap etc.
  villagebus.login = function(username, password, continuation, error) {
    return this.login.async(username, password, continuation, error);
  };
  villagebus.login.url = function() {
    return "http://" + afrimesh.settings.address + "/cgi-bin/luci/rpc/auth";
  };
  villagebus.login.async = function(username, password, continuation, error) {
    // LuCI RPC doesn't give us a session token so RPC login is f.useless 
    // for accessing the web interface  *sigh* 
    return rpc_async(this.url(), "login", [ username, password ], function(session) {
        //Set-Cookie: sysauth=7F6D11773AF2D11DFC308242C4E71A32; path=/cgi-bin/luci/;stok=15C3499EA0FDC5D342288A07A90CF643
        //console.error("GOTTA COOKIE: " + document.cookie);
        //console.error("sysauth: " + afrimesh.storage.cookie("sysauth"));
        //console.error("stok: " + afrimesh.storage.cookie("stok"));
        //console.error("path: " + afrimesh.storage.cookie("path"));
        continuation(session);
      }, error); 
  };
  
  /** - villagebus.mesh_topology ------------------------------------------------- */
  villagebus.mesh_topology       = function()  { return this.mesh_topology.vis();};
  villagebus.mesh_topology.vis   = function()  { 
    return this.vis.sync(); 
  }; 
  villagebus.mesh_topology.async = function(f) {
    return this.vis.async(f);
  };

  villagebus.mesh_topology.vis.url  = function() { 
    if (afrimesh.settings.network.mesh.vis_server == afrimesh.settings.address) {
      return "http://" + afrimesh.settings.address + ":2005"; 
    }
    return villagebus.ajax_proxy() + "http://" + afrimesh.settings.network.mesh.vis_server + ":2005"; 
    //return "http://" + afrimesh.settings.network.mesh.vis_server + ":2005?callback=foo"; 
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
    var address = "";
    if (typeof(afrimesh.settings.internet_gateway.address) != "undefined" && 
        afrimesh.settings.internet_gateway.address != "") {
      address = afrimesh.settings.internet_gateway.address;
    } else if (typeof(afrimesh.settings.network.mesh.vis_server) != "undefined" &&
               afrimesh.settings.network.mesh.vis_server != "") {
      address = afrimesh.settings.network.mesh.vis_server;
    } else {
      console.error("Could not determine address for pmacct server");
      address = afrimesh.settings.address;
    }
   if (address == afrimesh.settings.address) {	
      return "http://" + afrimesh.settings.address + "/cgi-bin/village-bus-pmacct.cgi"; 
    }
    return villagebus.ajax_proxy() + "http://" + address + "/cgi-bin/village-bus-pmacct.cgi"; 
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
    console.debug("PMACCT USES: " + this.url());
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

  villagebus.snmp.get.async = function(f, address, community, oids) {
    return villagebus.snmp.async(f, "get", address, community, oids);
  };

  villagebus.snmp.walk = function(address, community, oid) {
    return villagebus.snmp.sync("walk", address, community, oid);
  };

  // will always use the machine being viewed for the snmp service
  // if snmp is not supported on that machine, use another one!
  villagebus.snmp.url = function() {
    return "http://" + afrimesh.settings.address + "/cgi-bin/village-bus/snmp";
  };

  villagebus.snmp.poll = function(f, frequency, address, community, oids) {
    // TODO
  };

  villagebus.snmp.sync = function(command, address, community, oids) {
    return rpc(this.url(), command, [address, community, oids]);
  };

  villagebus.snmp.async = function(f, command, address, community, oids) {
    return rpc_async(this.url(), command, [address, community, oids], f); 
  };


  /** - villagebus.sys ---------------------------------------------------- */
  villagebus.sys = function(address) {
    return { uname : "", syslog : "", version : "" };
  };
  villagebus.sys.url = function(address) { 
    if (address == afrimesh.settings.address) {
      return "http://" + address + "/cgi-bin/village-bus/sys";
    }
    return afrimesh.villagebus.ajax_proxy() + "http://" + address + "/cgi-bin/village-bus/sys";
  };
  villagebus.sys.uname = function(address) { return villagebus.sys.uname.sync(address); };
  villagebus.sys.uname.sync  = function(address)    { return rpc(villagebus.sys.url(address), "uname", []); };
  villagebus.sys.uname.async = function(f, address) { return rpc_async(villagebus.sys.url(address), "uname", [], f); };  
  villagebus.sys.version = function(address) { return villagebus.sys.version.sync(address); };
  villagebus.sys.version.sync  = function(address)    { return rpc(villagebus.sys.url(address), "version", []); };
  villagebus.sys.version.async = function(f, address) { return rpc_async(villagebus.sys.url(address), "version", [], f); };  
  villagebus.sys.service = function(address, service, command)       { return villagebus.sys.service.sync(address, service, command); };
  villagebus.sys.service.sync  = function(address, name, command)    { return rpc(villagebus.sys.url(address), "service", [ name, command ]); };
  villagebus.sys.service.async = function(f, address, name, command) { return rpc_async(villagebus.sys.url(address), "service", [ name, command ], f); };  

  /** - villagebus.ipkg --------------------------------------------------- */
  villagebus.ipkg = function(address) {
    return { update : "", list : "", status : "", upgrade : "" };
  };
  villagebus.ipkg.url = function(address) { 
    if (address == afrimesh.settings.address) {
      return "http://" + address + "/cgi-bin/village-bus/ipkg";
    }
    return afrimesh.villagebus.ajax_proxy() + "http://" + address + "/cgi-bin/village-bus/ipkg";
  };
  villagebus.ipkg.update       = function(address) { return villagebus.ipkg.update.sync(address); };
  villagebus.ipkg.update.sync  = function(address) { return rpc(villagebus.ipkg.url(address), "update", []); };
  villagebus.ipkg.update.async = function(f, address) { return rpc_async(villagebus.ipkg.url(address), "update", [], f); };

  villagebus.ipkg.list       = function(address) { return villagebus.ipkg.list.sync(address); };
  villagebus.ipkg.list.sync  = function(address) { return rpc(villagebus.ipkg.url(address), "list", []); };
  villagebus.ipkg.list.async = function(f, address) { return rpc_async(villagebus.ipkg.url(address), "list", [], f); };

  villagebus.ipkg.status       = function(address) { return villagebus.ipkg.status.sync(address); };
  villagebus.ipkg.status.sync  = function(address) { return rpc(villagebus.ipkg.url(address), "status", []); };
  villagebus.ipkg.status.async = function(f, address) { return rpc_async(villagebus.ipkg.url(address), "status", [], f); };

  villagebus.ipkg.upgrade       = function(address, pkgname) { return villagebus.ipkg.upgrade.sync(address, pkgname); };
  villagebus.ipkg.upgrade.sync  = function(address, pkgname) { return rpc(villagebus.ipkg.url(address), "upgrade", [pkgname]); };
  villagebus.ipkg.upgrade.async = function(f, address, pkgname) { return rpc_async(villagebus.ipkg.url(address), "upgrade", [pkgname], f); };


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
      return "http://" + address + "/cgi-bin/village-bus/uci";
    }
    return afrimesh.villagebus.ajax_proxy() + "http://" + address + "/cgi-bin/village-bus/uci";
  };

  villagebus.uci.get = function(address, selector) { return villagebus.uci.get.sync(address, selector);  };
  villagebus.uci.set = function(address, entries)  { return villagebus.uci.set.sync(address, entries);   };

  villagebus.uci.get.sync = function(address, selector) { return rpc(villagebus.uci.url(address), "show", [selector]);  };
  villagebus.uci.get.async = function(f, address, selector) { return rpc_async(villagebus.uci.url(address), "show", [selector], f);  };

  villagebus.uci.set.sync = function(address, entries) { return rpc(villagebus.uci.url(address), "set", [entries]);  };
  villagebus.uci.set.async = function(f, address, entries) { return rpc_async(villagebus.uci.url(address), "set", [entries], f);  };


  /** - villagebus.syslog -------------------------------------------------------- */
  villagebus.syslog = function() { return this.syslog.sync(); }

  // will always show the log on the machine being viewed. If you want all nodes in the log, view
  // the remote log server
  villagebus.syslog.url = function() {
    return "http://" + afrimesh.settings.address + "/cgi-bin/village-bus/sys";
  };

  villagebus.syslog.sync = function(count) {
    if (count == undefined) { count = 10; }
    return rpc(villagebus.syslog.url(), "syslog", [count]);
  };

  villagebus.syslog.async = function(f, count) {
    if (count == undefined) { count = 10; }
    return rpc_async(villagebus.syslog.url(), "syslog", [count], f);
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

  // TODO - don't require parameters to be in an array - rather use varargs!
  var rpc = function(url, method, parameters) {
    // TODO - check host & path
    var request = {
      url         : url, //"http://" + rpc.host + rpc.path, 
      type        : "POST",
      contentType : "application/json",
      dataType    : "json",
      async       : false
    };
    request.data = JSON.stringify({ 
        id      : 1234,
        version : "2.0",
        method  : method,
        params  : parameters
      });
    request.success = function(response, result) {
      if (!response) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - empty response");
      } else if (response.error) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - " + response.error);
      } else if (response.result) {
        request.result = response.result;
      } else {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - unknown");
      }
    };
    var xml = $.ajax(request);
    return request.result;
  }; 

  function rpc_async(url, method, parameters, continuation, error) {
    var request = {
      url          :  url, //"http://" + rpc.host + rpc.path, 
      type         : "POST",
      contentType  : "application/json",
      dataType     : "json",
      async        : true,
      continuation : continuation,
    };
    request.data = JSON.stringify({ 
        id      : 1234,
        version : "2.0",
        method  : method,
        params  : parameters
      });
    request.error   = (typeof(error) == "undefined" ? rpc_async_error(request) : error);
    request.success = rpc_async_success(request, request.error);
    //console.debug("making request: " + show(request));
    return $.ajax(request);
  };

  function rpc_async_error(request) {
    return function(xhr, status, error) {
      console.error("FATAL JSON/RPC ERROR: " + JSON.stringify(request) + " - xhr: " + xhr + " status: " + status + " error: " + error);
    };
  };

  function rpc_async_success(request, error) {
    return function(response, result) {
      if (!response) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - empty response");
        error("empty response");
      } else if (response.error) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - " + response.error);
        error(response.error);
      } else if (response.result) {
        request.continuation(response.result);
      } else {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - unknown error: " + JSON.stringify(response));
        error("unknown error");
      }
    };
  };
  

  return villagebus;
};
console.debug("loaded afrimesh.villagebus.js");


