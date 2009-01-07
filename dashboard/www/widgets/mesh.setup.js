/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function mesh_setup_router() {

  $("div#widget-village-map").html("");

  if (!village_map) {
    village_map = create_map("widget-village-map", site_home_lon, site_home_lat, site_home_zoom);
  }

  /* invoke router positioning magick */
  if (!village_map.getLayersByName("Router Position")[0]) {
    var router_position = new OpenLayers.Layer.Vector("Router Position");
    village_map.addLayer(router_position);
  }

  // TODO - add an ip address so that set_router_position can update it!
  var router = new OpenLayers.Feature.Vector(Point(site_home_lon, site_home_lat));
  router.attributes = {};
  router.attributes.address = "TODO"; 
  router_position.addFeatures([router]);
  var control_drag = new OpenLayers.Control.DragFeature(router_position);
  control_drag.onComplete = set_router_position;
  village_map.addControl(control_drag);
  control_drag.activate();
}


