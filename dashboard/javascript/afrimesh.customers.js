/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


function BootCustomers(parent) {

  var customers = function() { return afrimesh.villagebus.radius(); };


  return customers;
};
console.debug("loaded afrimesh.customers.js");


