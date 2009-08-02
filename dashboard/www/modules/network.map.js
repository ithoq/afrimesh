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
  load("javascript/afrimesh.maps.js");
  OpenLayers.ImgPath = "images/";


  /** Map ----------------------------------------------------------------- */
  function _Map(id, longitude, latitude, extent, zoom, update_frequency) {

    this.update_frequency = update_frequency;
    var the_map = create_map();

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
      var relief = undefined;
      console.debug("map server: " + afrimesh.settings.map.server);
      if (afrimesh.settings.map.server == "openstreetmaps.org") {
        relief = new OpenLayers.Layer.OSM.CycleMap("Relief Map");
      } else {
        relief = new OpenLayers.Layer.OSM.LocalMap("Relief Map", "http://" + afrimesh.settings.map.server + "/tiles/");
      }
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
      var router_hover_selector = new OpenLayers.Control.SelectFeature(map.routers, 
                                                                { multiple : false, 
                                                                  hover    : false });
      router_hover_selector.onSelect = on_select_router;
      router_hover_selector.onUnselect = on_unselect_router;
      map.addControl(router_hover_selector);
      router_hover_selector.activate();
      
      var route_hover_selector = new OpenLayers.Control.SelectFeature(map.routes,
                                                                { multiple : false,
                                                                  hover    : true });
      route_hover_selector.onSelect = on_select_route;
      route_hover_selector.onUnselect = on_unselect_route;
      map.addControl(route_hover_selector);
      route_hover_selector.activate();
      
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
    var lq_to_color = function(lq) {
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
    this.lq_to_color = lq_to_color;
    
    /* event handling ----------------------------------------------------- */     

    function on_select_router(feature) {
      afrimesh.network.accounting(feature.router);
      the_map.selected = feature;
      var html = "<div class='popup'>";
      html += "<p><span id='ip'>" + "<a href='#'>" + feature.router.address + "</a>" + "</span>&nbsp;&nbsp;";
      html += "<span id='mac'>" + (feature.router.macaddr ? feature.router.macaddr : "unknown mac")  + "</span></p><br/>";
      var now = new Date();
      var last_seen = now - feature.last_seen;
      if (last_seen <= (update_frequency * 2.0)) { // UDE - this is a bit clumsy
        html += "<p><span id='health'>node is healthy</span></p><br/>";
      } else {
        html += "<p><span id='health' style='color:red;'>node is missing</span></p>";
        html += "<p><span id='message'>last checked in " + Math.floor(last_seen / 1000) + " seconds ago</span></p>";
      }
      if (feature.router.recBytes) {
	      html += "<p><span id='bytes'style='font-size:12'><span style='font-weight:bold'> In/Out: </span>&nbsp;&nbsp;";
		html += "<span id='bytes'style='font-size:12'>" + feature.router.recBytes;
      } else {
	      //html += "<p><span id='bytes'> Bytes transmitted: No Bytes transmitted</span></p><br/>" ;
      }
      if (feature.router.tranBytes) {
	      html += "/" + feature.router.tranBytes + "</span></p><br/>" ;
      } else {
	      //html += "<p><span id='bytes'> Bytes recieved: No Bytes recieved</span></p><br/>" ;
      }
      html += "<p><span id='neighbours'>";
      html+= "Neighbours";
      html+= "<table>";
      var tog = 1;
      feature.router.routes.map(function (route) {
          if (route.neighbour) { 
              var clor = lq_to_color(route.label);
            if (tog == 1) {  
              html += "<tr><td><span style='color:" + clor + "'>";
              html += route.neighbour + "&nbsp;" + "(" + route.label + ")"
              html += "</span>&nbsp" + "</td>";
              tog = 0;
            } else {
              html += "<td><span style='color:" + clor + "'>"; 
              html += route.neighbour + "&nbsp;" +"("+ route.label + ")";
              html += "</span>&nbsp" +"</td></tr>";
              tog =1;
            }
          }
        });
      
      html += "</table>";
      html+= "</span>";
      html += "</div>";
    //var myObj = new getObj('MyText');


      var popup = new OpenLayers.Popup.AnchoredBubble("id" + feature.router.address,
                                                      feature.geometry.getBounds().getCenterLonLat(),
                                                      null, html, null, false, 
                                                      function(event){on_unselect_router(the_map.selected);} );
      popup.setBackgroundColor("black");
      popup.setOpacity(0.9);
      popup.autoSize = false;
      feature.popup = popup;
      the_map.addPopup(popup);

      $("#ip").bind("click", function(event) {
          //evil_display_overlay("http://10.216.144.1/"); 
          evil_display_overlay("http://" + feature.router.address +"/");
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
      console.debug("selected: " + feature);
    }
    
    function on_unselect_route(feature) {
      console.debug("unselected: " + feature);
    }

    function on_position(feature) {
      // update router location config
      var location = new OpenLayers.LonLat(feature.geometry.x, feature.geometry.y).transform(epsg_900913, epsg_4326);
      afrimesh.villagebus.uci.set.async(function (response) {
          console.debug("Updated router location for:" + feature.router.address);
        }, feature.router.address, 
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


})();

