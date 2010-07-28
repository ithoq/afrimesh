/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootPerson(parent) {

  var person = {}; // function(id, continuation) { return this.person.load(id, continuation); };

  person.save = function(person, continuation) {
    
    console.log("SAVING: " + show(person));
    if (!person ||
        (!person.email     || person.email     == "") ||
        (!person.firstname || person.firstname == "") ||
        (!person.lastname  || person.latname   == "")) {
      return continuation(error, "Specify all the user details please.");
    }

    // 1. look for a person with this email
    var name = afrimesh.villagebus.Bind("/@root/db/person:" + person.email + ":id", function(error, id) {
        console.log("/@root/db/person:" + person.email + ":id -> (" + error + ", " + id + ")");
        if (error) return continuation(error, id);
        if (!id) {
          console.error("NO ID");
        } else {
          console.error("HAVE ID");
        }

      });
    name = afrimesh.villagebus.GET(name);

    // 2. generate a person id if there is no record associated with this email
    // <id> = incr person:id
    // sadd <id> person:ids    TODO
    function generate_id(person, continuation) {
      var name = afrimesh.villagebus.Bind("/@root/db/incr/person:id", function(error, id) {
          person.id = id;
          return continuation(error, person); 
        });
      return afrimesh.villagebus.GET(name);
    };

    // 3. link person id & email
    // set person:<email>:id id
    function link_details(person, continuation) {
      var name = afrimesh.villagebus.Bind("/@root/db/person:" + person.email + ":id", continuation);
      return afrimesh.villagebus.PUT(name);
    };

    // 4. save the person details
    // set person:<id>:details = { id, email, firstname, lastname };
    function update_details(person, continuation) {
      var name = afrimesh.villagebus.Bind("/@root/db/person:" + person.id + ":details", continuation);
      return afrimesh.villagebus.PUT(name);
    };

  };

  person.load = function(email) {
  };

  /*
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
  };*/

  return person;
};
console.debug("loaded afrimesh.person.js");


