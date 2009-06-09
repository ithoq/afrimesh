/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

var populate_dom = null;
var populate_select_interface = null;

(function() {


  /** populate DOM with the values in afrimesh.settings ------------------- */
  populate_dom = function() {
    var elements = $("[id*=afrimesh|settings]");
    elements.each(function(element) { 
        element = elements[element];
        if (element instanceof HTMLSelectElement) { // TODO - handle select elements
        } else if (element instanceof HTMLInputElement) {
          $(element).val(Q(afrimesh, element.id) + ""); 
        } else if (element instanceof HTMLElement) {
          $(element).html(Q(afrimesh, element.id) + ""); 
        } else {
          console.warn("UNKNOWN ELEMENT TYPE: " + element);
        }
      });
  };


    /** query internet gateway's interfaces via snmp ---------------------- */
    populate_select_interface = function() {
      $("input.[id*=afrimesh|settings|internet_gateway|address]").css("background", "#FFAAAA");
      var interfaces = afrimesh.villagebus.snmp.walk(afrimesh.settings.internet_gateway.address, 
                                                     afrimesh.settings.internet_gateway.snmp.community, 
                                                     [ ".1.3.6.1.2.1.2.2.1.2" ]);  // IF-MIB::ifDescr
      if (!interfaces || interfaces.error) {
        console.debug("GOT SNMP ERROR: " + interfaces.error);         // TODO display error in UI
        $("select.[id*=afrimesh|settings|internet_gateway|snmp|interface]").html("");
        return;
      }
      $("input.[id*=afrimesh|settings|internet_gateway|address]").css("background", "#AAFFAA");
      var interface_current = afrimesh.settings.internet_gateway.snmp.interface;
      var interface_count = 1;
      var options = "";
      for (var interface in interfaces) {
        options += "<option value='" + interface_count + "' ";
        options += (interface_count == interface_current ? "selected" : "") + ">";
        options += interfaces[interface];
        options += "</option>";
        interface_count++;
      }
      $("select.[id*=afrimesh|settings|internet_gateway|snmp|interface]").html(options);

    }


})();
