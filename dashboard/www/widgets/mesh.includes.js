/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/* - utilities ------------------------------------------------------------ */
function include(filename) {
  var head = document.getElementsByTagName('head')[0];
  script = document.createElement('script');
  script.src = filename;
  script.type = 'text/javascript';
  head.appendChild(script);
  /*var e = window.document.createElement('script');
  e.setAttribute('src', filename);
  window.document.body.appendChild(e);*/
}

/* - Bling ---------------------------------------------------------------- */
/* TODO - kak license can't use these */
/* include("js/"); */  /* http://www.netzgesta.de/corner/ */
/* include("js/"); */  /* http://www.netzgesta.de/glossy/glossy.zip */

/* - jQuery plugins ------------------------------------------------------- */
/*include("js/jquery.ui.js");*/ /* http://ui.jquery.com/download_builder/ TODO - configure a pack with only the parts I'm using */
include("javascript/jquery.json.js");
include("javascript/jquery.jeditable.js");
include("javascript/jquery.tablesorter.min.js"); /* http://tablesorter.com/docs/ */
include("javascript/jquery.sparkline.js");

/* - openstreetmaps.org --------------------------------------------------- */
/*include("http://www.openlayers.org/api/OpenLayers.js");
  include("http://www.openstreetmap.org/openlayers/OpenStreetMap.js"); */
/*include("js/openstreetmap.org/OpenLayers.js");
  include("js/openstreetmap.org/OpenStreetMap.js"); */

/* - mesh widgets --------------------------------------------------------- */
include("widgets/mesh.everything.js");
include("widgets/mesh.customers.js");
include("widgets/mesh.router.js");
include("widgets/mesh.village.js");
include("widgets/mesh.internet.js");
include("widgets/mesh.setup.js");


