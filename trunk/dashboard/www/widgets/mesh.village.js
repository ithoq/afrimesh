/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

function mesh_village_nodes() {
  $("div#widget-village-nodes").html("loaded");
}


function mesh_village_configure() {
  $("div#widget-village-configure").html("loaded");
}


/* globals - TODO - make them go away! */
var epsg_4326 = new OpenLayers.Projection("EPSG:4326");
var epsg_900913 = new OpenLayers.Projection("EPSG:900913");
var site_home_lon =  18.339733;
var site_home_lat = -34.138061;
var site_home_zoom = 16;
var site_extent_width  = 0.025; /* in degrees */
var site_extent_height = 0.02; /* in degrees */
var site_extent = new OpenLayers.Bounds();
site_extent.extend(LonLat(site_home_lon - site_extent_width, site_home_lat - site_extent_height));
site_extent.extend(LonLat(site_home_lon + site_extent_width, site_home_lat + site_extent_height));
var village_map_update_interval = 7500;
var village_map_timer;
var uci_show_afrimesh = { 
  package : "uci", 
  command : "show"
  /*args    : ["afrimesh"] */
};
var village_map = undefined;

function mesh_village_map() {
  $("div#widget-village-map").html("");

  /* create the map */
  if (!village_map) {
    village_map = create_map("widget-village-map", site_home_lon, site_home_lat, site_home_zoom);
  }

  /* add the layers */
  if (!village_map.getLayersByName("Mesh Links")[0]) {
    var layer_links = new OpenLayers.Layer.Vector("Mesh Links");
    village_map.addLayer(layer_links);
  }
  if (!village_map.getLayersByName("Mesh Routers")[0]) {
    var layer_routers = new OpenLayers.Layer.Markers("Mesh Routers");
    village_map.addLayer(layer_routers);
  }

  /* invoke router positioning magick */
  if (!village_map.getLayersByName("Router Position")[0]) {
    var router_position = new OpenLayers.Layer.Vector("Router Position");
    router_position.visibility = false;
    village_map.addLayer(router_position);
  }
  var control_drag = new OpenLayers.Control.DragFeature(router_position);
  control_drag.onComplete = set_router_position;
  village_map.addControl(control_drag);
  control_drag.activate();

  /* start updating */
  update_village_map(village_map);
}




/**
 * update_village_map
 *  
 * TODO - at some point attempt: http://openlayers.org/dev/examples/spherical-mercator.html 
 */
function update_village_map(village_map) {
  clearTimeout(village_map_timer);
  
  $.ajax({ 
      url: config.batman_vis_host, 
        type: "POST", 
        data: $.toJSON(batman_vis), 
        dataType: "json",                  
      success: function(links) { 
        update_viz_data(village_map, links); 
      }
    });
  

  function callback(arg) {     // magick
    this.arg = arg;
    var me = this;
    this.invoke=function () {
      update_village_map(me.arg);
    }
  }
  var f = new callback(village_map); 
  village_map_timer = setTimeout(f.invoke, village_map_update_interval);
}



/** 
 * update_viz_data
 *
 * Query B.A.T.M.A.N. vis server
 */
function new_update_viz_data(map, links) {
  var s = "<pre>";
  var layer_links   = map.getLayersByName("Mesh Links")[0];
  var layer_routers = map.getLayersByName("Mesh Routers")[0];

  // 1. populate & update map features 
  for (link in links) {                              
    link = links[link];  // [ link.node, link.neighbour/gateway, link.label ]
    if (link.gateway) { continue; } // skip gateways for now

    var feature_link = get_link(layer_links, link);
    feature_link.attributes.lq = link.label;

    if (!feature_link.attributes.router) {
      s += "ADDING: " + feature_router.attributes.address + "\n";
      feature_link.attributes.router = get_router(layer_routers, link.router);
    }
    if (!feature_link.attributes.neighbour) {
      //feature_link.attributes.neighbour = get_router(layer_routers, link.neighbour);
    }

    // mark features
    feature_link.attributes.mark_vis = true;
    feature_link.attributes.router.attributes.mark_vis = true;
    //feature_link.attributes.neighbour.attributes.mark_vis = true;
  }


  // 2. position & display routers
  for (feature_router in layer_routers.markers) {
    feature_router = layer_routers.markers[feature_router];
    s += "2. router: " + feature_router.attributes.address + "(" + feature_router.attributes.mark_vis + ")\n";
    
    if (!feature_router.attributes.mark_vis) { s += "DIMMING: " + feature_router.attributes.address + "\n"; } // dim routers that have dropped off
    feature_router.attributes.mark_vis = false;    
  } 

  // 3. position & display links
  for (feature_link in layer_links.features) {
    feature_link = layer_links.features[feature_link];
    s += "3. link: " + feature_link.id + "\n";

    // TODO: only display feature_link if we have both sides...
    
    if (!feature_link.attributes.mark_vis) {   // remove links that no longer exist
      s += "DESTROYING: " + feature_link.id + "\n";   
      layer_links.destroyFeatures([feature_link]);
    } 
    feature_link.attributes.mark_vis = false;    
  }


  s += "</pre>";
  $("#widget-village-message").html(s);
}




/* ---------------------------------------------------------------------------------------- */


/** 
 * update_viz_data
 *
 * Query B.A.T.M.A.N. vis server
 */
function update_viz_data(map, links) {
  var layer_links   = map.getLayersByName("Mesh Links")[0];
  var layer_routers = map.getLayersByName("Mesh Routers")[0];
  
  // 1. populate & update map features 
  for (link in links) {                              
    link = links[link];  // [ link.node, link.neighbour/gateway, link.label ]
    if (link.gateway) { continue; } // skip gateways for now
    
    // compare vis-links against mesh-links, creating where needed
    var feature_link = get_link(layer_links, link);
    
    // update with latest lq
    feature_link.style.strokeColor = lq_to_color(link.label + 0.0);
    feature_link.layer.drawFeature(feature_link);
    
    // do we have a node & neighbour for this feature_link ?
    var router = feature_link.attributes.router;
    if (!router) { 
      router = get_router(layer_routers, link.router);
      feature_link.attributes.router = router;
    } 
    var neighbour = feature_link.attributes.neighbour;
    if (!neighbour) {
      neighbour = get_router(layer_routers, link.neighbour);
      feature_link.attributes.neighbour = neighbour;
    }
    
    // do our node & neighbour have location info ?
    if (!router.attributes.location) {
      locate_router(router);
      continue;
    }
    if (!neighbour.attributes.location) {
      locate_router(neighbour);
      continue;
    }
    
    // does our link have location info
    if (!feature_link.attributes.location) {
      var midpoint_lon = router.lonlat.lon + ((neighbour.lonlat.lon - router.lonlat.lon) / 2.0);
      var midpoint_lat = router.lonlat.lat + ((neighbour.lonlat.lat - router.lonlat.lat) / 2.0);
      feature_link.geometry = new OpenLayers.Geometry.LineString([ new OpenLayers.Geometry.Point(router.lonlat.lon, router.lonlat.lat),
                                                                   new OpenLayers.Geometry.Point(midpoint_lon,      midpoint_lat) ]);
      feature_link.attributes.location = 1;
      feature_link.layer.drawFeature(feature_link);
      //feature_link.layer.redraw();
      var s = $("#widget-village-message").html() + "<pre>located: " + feature_link.id + "</pre>";
      $("#widget-village-message").html(s);
    }
  }
  
  // 2. position features
  // 3. remove/dim out of date features
  $("#widget-village-message").html("");
}



  /**
   * locate_router
   *
   * query router for its location
   * TODO: add a local cache ?
   */
  function locate_router(marker) {
    var node_url = config.ajax_proxy + "http://" + marker.attributes.address + "/cgi-bin/village-bus-uci";
    var s = $("#widget-village-message").html() + "<pre>locating: " + node_url + "</pre>";
    $("#widget-village-message").html(s);
    $.ajax({ 
      url: node_url, type: "POST", async: true,
      data: $.toJSON(uci_show_afrimesh), dataType: "json",
      success: function(data) {
        var s = $("#widget-village-message").html() + "<pre>located: " + marker.attributes.address + "</pre>";
        $("#widget-village-message").html(s);
        if (data.length != 1) { $("#widget-village-message").html($("#widget-village-message").html() + "<pre>no loc data</pre>"); return; }  // TODO
        var router_config = data[0];
        if (router_config.afrimesh && router_config.network.wifi.ipaddr) { // TODO - test
          marker.lonlat = LonLat(router_config.afrimesh.location.longitude, router_config.afrimesh.location.latitude);
          marker.attributes.location = marker.lonlat;
          marker.layer.redraw();

          // add a dragger! - TODO - figure out why this gets called twice
          if (!marker.attributes.dragger) {
            var dragger = new OpenLayers.Feature.Vector(Point(router_config.afrimesh.location.longitude, 
                                                              router_config.afrimesh.location.latitude));
            dragger.attributes = {};
            dragger.attributes.marker = marker;
            dragger.attributes.address = marker.attributes.address;
            marker.attributes.dragger = dragger;
            var router_position = marker.map.getLayersByName("Router Position")[0];
            router_position.addFeatures([marker.attributes.dragger]);
          }
          //var s = $("#widget-village-message").html() + "<pre>located: " + marker.attributes.address + "</pre>";
          //$("#widget-village-message").html(s);
        } else {
          $("#widget-village-message").html($("#widget-village-message").html() + "no location for: " + marker.attributes.address + "\n</pre>");
        }
      }
    });
  }


  /**
   * get_link
   *
   * search for link in mesh-links, creating where needed
   */
  function get_link(layer_links, link) {
    var id = link.router + "->" + link.neighbour;
    var feature_link = layer_links.getFeatureById(id);
    if (feature_link) {
      return feature_link;
    }

    feature_link = new OpenLayers.Feature.Vector(null, {}, {
        strokeOpacity: 0.5,
        strokeColor: "black",
        strokeWidth: 10,
        strokeDashstyle: "solid",
        strokeLinecap: "round",
        pointRadius: 0,
        pointerEvents: "visiblePainted" }); 
    feature_link.id = id;
    feature_link.geometry = new OpenLayers.Geometry.LineString([ new OpenLayers.Geometry.Point(0.0, 0.0),
                                                                 new OpenLayers.Geometry.Point(0.0, 0.0) ]);
    layer_links.addFeatures([feature_link]);
    $("#widget-village-message").html($("#widget-village-message").html() + "<pre>Made: " + feature_link.id + "</pre>"); 

    return feature_link;
  }



  /**
   * get_router
   * 
   * 3. get router from mesh-routers, creating where needed
   */
  //var router_icon = new OpenLayers.Icon('http://boston.openguides.org/markers/AQUA.png', 
  //                                      new OpenLayers.Size(10,17), new OpenLayers.Pixel(-(size.w/2), -size.h));
  function get_router(layer_routers, address) {
    for (marker in layer_routers.markers) {
      marker = layer_routers.markers[marker];
      if (marker.attributes.address == address) {
        return marker;
      }
    }

    //var feature = new OpenLayers.Feature(layer_routers, new OpenLayers.LonLat(null, null));
    var feature = new OpenLayers.Feature(layer_routers, LonLat(site_home_lon, site_home_lat));
    //var feature = new OpenLayers.Feature(layer_routers, LonLat(null, null));
    //var feature = new OpenLayers.Feature(layer_routers);

    /* - popup ------------------------------------------------ */
    feature.data.popupContentHTML =  "<div style='color:white; '><p><b style='font-size:125%;'>" + name + "</b> ";
    feature.data.popupContentHTML += "&nbsp;&nbsp;<span style='color:darkgray; font-size:75%;'><i>" + address + "</i></span></p><br/>";
    feature.data.popupContentHTML += "<p>Health: <span style='color:green;'>feeling good</span>" + 
                                     "<span style='color:darkgray; font-size:75%;'> <i>100 days</i></span></p>";
    feature.data.popupContentHTML += "<p>Capacity: <span style='color:green;'>normal</span>" + 
                                     "<span style='color:darkgray; font-size:75%;'> <i>5%</i></span></p>";
    feature.data.popupContentHTML += "<p>Internet: <span style='color:green;'>normal</span>" + 
                                     "<span style='color:darkgray; font-size:75%;'> <i>100/512 10/256</i></span></p>";
    feature.data.popupContentHTML += "<p>Location: <span style='color:darkgray;'></span></p>";
    feature.data.popupContentHTML += "</div>";
    feature.popupClass = OpenLayers.Class(OpenLayers.Popup.AnchoredBubble, {
      'autoSize': true
    }); // AutoSizeAnchoredBubble; //AutoSizeFramedCloud; //AutoSizeAnchoredBubble; //AutoSizeAnchored;
    feature.data.overflow = "auto";
    var marker = feature.createMarker();
    //var marker = new OpenLayers.Marker(LonLat(null, null)); //, router_icon.clone());
    marker.events.register("mousedown", feature, function (evt) {
      if (this.popup == null) {
        this.popup = this.createPopup(true);
        this.popup.setBackgroundColor("black");
        this.popup.setOpacity(0.9);
        layer_routers.map.addPopup(this.popup);
        this.popup.lonlat = marker.lonlat;
        this.popup.show();
      } else {
        this.popup.lonlat = marker.lonlat;
        this.popup.toggle();
      }
      currentPopup = this.popup;
      OpenLayers.Event.stop(evt);
    });
    /* - end popup --------------------------------------------------------- */

    marker.layer = layer_routers;
    marker.attributes = {}
    marker.attributes.address = address;
    marker.attributes.location = undefined;
    marker.attributes.dragger  = undefined;
    //marker.display(false);
    //marker.style.visibility = "none";
    layer_routers.addMarker(marker);
    //$("#widget-village-message").html($("#widget-village-message").html() + "<pre>added: " + marker.attributes.address + "</pre>");

    return marker;
  }


  /** 
   * create_map
   */
  function create_map(id, lon, lat, zoom) {
    var options = {
      projection: epsg_900913,
      displayProjection: epsg_4326,
      units: "m",
      numZoomLevels: 20,
      restrictedExtent: site_extent /*,
      maxResolution: 156543.0339,
      maxExtent: new OpenLayers.Bounds(-20037508, -20037508,
                                        20037508,  20037508.34) */
    };
    var map = new OpenLayers.Map(id, options);
    var osma = new OpenLayers.Layer.OSM.Osmarender("Osmarender");
    var mapnik = new OpenLayers.Layer.OSM.Mapnik("Mapnik");
    var relief = new OpenLayers.Layer.OSM.CycleMap("Relief");
    var satellite = new OpenLayers.Layer.Yahoo("Satellite", {  /* TODO - http://labs.metacarta.com/osm/over-yahoo/ */
      'type': YAHOO_MAP_SAT, 
      'sphericalMercator': true } );
    map.addLayers([relief,osma,mapnik,satellite]);
    map.addControl(new OpenLayers.Control.Attribution());
    map.addControl(new OpenLayers.Control.MousePosition());
    map.addControl(new OpenLayers.Control.LayerSwitcher());
    map.addControl(new OpenLayers.Control.ScaleLine());
    map.setCenter(LonLat(lon, lat), zoom);
    if (!map.getCenter()) { map.zoomToMaxExtent() }
    return map;
  }


  /**
   * lq_to_color
   */
  function lq_to_color(lq) {
    if (lq > 1.50) {
      return "red";
    } else if (lq > 1.25) {
      return "orange";
    } else if (lq > 1.05) {
      return "green";
    } 
    return "lightblue";
  }
  

  /**
   * util constructors
   */
  function LonLat(lon, lat) {
    return new OpenLayers.LonLat(lon, lat).transform(epsg_4326, epsg_900913); 
  }
  function Point(lon, lat) {
    return new OpenLayers.Geometry.Point(lon, lat).transform(epsg_4326, epsg_900913);
  }



/**
 * set_router_position
 */
function set_router_position(feature) {
  //var p = new OpenLayers.Geometry.Point(feature.geometry.x, feature.geometry.y).transform(epsg_900913, epsg_4326);
  var p = new OpenLayers.LonLat(feature.geometry.x, feature.geometry.y).transform(epsg_900913, epsg_4326);
  var uci_set_location = { 
    package: "uci",  
    command: "set", 
    arguments : [ { config  : "afrimesh", section : "location", option  : "longitude", value   : p.lon  },
                  { config  : "afrimesh", section : "location", option  : "latitude",  value   : p.lat  } ]
  };

  // debug output
  var s = $("div#widget-village-message").html(); 
  s += "<pre>" + feature.attributes.address + " : " + p.lon + "," + p.lat + "</pre>";
  s += "<pre>" + dump_object(uci_set_location) + "</pre>";
  $("div#widget-village-message").html(s);

  $.ajax({ 
      url: config.ajax_proxy + "http://" + feature.attributes.address + "/cgi-bin/village-bus-uci", type: "POST", 
        contentType: "application/json", dataType: "json", data: $.toJSON(uci_set_location),
        success:  
      function(data) {
        s = "";
        for (d in data) { s += dump_object(data[d]); }
        $("div#widget-village-message").html(s);
      }
    });
  
  if (feature.attributes.marker) {
    feature.attributes.marker.attributes.location = undefined; // TODO - I don't like this pattern of 'attributes' and a location sep from lonlat
    feature_link.attributes.location = undefined;
    feature_link.layer.removeFeatures([feature_link]);
  }
}

