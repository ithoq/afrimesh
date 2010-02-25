/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootCustomers(parent) {

  var customers = function(callback) { return this.customers.status(callback); };

  // TODO - use fn assignment instead where possible?
  customers.select = function(callback) {
    return afrimesh.villagebus.radius.select(callback);
  }
  customers.generate = function(username, type, seconds, callback) { 
    return afrimesh.villagebus.radius.insert(username, type, seconds, callback);
  };
  customers.update = function(username, new_username, new_password, new_type, callback) { 
    return afrimesh.villagebus.radius.update(username, new_username, new_password, new_type. callback); 
  };
  customers.update.username = function(username, new_username, callback) { 
    return afrimesh.villagebus.radius.update(username, new_username, null, null, callback); 
  };
  customers.update.type = function(username, new_type, callback) { 
    return afrimesh.villagebus.radius.update(username, null, null, new_type, callback); 
  };
  customers.remove = function(username, callback) { 
    return afrimesh.villagebus.radius.remove(username, callback); 
  };
  customers.status  = function(callback) {
    return afrimesh.villagebus.radius.who(callback);
  };  

  return customers;
};
console.debug("loaded afrimesh.customers.js");


