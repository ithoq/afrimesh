/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

/* declare global namespaces */
//if (!afrimesh) { afrimesh = {}; }

/* check for namespace pollution */
//var vars=[]; for(var v in this){vars.push(v);} vars.sort();


/* - configuration -------------------------------------------------------- */
config = { };
//config.router_host = "";
// TODO: s/router_host/this_host
config.router_host = "http://meraka.7degrees.co.za";   /* No slash at end please, gets in the way of port numbers */
config.ajax_proxy = config.router_host + "/cgi-bin/ajax-proxy.cgi?url=";  /* TODO - implement the funky ajax security hack */
config.dashboard_host = config.ajax_proxy + "http://vdashboard.7degrees.co.za";
config.batman_vis_host = config.ajax_proxy + "http://vdashboard.7degrees.co.za:2004";
config.gateway_host = "192.168.20.1";  /* used by: snmp to get gateway traffic stats*/


/* - json commands -------------------------------------------------------- */
var uci_show = { 
  package: "uci",  /* uci | opkg | snmp | radius | batman | ? */
  command: "show" 
  /* arguments : [ { config  : "",
                     section : "",
                     option  : "",
                     value   : ""  } ] */
};

var snmp_traffic = {
  package: "snmp",
  command: "get",
  address: config.gateway_host, 
  community: "public",
  oids: [ ".1.3.6.1.2.1.2.2.1.10.4",     /* interfaces.ifTable.ifEntry.ifInOctets.4  ->  eth0.1 == .4 */ 
          ".1.3.6.1.2.1.2.2.1.16.4" ]    /* interfaces.ifTable.ifEntry.ifOutOctets.4 ->  eth0.1 == .4 */
}

var batman_vis = {
  package: "batman",
  command: "vis"
};


/* - initialization -------------------------------------------------------- */
function load_mesh_modules() {

  /* ---------------------------------------------------------------------- */
  /* - menu drawers - */
  $("li.drawer h2:not(.open)").next().hide();
  $("h2.drawer-handle").mouseover(function () {
    // find the open drawer, remove the class, move to the UL following it and hide it
    $("h2.open").removeClass("open").next().hide();
    // add the open class to this H2, move to the next element (the UL) and show it
    $(this).addClass("open").next().show(); 
  });


  /* ---------------------------------------------------------------------- */
  /* - CUSTOMERS -> recharge - */
  $("#menu-customers-recharge").click(function() {
      $("#content").hide()
        .load("widgets/customers-recharge.html", function() {
            mesh_customers_recharge();
            $("#content").fadeIn("slow");
            $(".button").corner("8px");
            $(".button-submit").corner("8px");
          });
    });

  /* - CUSTOMERS -> watch - */
  $("#menu-customers-watch").click(function() {
      $("#content").hide()
        .load("widgets/customers-watch.html", function() {
            mesh_customers_watch();
            $("#content").fadeIn("slow");
          });
    });

  /* - CUSTOMERS -> manage - */
  $("#menu-customers-manage").click(function() {
      $("#content").hide()
        .load("widgets/customers-manage.html", function() {
            mesh_customers_manage();
            $("#content").fadeIn("slow");
          });
    });
  

  /* ---------------------------------------------------------------------- */
  /* - VILLAGE -> mesh - */
  $("#menu-village-nodes").click(function() {
      $("#content").hide()
        .load("widgets/village-nodes.html", function() {
            mesh_village_nodes();
            $("#content").fadeIn("slow");
          });
    });

  /* - VILLAGE -> configure mesh - */
  $("#menu-village-configure").click(function() {
      $("#content").hide()
        .load("widgets/village-configure.html", function() {
            mesh_village_configure();
            $("#content").fadeIn("slow");
          });
    });

  /* - VILLAGE -> weather - */
  $("#menu-village-weather").click(function() {
      $("#content").hide()
        .load(config.ajax_proxy + "http://www.icon.co.za/~aleroux/ div.menubody", function() {
            $("#content").fadeIn("slow");
          });
      return false;
    });
  
  $("#menu-village-weather-complex").click(function() {
      $("#content").children("#widget").hide();
      $.ajax({ 
          url: "http://meraka.7degrees.co.za/cgi-bin/ajax-proxy.cgi?url=http://www.icon.co.za/~aleroux/",
            processData: true,
            dataType: "html",
            success: 
          function(data) {
            $("#widget").html(data);
            $("#widget").fadeIn("slow");
          }
        })
      });

  /* - VILLAGE -> map - */
  $("#menu-village-map").click(function() {
      $("#content").hide()
        .load("widgets/village-map.html", function() {
            mesh_village_map();
            $("#content").fadeIn("slow");
            $("#widget-village-map").width("100%");                                  // TODO - grrrrr
            $("#widget-village-map").height("75%");
            village_map.setCenter(village_map.getCenter(), village_map.getZoom());
          });

    });

  /* ---------------------------------------------------------------------- */
  /* - INTERNET -> configure - */
  $("#menu-internet-configure").click(function() {
      $("#content").hide()
        .load("widgets/internet-configure.html", function() {
            $("#content").fadeIn("slow");
            mesh_internet_configure();
            $(".button").corner("4px");
          });
    });


  /* ---------------------------------------------------------------------- */
  /* - ROUTER -> configure - */
  $("#menu-router-configure").click(function() {
      $("#content").hide()
        .load("widgets/router-configure.html", function() {
            $.ajax({ 
                url: config.router_host + "/cgi-bin/village-bus-uci", 
                  type: "POST", 
                  contentType: "application/json", 
                  processData: false,
                  dataType: "json", data: $.toJSON(uci_show),
                  success: 
                function(data) {
                  if (data.length != 1) { $("div#widget-router-configure").html("no data"); return; }
                  var config = data[0];
                  var s = '<pre>';
                  for (package in config) {
                    for (section in config[package]) {
                      for (option in config[package][section]) {
                        s += package + "." + section + "." + option + "  :  " + config[package][section][option]  + "\n";
                      }
                    }
                  }
                  s += '</pre>';
                  $("div#widget-router-configure").html(s);
                }
              });
            $("#content").fadeIn("slow");
          });
      return false;
    });
  

  /* ---------------------------------------------------------------------- */
  /* - EVERYTHING -> watch - */
  $("#menu-everything-watch").click(function() {
      $("#content").hide()
        .load("widgets/everything-watch.html", function() {
            $("#content").fadeIn("slow");
            $(".traffic-village").sparkline([0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], {type: 'bar'});
            update_traffic_graph();
          });
      return false;
    });

  /* - EVERYTHING -> help - */
  $("#menu-everything-help").click(function() {
      $("#content").hide()
        .load("widgets/everything-help.html", function() {
            /*mesh_everything_help();*/
            $("#content").fadeIn("slow");
          });
    });

  /* ---------------------------------------------------------------------- */
  /* - SETUP -> router - */
  $("#menu-setup-router").click(function() {
      $("div#widget-setup-router").html("connecting to mesh...");
      $("div#widget-setup-router").html("connected");

      // TODO - setup should probably pull from the dashboard server rather than the local router...
      $("#content").hide()
        .load("widgets/setup-router.html", function() {
            $("#content").fadeIn("slow");
            mesh_setup_router();
          });
      return false;
    });

  /* - SETUP -> mesh - */
  $("#menu-setup-mesh").click(function() {
      $("#content").hide()
        .load("widgets/setup-mesh.html", function() {
            $("#content").fadeIn("slow");
            mesh_setup_router();
          });
      return false;
    });


}


/**
 * dump_object
 * TODO - move to util.js
 */
function dump_object(o) {
  var s = "<pre>";
  for (p in o) {
    s += (p + "=" + o[p] + "\n");
  }
  s += "</pre>";
  return s;
}

