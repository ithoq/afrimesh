/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 *
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootCommunity(parent) {
  
  var community = function() { return this.community.neighbors(); };

  /**
   * By default, returns a list of everyone.
   *
   * TODO: return 'radius' neighbors for person with 'email'
   */
  community.neighbors = function(continuation, email, radius) {
    var name = afrimesh.villagebus.Name("/@self/db/keys/person:*/details");
    name = afrimesh.villagebus.Bind(name, function(error, response) {
        if (error) return continuation(error, null); 
        if (!response) return; // no neighbors
        response.map(function(key) {
            return afrimesh.villagebus.GET(afrimesh.villagebus.Bind("/@self/db/" + key, function(error, response) {
                return continuation(error, response);
              }));
          });
      });
    name = afrimesh.villagebus.GET(name);
    return name;    
  };

  return community;
};
exports.BootCommunity = BootCommunity;
console.debug("loaded afrimesh.community.js");
