/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/**
 * includes
 */
//load("javascript/openlayers/OpenLayers.js");
//load("javascript/openlayers/OpenStreetMap.js");
if (afrimesh.settings.map.google == "1") {
  //load("http://maps.google.com/maps/api/js?sensor=false"); // TODO - figure out why not working
}


/** 
 * OpenLayers configuration
 */
OpenLayers.ImgPath = "images/";

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


/**
 * Class: OpenLayers.Layer.OSM.LocalMap
 *
 * Inherits from:
 *  - <OpenLayers.Layer.OSM>
 */
OpenLayers.Layer.OSM.LocalMap = OpenLayers.Class(OpenLayers.Layer.OSM, {
    /**
     * Constructor: OpenLayers.Layer.OSM.LocalMap
     *
     * Parameters:
     * name - {String}
     * url - {String}
     * options - {Object} Hashtable of extra options to tag onto the layer
     */
    initialize: function(name, url, options) {
      url = [ url ];
      options = OpenLayers.Util.extend({ numZoomLevels: 20 }, options);
      var newArguments = [ name, url, options ];
      OpenLayers.Layer.OSM.prototype.initialize.apply(this, newArguments);
    },
    CLASS_NAME: "OpenLayers.Layer.OSM.LocalMap"
});


console.debug("loaded afrimesh.maps.js");
