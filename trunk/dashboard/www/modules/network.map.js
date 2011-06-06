/*
 * Afrimesh: easy management for mesh networks
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
        //numZoomLevels     : 20,
        controls          : [ new OpenLayers.Control.Navigation({ 
                                zoomWheelEnabled : false }),
                              new OpenLayers.Control.PanPanel(),
                              new OpenLayers.Control.ZoomPanel() ],
        //theme             : "style/map.default.css"  
        theme             : "style/map.css?version=50",  // Ffffffffffuuuuuuuuuu!!!! Damn you Safaris!!!!
        //allOverlays: true,
        //maxExtent: new OpenLayers.Bounds(1549471.9221, 6403610.94, 1550001.32545, 6404015.8)
      };
      var map = new OpenLayers.Map(id, options);
      var layers = [];

      // map providers - google
      if (afrimesh.settings.map.google == "1") {
        layers.push(new OpenLayers.Layer.Google("Google Hybrid", {
          type: google.maps.MapTypeId.HYBRID,
          numZoomLevels: 21 }));
        layers.push(new OpenLayers.Layer.Google("Google Streetmap",  {
          numZoomLevels: 21 }));
      }

      // map providers - openstreetmap
      console.debug("map server: " + afrimesh.settings.map.server);
      if (afrimesh.settings.map.server == "openstreetmap.org") {
        layers.push(new OpenLayers.Layer.OSM.CycleMap("Relief Map"));
      } else {
        layers.push(new OpenLayers.Layer.OSM.LocalMap("Relief Map", "http://" + afrimesh.settings.map.server + "/tiles/"));
      }
      //layers.push(new OpenLayers.Layer.OSM.Mapnik("Street Map"));

      // grid
      var range = 20000000;
      var step = 1000000;
      var grid_data = [];
      for (var x = -range; x <= range; x += step) {
        grid_data.push([ [ x, -range ], [ x,  range ] ]);
      }
      for (var y = -range; y <= range; y += step) {
        grid_data.push([ [ -range, y ], [  range, y ] ]);
      }
      var grid_json = {
        type     : "FeatureCollection", 
        features : [ { geometry : {
          type : "GeometryCollection", 
          geometries : [ { type        : "MultiLineString", 
                           coordinates : grid_data }, 
                         { type        : "Point", 
                           coordinates : [ 0, 0 ] } ] }, 
                       type : "Feature", 
                       properties : {}} ]
      };
      var geojson_format = new OpenLayers.Format.GeoJSON();
      var styles = new OpenLayers.StyleMap({
        "default" : {
          strokeWidth: 1.0,
          strokeColor: "#666666",
          strokeOpacity: 0.1,
          pointRadius: 3.0, 
          fillColor: "#ffcc66",
        } });
      var grid = new OpenLayers.Layer.Vector("Grid", { styleMap : styles }); 
      grid.addFeatures(geojson_format.read(grid_json));
      layers.push(grid);

      // add all layers
      map.addLayers(layers);

      // controls
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
      map.dragger.onDrag = function() {}
      map.dragger.isOnDrag = false;
      map.addControl(map.dragger);
      map.dragger.activate();

      // Aaaaaaaaaaaaaaaauuuuuuuuuuurgh - http://openlayers.org/pipermail/users/2010-May/017742.html
      map.dragger.handlers['drag'].stopDown = false;
      map.dragger.handlers['drag'].stopUp = false;
      map.dragger.handlers['drag'].stopClick = false;
      map.dragger.handlers['feature'].stopDown = false;
      map.dragger.handlers['feature'].stopUp = false;
      map.dragger.handlers['feature'].stopClick = false;

      var router_click_selector = new OpenLayers.Control.SelectFeature(map.routers, { 
        multiple : false, 
        hover    : false });
      router_click_selector.onSelect = on_select_router;
      router_click_selector.onUnselect = on_unselect_router;
      map.addControl(router_click_selector);
      router_click_selector.activate();

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
    this.center = function(longitude, latitude, zoom) {
      if (!longitude) {
        return the_map.getCenter();
      }
      if (zoom) {
        the_map.zoomTo(zoom);
      }
      the_map.panTo(LonLat(longitude, latitude));
    };


    /** 
     * add a temporary popup for feature
     */
    this.popup = function(feature, html, delay) {
      var popup = new OpenLayers.Popup.AnchoredBubble("call",
                                                      feature.geometry.getBounds().getCenterLonLat(),
                                                      new OpenLayers.Size(1, 1), 
                                                      html, 
                                                      null, 
                                                      false);
      popup.setBackgroundColor("white");
      popup.setOpacity(0.95);
      popup.autoSize = true;
      the_map.addPopup(popup);
      setTimeout(function() {
          console.log("Nuking popup");
          the_map.removePopup(popup);
          popup.destroy();
        }, (delay ? delay : 5000));
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
      the_map.dragger.isOnDrag = true;

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
      the_map.dragger.onDrag = function() {}
      the_map.dragger.isOnDrag = false;
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
      var feature = the_map.routers.getFeatureById(router.address);
      if (feature) {
        //console.log("Returning feature: " + router.address + " -> " + feature.router.address);
        //console.log(feature.router);
        return feature;
      } 
      if (!router.hasOwnProperty("routes")) { // TODO check that it's a complete router object before adding it
        console.error("Phony router, trying our best...");
        console.log(router);
        router.routes = [];
      }
      return add_router(router);
    };
    function add_router(router) {
      var feature = new OpenLayers.Feature.Vector();
      feature.style = { fillOpacity: 1.0, 
                        strokeOpacity: 1.0,
                        strokeColor: "black",
                        strokeWidth: 1.0,
                        pointRadius: 8.0 };
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
      feature.style = { strokeWidth: 7.0,
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
      good  : 1.5,
      bad   : 2.0,
      fault : 3.0
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
      if (the_map.dragger.isOnDrag) { 
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

