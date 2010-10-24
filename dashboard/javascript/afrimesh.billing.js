/*
 * Afrimesh: easy management for mesh networks
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootBilling(parent) {

  var billing = {}; 

  /* - a2billing / a3glue ----------------------------------------------- */
  billing.voip = function(address, continuation) {
    var name = afrimesh.villagebus.Name("/@a3glue/a3g_srv.php?field=ipaddress&value=" + 
                                        address + "&mode=1");
    //var name = afrimesh.villagebus.Name("/@a3glue");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.jsonp_to_json(name, null, true); // a3glue does not support JSONP
    name = afrimesh.villagebus.GET(name);
    return name;
  };

  return billing;
};
exports.BootBilling = BootBilling;
console.debug("loaded afrimesh.billing.js");

