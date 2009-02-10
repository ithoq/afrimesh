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
  function _Map(id, longitude, latitude, extent, zoom) {

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
      var selector = new OpenLayers.Control.SelectFeature(map.routers);
      selector.onSelect = on_select;
      map.addControl(selector);
      selector.activate();
      return map;
    };


    /** 
     * Return the map feature representing the given router, creating it and 
     * adding it to the map if it doesn't exist yet.
     */
    this.router = function(router) {
      var feature = the_map.routers.getFeatureById(router.address);
      if (feature) {
        return feature;
      }
      return add_router(router);
    };
    function add_router(router) {
      var feature = new OpenLayers.Feature.Vector();
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
    this.route = function(route) {
      if (route.gateway) { return {}; } // skip gateways for now
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
      feature.id = route.router + "->" + route.neighbour;
      feature.route = route;
      feature.geometry = new OpenLayers.Geometry.LineString([ feature_origin.geometry, feature_destination.geometry ]);
      the_map.routes.addFeatures([feature]);
      //console.debug("Added route for route: " + (route.router + "->" + route.neighbour));
      return feature;
    };
    

    function on_select(feature) {
      console.log("on_select: " + dump_object(feature.router));
    };

    function on_position(feature) {
      console.log("on_position: " + dump_object(feature.router));
      // update router location config

      // update route geometry
      feature.router.routes.map(function(route) {
          if (route.gateway) { return; } // skip gateways for now
          console.debug("Updating route geometry for: " + (route.router + "->" + route.neighbour));
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

