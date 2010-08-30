/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/** symbols you want to expose to the global namespace */
var some_public_symbol = null;

/** encapsulate definitions in an anonymous function to avoid namespace pollution */
(function() {

  some_public_symbol = function() {
  };

  var some_private_symbol = function() {
  };

})();
