/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

var Map = undefined;
(function() {

  /** Includes ------------------------------------------------------------ */
  if (afrimesh.settings.map.aerial) {
    //load("http://maps.google.com/maps?file=api&amp;v=2&amp;key=ABQIAAAAUILHePuv7Cq0mK0HSze5PRRTJkofi8MdSXz7uUeCtVadpg691BS55tAxiNsjEjEAKrgA9qrFrG_PPw");
    //load("http://api.maps.yahoo.com/ajaxymap?v=3.0&appid=euzuro-openlayers");
    load("javascript/yahoo/ajaxymap.js");
    load("javascript/yahoo/dom_2.0.1-b2.js");
    load("javascript/yahoo/event_2.0.0-b2.js");
    load("javascript/yahoo/dragdrop_2.0.1-b4.js");
    load("javascript/yahoo/animation_2.0.1-b2.js");
    load("javascript/yahoo/ymapapi_3_8_2_1.js");
  }
  load("javascript/openlayers/OpenLayers.js");
  load("javascript/openlayers/OpenStreetMap.js");
  OpenLayers.ImgPath = "images/";



  /** Map ----------------------------------------------------------------- */
  function _Map(id, longitude, latitude, extent, zoom, update_frequency) {

    this.update_frequency = update_frequency;
    the_map = create_map();

    function create_map() { 
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
      //var osma = new OpenLayers.Layer.OSM.Osmarender("Osmarender");
      var mapnik = new OpenLayers.Layer.OSM.Mapnik("Street Map");
      var relief = new OpenLayers.Layer.OSM.CycleMap("Relief Map");
      if (afrimesh.settings.map.aerial) {
        var satellite = new OpenLayers.Layer.Yahoo("Aerial Map", { // TODO - http://labs.metacarta.com/osm/over-yahoo/
            type              : YAHOO_MAP_SAT,
            sphericalMercator : true
          });
        map.addLayers([ relief, /*osma,*/ mapnik, satellite ]);
      } else {
        map.addLayers([ relief, /*osma,*/ mapnik ]);
      }
      map.addControl(new OpenLayers.Control.Attribution());
      map.addControl(new OpenLayers.Control.MousePosition());
      map.addControl(new OpenLayers.Control.LayerSwitcher());
      map.addControl(new OpenLayers.Control.ScaleLine());
      map.setCenter(LonLat(longitude, latitude), zoom);
      if (!map.getCenter()) { map.zoomToMaxExtent(); }
      map.addLayer(new OpenLayers.Layer.Vector("Mesh Links"));
      map.addLayer(new OpenLayers.Layer.Vector("Mesh Routers"));
      map.routes = map.getLayersByName("Mesh Links")[0];
      map.routers  = map.getLayersByName("Mesh Routers")[0];

      var dragger = new OpenLayers.Control.DragFeature(map.routers);
      dragger.onComplete = on_position;
      map.addControl(dragger);
      dragger.activate();
      /**var click_selector = new OpenLayers.Control.SelectFeature(map.routers);
      click_selector.multiple = false;
      click_selector.toggle = true;
      click_selector.onSelect = on_select_router;
      click_selector.onUnselect = on_unselect_router;
      map.addControl(click_selector);
      click_selector.activate();*/
      var hover_selector = new OpenLayers.Control.SelectFeature(map.routers, 
                                                                { multiple : false, 
                                                                  hover    : true });
      hover_selector.onSelect = on_select_router;
      hover_selector.onUnselect = on_unselect_router;
      map.addControl(hover_selector);
      hover_selector.activate();
      return map;
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
      feature.style = { fillColor: "blue", 
                        fillOpacity: 0.0, 
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


    /** 
     * Return the map feature representing the given route, creating it and 
     * adding it to the map if it doesn't exist yet.
     */
    this.routes = the_map.routes;
    this.route = function(route) {
      if (route.gateway) { return { }; } // skip gateways for now
      var feature = the_map.routes.getFeatureById(route.router + "->" + route.neighbour);
      if (feature) {
        return feature;
      }
      return add_route(route);
    };
    function add_route(route) {
      var feature_origin      = the_map.routers.getFeatureById(route.router);
      var feature_destination = the_map.routers.getFeatureById(route.neighbour);
      if (!feature_origin      || !feature_origin.geometry ||
          !feature_destination || !feature_destination.geometry) {
        //console.debug("Still waiting for location information for route: " + (route.router + "->" + route.neighbour));
        return {};
      }
      var feature = new OpenLayers.Feature.Vector();
      feature.style = { strokeOpacity: 0.0,
                        strokeColor: "black",
                        strokeWidth: 10,
                        strokeDashstyle: "solid",
                        strokeLinecap: "round",
                        pointRadius: 0 };
      feature.id = route.router + "->" + route.neighbour;
      feature.route = route;
      feature.geometry = new OpenLayers.Geometry.LineString([ feature_origin.geometry, feature_destination.geometry ]);
      the_map.routes.addFeatures([feature]);
      //console.debug("Added route for route: " + (route.router + "->" + route.neighbour));
      return feature;
    };


    /**
     * Take a B.A.T.M.A.N. link quality value and convert it to a HTML color string
     */
    this.lq_to_color = function(lq) {
      if (lq == "HNA") {
        return "black";
      }
      lq = lq + 0.0;
      if (lq > 1.06) {
        return "red";
      } else if (lq > 1.04) {
        return "orange";
      } else if (lq > 1.02) {
        return "green";
      } 
      return "lightblue";
    }
    
    
    /* event handling ----------------------------------------------------- */

    function on_select_router(feature) {
      the_map.selected = feature;
      var html = "<div class='popup'>";
      html += "<p><span id='ip'>" + feature.router.address + "</span>&nbsp;&nbsp;<span id='mac'>ma:ca:dd:rr:es:ss</span></p><br/>";
      var now = new Date();
      var last_seen = now - feature.last_seen;
      if (last_seen <= (update_frequency * 2.0)) { // UDE - this is a bit clumsy
        html += "<p><span id='health'>node is healthy</span></p>";
      } else {
        html += "<p><span id='health' style='color:red;'>node is missing</span></p>";
        html += "<p><span id='message'>last checked in " + Math.floor(last_seen / 1000) + " seconds ago</span></p>";
      }
      html += "</div>";
      var popup = new OpenLayers.Popup.AnchoredBubble("Chicken" + feature.router.address,
                                                      feature.geometry.getBounds().getCenterLonLat(),
                                                      null, html, null, true, 
                                                      function(event){on_unselect_router(the_map.selected);} );
      popup.setBackgroundColor("black");
      popup.setOpacity(0.9);
      popup.autoSize = true;
      feature.popup = popup;
      the_map.addPopup(popup);
    };

    function on_unselect_router(feature_router) {
      the_map.selected = null;
      if (feature_router.popup) {
        the_map.removePopup(feature_router.popup);
        feature_router.popup.destroy();
        feature_router.popup = null;
      }
    };

    function on_position(feature) {
      // update router location config
      var location = new OpenLayers.LonLat(feature.geometry.x, feature.geometry.y).transform(epsg_900913, epsg_4326);
      afrimesh.villagebus.uci.set.async(function (response) {
          console.debug("Updated router location for:" + feature.router.address);
        }, 
        feature.router.address, 
        [ { config: "afrimesh", section: "location", option: "longitude", value: location.lon }, 
          { config: "afrimesh", section: "location", option: "latitude",  value: location.lat } ]);
      
      // update route geometry
      feature.router.routes.map(function(route) {
          if (route.gateway) { return; } // skip gateways for now
          var feature_destination = the_map.routers.getFeatureById(route.neighbour);
          var feature_route       = the_map.routes.getFeatureById(route.router + "->" + route.neighbour);
          if (!feature_destination || !feature_route) {
            return;
          }
          // just a tad irritating to have to remove and re-add features when changing geometry!
          the_map.routes.removeFeatures([feature_route]); 
          feature_route.geometry = new OpenLayers.Geometry.LineString([ feature.geometry, feature_destination.geometry ]); 
          the_map.routes.addFeatures([feature_route]);
          the_map.routes.redraw();
          console.debug("Updated route geometry for: " + (route.router + "->" + route.neighbour));
        });
    };

  };
  Map = _Map;



  /** 
   * convenience constructors 
   */
  var epsg_4326   = new OpenLayers.Projection("EPSG:4326");
  var epsg_900913 = new OpenLayers.Projection("EPSG:900913");
  function LonLat(longitude, latitude) {
    return new OpenLayers.LonLat(longitude, latitude).transform(epsg_4326, epsg_900913); 
  };
  function Point(longitude, latitude) {
    return new OpenLayers.Geometry.Point(longitude, latitude).transform(epsg_4326, epsg_900913);  
  };



})();

