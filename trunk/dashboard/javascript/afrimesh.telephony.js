/*
 * Afrimesh: easy management for mesh networks
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

function BootTelephony(parent) {

  var telephony = {}; 


  /* - asterisk/sip ----------------------------------------------------- */
  telephony.sip = {};

  /**
   * sip 
   */
  telephony.sip = function(address, command, continuation) {
    var name = afrimesh.villagebus.Name("/@" + address + "/telephony/sip/" + command);
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); // TODO - return Fail(error, continuation) maybe ?
        return continuation(error, response[0]);
      });
    name = afrimesh.villagebus.GET(name);
    return name;
  };
  telephony.sip.peers = function(address, continuation) {
    return telephony.sip(address, "peers", continuation);
  };
  
  return telephony;
};
exports.BootTelephony = BootTelephony;
console.debug("loaded afrimesh.telephony.js");


