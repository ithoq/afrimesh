

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
  villagebus.snmp   = undefined;


  /** - villagebus.uci ---------------------------------------------------- */
  villagebus.uci    = undefined; 

  return villagebus;
};


console.debug("loaded afrimesh.villagebus.js");
