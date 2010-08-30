/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

var populate_dom = null;
var populate_select_interface = null;
var populate_mesh_controls = null;
var update_sip_server = null;
var update_iax_server = null;
var update_radius_server = null;
var update_vis_server = null;
var update_accounting_server = null;
var update_map_server = null;
var update_location = null;
var LocationMap = null;
(function() {

  /** populate DOM with the values in afrimesh.settings ------------------- */
  populate_dom = function() {
    var elements = $("[id*=afrimesh|settings]");
    elements.each(function(element) {
        element = elements[element];
        if (element instanceof HTMLSelectElement) { // TODO - handle select elements
        } else if (element instanceof HTMLInputElement) {
          if (element.type == "checkbox") {
            var checked = Q(afrimesh, element.id);
            if (checked == "on" || checked == "1" || checked == "true") {
              checked = true;
            } else {
              checked = false;
            }
            $(element).attr("checked", checked); 
          } else {
            var value = Q(afrimesh, element.id);
            if (value != undefined) {
              $(element).val(String(value)); 
            } else {
              $(element).val("");
            }
          }
        } else if (element instanceof HTMLElement) {
            var value = Q(afrimesh, element.id);
            if (value != undefined) {
              $(element).html(String(value)); 
            } else {
              $(element).html("");
            }
        } else {
          console.warn("UNKNOWN ELEMENT TYPE: " + element);
        }
      });
  };


  /** query internet gateway's interfaces via snmp ---------------------- */
  populate_select_interface = function() {
    $("input.[id*=afrimesh|settings|internet_gateway|address]").css("background", "#FFAAAA");
    afrimesh.device.snmp.walk(afrimesh.settings.internet_gateway.address,
                              afrimesh.settings.internet_gateway.snmp.community,
                              ".1.3.6.1.2.1.2.2.1.2",  // IF-MIB::ifDescr 
      function(error, interfaces) {
          if (error) return console.error("Could not access internet gateway via SNMP: " + error);
          if (interfaces == undefined) {
            $("p.[id*=internet_gateway|error]").html("Internet gateway unreachable.");
            console.debug("SNMP ERROR: " + (interfaces ? interfaces.error : "unknown error"));
            console.debug("settings  server: " + afrimesh.settings.address);
            $("select.[id*=afrimesh|settings|internet_gateway|snmp|interface]").html("");
            return;
          }
          $("p.[id*=internet_gateway|error]").html("");
          $("input.[id*=afrimesh|settings|internet_gateway|address]").css("background", "#AAFFAA");
          var interface_current = afrimesh.settings.internet_gateway.snmp.interface;
          var interface_count = 1;
          var options = "";
          for (var interface in interfaces) {
            options += "<option value='" + interface_count + "' ";
            options += (interface_count == interface_current ? "selected" : "") + ">";
            options += interfaces[interface];
            options += "</option>";
            interface_count++;
          }
          $("select.[id*=afrimesh|settings|internet_gateway|snmp|interface]").html(options);
      });
  }

  update_mesh_controls = function() {
    var routing_class = afrimesh.settings.network.mesh.routing_class;
    var gateway_class = afrimesh.settings.network.mesh.gateway_class;
    if (gateway_class != undefined && 
        gateway_class != "0" && 
        gateway_class != "") {
      $("input[name='is_mesh_gateway']").attr("checked", true);
    } else {
      $("input[name='is_mesh_gateway']").attr("checked", false);
      $("select.[id*=afrimesh|settings|network|mesh|routing_class]").val(routing_class);
    }
    $("input[name='is_mesh_gateway']").change();
  };
  
  update_sip_server = function() {
    $("fieldset#asterisk :text, :password").css("background", "#FFAAAA");
    var enabled = $("input.[id*=afrimesh|settings|voip|sip|enable]").attr("checked");
    if (!enabled) {
      $(".sip_trunk").hide();
      return;
    } 
    $(".sip_trunk").show();
    var server = $("input.[id*=afrimesh|settings|voip|sip|address]").val();
    afrimesh.telephony.sip.peers("self", function(error, peers) {
      if (error) return console.error("Could not get sip peers: " + error);
        peers.map(function(peer) {
            console.debug(show(peer));
            if (peer.host == server && peer.status == "OK") {
              $("fieldset#asterisk :text, :password").css("background", "#AAFFAA");
            }
          });
    });
  };

  update_iax_server = function() {
    //console.debug("checked: " + $("input.[id*=afrimesh|settings|voip|iax|enable]").is(":checked"));
    var enabled = $("input.[id*=afrimesh|settings|voip|iax|enable]").attr("checked");
    if (!enabled) {
      $(".iax_trunk").hide();
      return;
    } 
    $(".iax_trunk").show();
    var server = $("input.[id*=afrimesh|settings|voip|iax|address]").val();
    console.debug("asterisk server: " + server);
    // TODO - try to ping asterisk server
  };
  
  update_vis_server = function() {
    $("input.[id*=afrimesh|settings|network|mesh|vis_server]").css("background", "#FFAAAA");
    try {
      afrimesh.network.routes(function (error, routes) {
          if (error || !isArray(routes)) {
            $("p.[id*=vis_server|error]").html("Visualization server unreachable.");
            return console.debug("Visualization server unreachable: " + error);
          }
          $("input.[id*=afrimesh|settings|network|mesh|vis_server]").css("background", "#AAFFAA");
          $("p.[id*=vis_server|error]").html("");
        });
    } catch (error) {
      $("p.[id*=vis_server|error]").html("Visualization server unreachable. " + error + ".");
      console.debug("Vis server is unreachable. " + error);
    }
  };

  update_accounting_server = function() {
    $("input.[id*=afrimesh|settings|network|mesh|accounting_server]").css("background", "#FFAAAA");
    try {
      afrimesh.network.accounting(function (error, data) {
          if (error && !isArray(data)) {
            $("p.[id*=accounting_server|error]").html("Accounting server unreachable.");
            return console.debug("utility.settings.js->update_vis_server: Visualization server unreachable.");
          }
          $("input.[id*=afrimesh|settings|network|mesh|accounting_server]").css("background", "#AAFFAA");
          $("p.[id*=accounting_server|error]").html("");
        });
    } catch (error) {
      $("p.[id*=vis_server|error]").html("Visualization server unreachable. " + error + ".");
      console.debug("Vis server is unreachable. " + error);
    }
  };

  update_map_server = function() { 
    $("input.[id*=afrimesh|settings|map|server]").css("background", "#FFAAAA")
    $("#location").addClass("message");
    $("#location").html("<p><br/><p>Contacting Map Server...</p>");
    // if it's a valid map server it will have the OSM/OpenLayers scripts hosted
    try {
      //var request = $.get(afrimesh.villagebus.ajax_proxy() + "http://" + afrimesh.settings.map.server + "/openlayers/OpenStreetMap.js",
      //                    "", check_complete, "text");
      // TODO - cleanup. Disabled ^^^^ so we don't end up loading the OSM libs twice
      check_complete();
    } catch (error) {
      $("p.[id*=afrimesh|settings|map|server|error]").html("Map server unreachable: " + error + ".");
      console.debug("Map server unreachable: " + error);
      return;
    }

    function check_complete(data, textStatus) {
      if (data == "") {
        var message = "<p><br/><p>Could not contact Map Server.</p>";
        message += "<p><br/><p><b>Please check:</b> ";
        message += "The Map Server address and your network connection.";
        message += "</p>";
        $("#location").html(message);
        return;
      }

      // we have a map server
      $("#location").html("<p><br/><p>Map Server Contacted.</p> <p><br/><p>Loading Network Map...</p>");

      // Now Load OpenStreetMap & OpenLayers libraries
      load.async(afrimesh.settings.map.openlayers_url, function(data, textStatus) {
          if (typeof OpenLayers == "undefined") {
            return load_error("The OpenLayers mapping service could not be found.");
          }
          load.async(afrimesh.settings.map.osm_url, load_complete); // TODO - see above where we disabled double OSM load
        });
    };

    function load_complete(data, textStatus) {
      load("javascript/afrimesh.maps.js"); 
      $("p.[id*=map|server|error]").html("");
      $("#location").replaceWith("<div id='location' />");
      var location_map = new LocationMap("location", 
                                         parseFloat(afrimesh.settings.location.longitude),
                                         parseFloat(afrimesh.settings.location.latitude),
                                         parseFloat(afrimesh.settings.map.extent),
                                         parseInt(afrimesh.settings.map.zoom));
      var router = { address : afrimesh.settings.address };
      location_map.router(router);
      this.location_map = location_map;
      $("input.[id*=afrimesh|settings|map|server]").css("background", "#AAFFAA");
      $("#OpenLayers.Control.PanZoom_5_zoomworld_innerImage").unbind("click");
      $("#OpenLayers.Control.PanZoom_5_zoomworld").unbind("click");
      $("div[id$=_zoomworld]").click(function () {
          location_map.zoom(1);
        });
    };

    function load_error(message) {
      var html = "<p><br/><p>Could Not Contact Map Server.</p>";
      html += "<p><br/><p>" + message + "</p>";
      $("#location").html(html);
    };

  };
  
  update_radius_server = function() {
    var radtype_current = afrimesh.settings.radius.radtype;
    if (radtype_current == 1) {
      $("select.[id*=afrimesh|settings|radius|radtype]").html
        ("<option value = '1' selected>mysql</option> <option value = '2'>memcachedb</option>");
    } else {
      $("select.[id*=afrimesh|settings|radius|radtype]").html
      ("<option value = '1'>mysql</option> <option value = '2' selected>memcachedb</option>");
    }
    $("input.[id*=afrimesh|settings|radius|server]").css("background", "#FFAAAA");

    // TODO - rewrite to be able to block on return of multiple async dispatches :-)
    function cb_status(error, status) {
      status_error = (status && isArray(status) && status[0].error) ? status[0].error : undefined;
      if (error || status_error) { // TODO - only one kind of error
        $("p.[id*=radius|server|error]").html("RADIUS server unreachable. " + error);
        return console.error("RADIUS server is unreachable. " + status_error + " - " + error);
      }
      afrimesh.customers.select(cb_select);
    };
    function cb_select(error, select) {
      select_error = (select && isArray(select) && select[0].error) ? select[0].error : undefined;
      if (error || select_error) {
        $("p.[id*=radius|server|error]").html("mysql database inaccessible. " + error);
        return console.error("mysql database is inaccessible. " + select_error) + " - " + error;
      }
      $("p.[id*=radius|server|error]").html("");
      $("input.[id*=afrimesh|settings|radius|server]").css("background", "#AAFFAA");
    };
    afrimesh.customers.status(cb_status);
  };

  update_location = function() {
    console.debug("locating...");
    /*var userid = "-6354307135308710025";  // Google Latitude ID
    $.getJSON("http://www.google.com/latitude/apps/badge/api?user=" + userid + "&type=json", function(latitude) {
        console.log(JSON.stringify(latitude));
        });*/
    console.debug(afrimesh.settings.location.longitude + ", " + 
                  afrimesh.settings.location.latitude);
    var longitude = parseFloat($("input.[id*=afrimesh|settings|location|longitude]").val());
    var latitude = parseFloat($("input.[id*=afrimesh|settings|location|latitude]").val());
    console.debug(longitude + ", " + latitude);
    this.location_map.center(longitude, latitude);
    network_map.center(longitude, latitude, 14);
  };
  
  /** create a map which can be used to set the router location --------- */
  function _LocationMap(id, longitude, latitude, extent, zoom, on_position) {

    var the_map = (function() { 
        var options = {
          projection        : epsg_900913,
          displayProjection : epsg_4326,
          units             : "m",
          transitionEffect  : "resize",
          controls          : [ new OpenLayers.Control.Navigation({ 
                                  zoomWheelEnabled : false }),
                                new OpenLayers.Control.PanPanel(),
                                new OpenLayers.Control.ZoomPanel() ],
          //numZoomLevels     : 20,
          //theme             : "style/map.default.css"
          theme             : "style/map.css?version=43"
        };
        var map = new OpenLayers.Map(id, options);
        if (afrimesh.settings.map.server == "openstreetmap.org") { // TODO afrimesh.settings.map.server == afrimesh.settings.map.server.default
          map.addLayers([ new OpenLayers.Layer.OSM.CycleMap("Relief Map") ]);
        } else {
          map.addLayers([ new OpenLayers.Layer.OSM.LocalMap("Relief Map", "http://" + afrimesh.settings.map.server + "/tiles/") ]);
        }
        //map.addControl(new OpenLayers.Control.Attribution());
        map.addControl(new OpenLayers.Control.MousePosition());
        map.addControl(new OpenLayers.Control.ScaleLine());
        map.setCenter(LonLat(longitude, latitude), zoom);
        map.addLayer(new OpenLayers.Layer.Vector("Mesh Routers"));
        map.routers  = map.getLayersByName("Mesh Routers")[0];
        map.dragger = new OpenLayers.Control.DragFeature(map.routers);
        if (on_position) {
          map.dragger.onComplete = on_position;
        } else {
          map.dragger.onComplete = function(feature) {     // TODO - ultimately we want this to be just "feature.router.settings.location=" 
            var location = new OpenLayers.LonLat(feature.geometry.x, feature.geometry.y).transform(epsg_900913, epsg_4326);
            var longitude = parseFloat(location.lon).toFixed(7);
            var latitude  = parseFloat(location.lat).toFixed(7);
            $("input.[id*=afrimesh|settings|location|longitude]").val(longitude);
            $("input.[id*=afrimesh|settings|location|latitude]").val(latitude);
            afrimesh.device.location.set(feature.router.address, location.lon, location.lat, function (error, response) {
                if (error) return console.error("Could not update router location for " + feature.router.address + ": " + error);
                console.debug("Updated router location for:" + feature.router.address);
                update_location();
              });
          };
        }
        map.addControl(map.dragger);
        map.dragger.activate();
        return map;
      })(); /* create_map */

    
    /**
     * Recenter map on coordinates
     */
    this.center = function(longitude, latitude, zoom) {
      var feature = this.router({ address : afrimesh.settings.address });
      var location = new LonLat(longitude, latitude);
      feature.move(location);
      if (zoom) {
        the_map.zoomTo(zoom);
      }
      the_map.panTo(LonLat(longitude, latitude));
    };

    /** 
     * Zoom map
     */
    this.zoom = function(level) {
      the_map.zoomTo(level);
    };
    
    /** 
     * Return the map feature representing the given router, creating it and 
     * adding it to the map if it doesn't exist yet.
     */
    this.routers = the_map.routers;
    this.router = function(router) {
      var feature = the_map.routers.getFeatureById(router.address);
      if (feature) {
        return feature;
      }
      return add_router(router);
    };
    function add_router(router) {
      var feature = new OpenLayers.Feature.Vector();
      feature.style = { fillColor: "black", 
                        fillOpacity: 0.5, 
                        strokeOpacity: 1.0,
                        strokeColor: "black",
                        strokeWidth: 1.0,
                        pointRadius: 10.0 };
      feature.id = router.address;
      feature.router = router;
      the_map.routers.addFeatures([feature]);
      feature.geometry = new Point(longitude, latitude);
      the_map.routers.redraw();
      return feature;
    };
    
    
  };
  LocationMap = _LocationMap;


  /** helpers ------------------------------------------------------------- */
  disable_key = function(key, event) {
    var pressed;      
    if (window.event) pressed = window.event.keyCode; // IE
    else              pressed = e.which;              // firefox      
    return (pressed != key);
  }

  
})();
