/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootCustomers(parent) {

  var customers = function(continuation) { return this.customers.status(continuation); };

  customers.status  = function(continuation) {
    var name = afrimesh.villagebus.Name("/@radius");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.POST(name, { command : "who" }, null, true);
    return name;
  };  

  customers.select = function(continuation) {
    var name = afrimesh.villagebus.Name("/@radius");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.POST(name, { command : "list" }, null, true);
    return name;
  };

  customers.generate = function(username, type, seconds, continuation) { 
    var name = afrimesh.villagebus.Name("/@radius");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.POST(name, { command  : "new",
                                            username : username, 
                                            type     : type,
                                            seconds  : seconds }, null, true);    
    return name;
  };

  customers.update = function(username, new_username, new_password, new_type, continuation) { 
    var name = afrimesh.villagebus.Name("/@radius");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.POST(name, { command      : "modify",
                                            username     : username, 
                                            new_username : new_username,
                                            new_password : new_password,
                                            new_type     : new_type }, null, true);    
    return name;
  };

  customers.update.username = function(username, new_username, continuation) { 
    return customers.update(username, new_username, null, null, continuation);
  };

  customers.update.type = function(username, new_type, continuation) { 
    return customers.update(username, null, null, new_type, continuation);
  };

  customers.remove = function(username, continuation) { 
    var name = afrimesh.villagebus.Name("/@radius");
    name = afrimesh.villagebus.Bind(name, continuation);
    name = afrimesh.villagebus.POST(name, { command  : "delete",
                                            username : username }, null, true);    
    return name;
  };

  return customers;
};
exports.BootCustomers = BootCustomers;
console.debug("loaded afrimesh.customers.js");


