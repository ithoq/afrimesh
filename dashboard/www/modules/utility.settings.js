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
var LocationMap = null;

(function() {

  /** mapping Includes ---------------------------------------------------- */
  load("javascript/afrimesh.maps.js"); 
  OpenLayers.ImgPath = "images/";


  /** populate DOM with the values in afrimesh.settings ------------------- */
  populate_dom = function() {
    var elements = $("[id*=afrimesh|settings]");
    elements.each(function(element) { 
        element = elements[element];
        if (element instanceof HTMLSelectElement) { // TODO - handle select elements
        } else if (element instanceof HTMLInputElement) {
          $(element).val(Q(afrimesh, element.id) + ""); 
        } else if (element instanceof HTMLElement) {
          $(element).html(Q(afrimesh, element.id) + ""); 
        } else {
          console.warn("UNKNOWN ELEMENT TYPE: " + element);
        }
      });
  };


    /** query internet gateway's interfaces via snmp ---------------------- */
    populate_select_interface = function() {
      $("input.[id*=afrimesh|settings|internet_gateway|address]").css("background", "#FFAAAA");
      var interfaces = afrimesh.villagebus.snmp.walk(afrimesh.settings.internet_gateway.address, 
                                                     afrimesh.settings.internet_gateway.snmp.community, 
                                                     [ ".1.3.6.1.2.1.2.2.1.2" ]);  // IF-MIB::ifDescr
      console.log("GETTING INTERFACES: " + interfaces);
      if (!interfaces || interfaces.error) {
        console.debug("SNMP ERROR: " + (interfaces ? interfaces.error : "unknown error"));
        console.debug("dashboard server: " + afrimesh.settings.hosts.dashboard_server);
        console.debug("settings  server: " + afrimesh.settings.address);
        $("select.[id*=afrimesh|settings|internet_gateway|snmp|interface]").html("");
        return;
      }
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
    }

    update_vis_server = function() {
      $("input.[id*=afrimesh|settings|hosts|batman_vis_server]").css("background", "#FFAAAA");
      try {
        var routes = afrimesh.villagebus.batman();
        console.debug("We got: " + routes);
        if (routes != undefined && isArray(routes)) {
          $("input.[id*=afrimesh|settings|hosts|batman_vis_server]").css("background", "#AAFFAA");
        }
      } catch (error) {
        console.debug("Vis server is unreachable. " + error);
      }
    }

    /** create a map which can be used to set the router location --------- */
    function _LocationMap(id, longitude, latitude, extent, on_position) {

      var the_map = (function() { 
          var options = {
            projection        : epsg_900913,
            displayProjection : epsg_4326,
            units             : "m",
            numZoomLevels     : 20,
            restrictedExtent  : new OpenLayers.Bounds(),
            theme             : "style/map.default.css"
          };
          options.restrictedExtent.extend(LonLat(longitude - extent, latitude - extent));
          options.restrictedExtent.extend(LonLat(longitude + extent, latitude + extent));
          var map = new OpenLayers.Map(id, options);
          map.addLayers([ new OpenLayers.Layer.OSM.CycleMap("Relief Map") ]);
          map.addControl(new OpenLayers.Control.Attribution());
          map.addControl(new OpenLayers.Control.MousePosition());
          map.addControl(new OpenLayers.Control.ScaleLine());
          //map.setCenter(LonLat(longitude, latitude), zoom);
          if (!map.getCenter()) { map.zoomToMaxExtent(); }
          map.addLayer(new OpenLayers.Layer.Vector("Mesh Routers"));
          map.routers  = map.getLayersByName("Mesh Routers")[0];
          
          var dragger = new OpenLayers.Control.DragFeature(map.routers);
          dragger.onComplete = on_position;
          map.addControl(dragger);
          dragger.activate();
          return map;
        })(); /* create_map */
      

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
        afrimesh.villagebus.uci.get.async(function (config) {
            feature.geometry = new Point(config.afrimesh.location.longitude, config.afrimesh.location.latitude);
            the_map.routers.redraw();
          }, router.address, "afrimesh.location");
        return feature;
      };
      
    
    };
    LocationMap = _LocationMap;

})();
