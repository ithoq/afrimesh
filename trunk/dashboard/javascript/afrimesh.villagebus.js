/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


var BootVillageBus = function (parent) {

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

  villagebus.batman.vis.url  = "http://" + parent.settings.hosts.batman_vis_server + ":2004";
  villagebus.batman.vis.urlf = function() { return "http://" + afrimesh.settings.hosts.batman_vis_server + ":2004"; };
  
  villagebus.batman.vis.async = function(f) { 
    var xml = XMLHttpRequest();
    xml.open("POST", this.url, true); 
    xml.onreadystatechange = function() {
      if (xml.readyState == 4) {
        f(eval(xml.responseText));
      }
    };
    xml.send("{}");
  };
  villagebus.batman.vis.poll = function(f, frequency) {   
    this.async(f);
    setTimeout(function() { afrimesh.villagebus.batman.vis.poll(f, frequency); }, 
               frequency);
  };
  villagebus.batman.vis.sync = function() { // TODO - error handling + timeout 
    var response = undefined;
    this.async(function(routes) { 
        response = routes;
      });
    while (!response) { sleep(); }
    return response; 
  };
  
  
  /** - villagebus.radius ------------------------------------------------- */
  villagebus.radius = undefined;


  /** - villagebus.snmp --------------------------------------------------- */
  villagebus.snmp = function(address, community, oids) {
    return villagebus.snmp.sync(address, community, oids);
  };

  villagebus.snmp.url = "http://" + parent.settings.hosts.dashboard_server + "/cgi-bin/village-bus-snmp";

  villagebus.snmp.async = function(f, address, community, oids) {
    // TODO
  };

  villagebus.snmp.poll = function(f, frequency, address, community, oids) {
    // TODO
  };

  villagebus.snmp.sync = function(address, community, oids) {
    var request = { package   : "snmp",
                    command   : "get",
                    address   : address,
                    community : community, 
                    oids      : oids };
    var response = {};
    $.ajax({ 
        url : this.url, 
          type        : "POST", 
          contentType : "application/json", 
          dataType    : "json", 
          async       : false,
          data        : $.toJSON(request),
          success     : 
        function(data) { 
          if (data.length != 1) {
            console.error("villagebus.snmp failed to get data from address: " + address);
            return; // UDE we don't yet have a standard strategy for return values for failed calls
          }
          if (data[0].error) {
            console.error("villagebus.snmp failed with error: " + data[0].error);
          }
          response = data[0];
        }
      });
    return response;
  };


  /** - villagebus.uci ---------------------------------------------------- */
  villagebus.uci    = undefined; 

  return villagebus;
};


console.debug("loaded afrimesh.villagebus.js");
