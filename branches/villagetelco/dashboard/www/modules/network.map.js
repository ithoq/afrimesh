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

  /** Includes ---------------------------------------------------------- */


  /** Map --------------------------------------------------------------- */
  function _Map(id, longitude, latitude, extent, zoom, update_target) {

    this.update_target    = update_target;
    this.update_frequency = 3000;
    var the_map = create_map();

    function create_map() { 
      var options = {
        projection        : epsg_900913,
        displayProjection : epsg_4326,
        units             : "m",
        numZoomLevels     : 20,
        controls          : [ new OpenLayers.Control.Navigation({ 
                                zoomWheelEnabled : false }),
                              new OpenLayers.Control.PanPanel(),
                              new OpenLayers.Control.ZoomPanel() ],
        //theme             : "style/map.default.css"  
        theme             : "style/map.css?version=43"  // Ffffffffffuuuuuuuuuu!!!! Damn you Safaris!!!!
      };
      var map = new OpenLayers.Map(id, options);
      var mapnik = new OpenLayers.Layer.OSM.Mapnik("Street Map");
      var relief = undefined;
      console.debug("map server: " + afrimesh.settings.map.server);
      if (afrimesh.settings.map.server == "openstreetmap.org") {
        relief = new OpenLayers.Layer.OSM.CycleMap("Relief Map");
      } else {
        relief = new OpenLayers.Layer.OSM.LocalMap("Relief Map", "http://" + afrimesh.settings.map.server + "/tiles/");
      }
      if (afrimesh.settings.map.aerial == "1") {
        var satellite = new OpenLayers.Layer.Yahoo("Aerial Map", { // TODO - http://labs.metacarta.com/osm/over-yahoo/
            type              : YAHOO_MAP_SAT,
            sphericalMercator : true
          });
        map.addLayers([ relief, mapnik, satellite ]);
      } else {
        map.addLayers([ relief, mapnik ]);
      }
      map.addControl(new OpenLayers.Control.Attribution());
      map.addControl(new OpenLayers.Control.MousePosition());
      map.addControl(new OpenLayers.Control.LayerSwitcher({activeColor:"black"}));
      map.addControl(new OpenLayers.Control.ScaleLine());
      map.setCenter(LonLat(longitude, latitude), zoom);
      if (!map.getCenter()) { map.zoomToMaxExtent(); }
      map.addLayer(new OpenLayers.Layer.Vector("Mesh Links"));
      map.addLayer(new OpenLayers.Layer.Vector("Mesh Routers"));
      map.routes = map.getLayersByName("Mesh Links")[0];
      map.routers  = map.getLayersByName("Mesh Routers")[0];

      map.dragger = new OpenLayers.Control.DragFeature(map.routers);
      map.dragger.onComplete = on_position;
      map.addControl(map.dragger);
      map.dragger.activate();

      var router_hover_selector = new OpenLayers.Control.SelectFeature(map.routers, { 
        multiple : false, 
        hover    : false });
      router_hover_selector.onSelect = on_select_router;
      router_hover_selector.onUnselect = on_unselect_router;
      map.addControl(router_hover_selector);
      router_hover_selector.activate();

      return map;
    };


    /** 
     * Zoom map
     */
    this.zoom = function(level) {
      the_map.zoomTo(level);
    };

    /**
     * Map center
     */
    this.center = function(longitude, latitude) {
      if (!longitude) {
        return the_map.getCenter();
      }
      if (the_map.zoom < 10) {
        the_map.zoomTo(10);
      }
      the_map.panTo(LonLat(longitude, latitude));
    };

    /**
     * Register a callback for dragging a specific feature
     */
    this.on_drag_feature = function(feature, callback) {

      // add onDrag handler that responds to this router address
      var address = feature.router.address;
      the_map.dragger.onDrag = function(feature) {
        if (feature.router.address != address) { return; }
        var location = new OpenLayers.LonLat(feature.geometry.x, feature.geometry.y).transform(epsg_900913, epsg_4326);
        return callback(location.lon, location.lat);
      };

      // disable routes layer to reduce visual clutter
      the_map.routes.display(false);

      // highlight it on the map
      feature.pulse = (function() {
        var scale  = 1.0;
        var inc    = 0.1;
        function pulse() {
          if (scale >= 1.3 || scale <= 0.9) inc = - inc;
          scale += inc;
          feature.style.pointRadius   = 10.0 * scale;
          feature.style.fillOpacity   = 1.0 * (1.9 - scale);
          the_map.routers.drawFeature(feature);
        };
        return setInterval(pulse, 100);
      })();

      return feature;
    };

    /**
     * Disable callback
     */
    this.reset_drag_feature = function(feature) {
      the_map.dragger.onDrag = undefined;
      clearTimeout(feature.pulse);
      feature.style.pointRadius = 10.0;
      feature.style.fillOpacity = 1.0;
      the_map.routers.drawFeature(feature);
      on_position_update_routes(feature);
      the_map.routes.display(true);
    };


    /** 
     * Return the map feature representing the given router, creating it and 
     * adding it to the map if it doesn't exist yet.
     */
    this.routers = the_map.routers;
    this.router = function(router) {
      // TODO polymorphism please for all callers
      if (isString(router)) {
        var feature = the_map.routers.getFeatureById(router);
        console.log("Returning feature: " + router + " -> " + feature);
        return feature; 
      }
      var feature = the_map.routers.getFeatureById(router.address);
      if (feature) {
        return feature;
      } // TODO check that it's a complete router object before adding it
      return add_router(router);
    };
    function add_router(router) {
      var feature = new OpenLayers.Feature.Vector();
      feature.style = { fillOpacity: 1.0, 
                        strokeOpacity: 1.0,
                        strokeColor: "black",
                        strokeWidth: 1.0,
                        pointRadius: 10.0 };
      feature.style.fillColor = router.routes.some(function(route) { return route.label == "HNA"; }) ? "red" : "blue";
      feature.id = router.address;
      feature.router = router;
      the_map.routers.addFeatures([feature]);
      afrimesh.device.location(router.address, function (error, longitude, latitude) {
          if (error) return console.error(error);
          feature.geometry = new Point(longitude, latitude);
          the_map.routers.redraw();
        });
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
      feature.style = { strokeWidth: 10,
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
    var quality = { 
      good  : 1.02,
      bad   : 1.04,
      fault : 1.06
    };
    
    var lq_to_color = function(lq) {
      if (lq == "HNA") { // gateway 
        return "black";
      }
      lq = parseFloat(lq);
      if (lq > quality.fault)     {  return { rgb : "silver",     a : 0.1 };  } /* 246-xF6 020-x14 006-x06 - red */
      else if (lq > quality.bad)  {  return { rgb : "orange",     a : 1.0 };  } /* 249-xF9 165-xA5 000-x00 */
      else if (lq > quality.good) {  return { rgb : "lightgreen", a : 1.0 };  } /* 144-x90 238-xEE 144-x90 */
      return { rgb : "green", a : 1.0 };                                        /* 034-x22 128-x80 000-x00 */
    }
    this.lq_to_color = lq_to_color;
    
    var lq_to_argb = function(lq) {
      if (lq == "HNA") { // gateway 
        return "ff000000";
      }
      lq = parseFloat(lq);
      if (lq > quality.fault)     {  return "f61406ff";  } 
      else if (lq > quality.bad)  {  return "f9a500ff";  }
      else if (lq > quality.good) {  return "90ee90ff";  } 
      return "228000ff";
    }
    this.lq_to_argb = lq_to_argb;
    

    /* event handling ----------------------------------------------------- */     

    function on_select_router(feature) {
      the_map.selected = feature;
      var html = "<div class='popup'>";
      html += "<div id='address'>";
      html += "<span id='ip'>" + "<a href='#'>" + feature.router.address + "</a>" + "</span>&nbsp;&nbsp;";
      html += "<span id='mac'></span>";
      html += "</div>";
      var last_seen = (new Date()) - feature.last_seen;
      html += "<div id='health'>";
      if (last_seen <= (update_target * 2.0)) { // UDE - this is a bit clumsy
        html += "node is healthy";
      } else {
        html += "<span style='color:red;'>node last checked in " + pretty_seconds(last_seen / 1000.0) + " ago</span>";
      }
      html += "</div>";
      html += "<div id='traffic'></div>";
      html += "<div id='neighbours'>";
      html += "Neighbours";
      html += "<table border=0>";
      var rowend = false;
      feature.router.routes.map(function (route) {
        if (route.neighbour) { 
          html += (!rowend ? "<tr>" : "");
          html += "<td><span style='color:" + lq_to_color(route.label).rgb + "'>";
          html += route.neighbour + "(" + route.label + ")&nbsp;";
          html += "</span></td>";
          html += (rowend ? "</tr>" : "");
          rowend = !rowend;
        }
      });
      html += (rowend ? "</tr>" : "") + "</table>";
      html+= "</div>";  /* end neighbours */
      html += "</div>"; /* end popup */
      var popup = new OpenLayers.Popup.AnchoredBubble("router_popup_" + feature.router.address,
                                                      feature.geometry.getBounds().getCenterLonLat(),
                                                      new OpenLayers.Size(275, 150), html, null, true, 
                                                      function(event){on_unselect_router(the_map.selected);} );
      console.log("POP: " + popup.id);
      popup.setBackgroundColor("black");
      popup.setOpacity(0.95);
      popup.autoSize = false;
      feature.popup = popup;
      the_map.addPopup(popup);
      // Stupid Safari fix
      $(".olPopupContent").css("overflow", "hidden");
      $(".popup span#ip").bind("click", function(event) {
          evil_display_overlay("http://" + feature.router.address +"/");
        });

      // populate accounting information
      afrimesh.network.accounting.router(feature.router, function(error, router) {
          if (error) return console.debug("Could not retrieve accounting information for router: " + error);
          var packets = (router.traffic.packets ? router.traffic.packets : "-");
          var bytes   = (router.traffic.bytes   ? pretty_bytes(router.traffic.bytes) : "-");
          $(".popup div#traffic").html("<span>" + packets + " packets " + bytes   + " </span>");
          $(".popup span#mac").html(feature.router.mac ? feature.router.mac : "unknown mac");  
        });
    };


    function on_unselect_router(feature) {
      the_map.selected = null;
      if (feature.popup) {
        the_map.removePopup(feature.popup);
        feature.popup.destroy();
        feature.popup = null;
      }
    };


    function on_select_route(feature) {
    };
    function on_unselect_route(feature) {
    };

    function on_position(feature) {
      // update router location config
      var location = new OpenLayers.LonLat(feature.geometry.x, feature.geometry.y).transform(epsg_900913, epsg_4326);
      afrimesh.device.location.set(feature.router.address, location.lon, location.lat, function (error, response) {
          if (error) return console.error("Could not update router location for " + feature.router.address + ": " + error);
          console.debug("Updated router location for:" + feature.router.address);
        });

      // don't update route geometry!
      if (the_map.dragger.onDrag) { 
        return; 
      }

      // update route geometry
      on_position_update_routes(feature);      
    };

    function on_position_update_routes(feature) {
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


})();

