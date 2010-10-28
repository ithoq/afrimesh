/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "mod_package.h"
#include "mod_sys.h" // for exec_parsed - TODO split into a common dependency


/* - package ----------------------------------------------------------------- */
vtable*  package_vt = 0;
object*  _Package   = 0;
package* Package    = 0;
symbol*  s_package  = 0;
symbol*  s_package_update  = 0;
symbol*  s_package_list    = 0;
symbol*  s_package_status  = 0;
symbol*  s_package_upgrade = 0;


void package_init() 
{
  package_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(package_vt, s_addMethod, s_print, package_print);
  send(package_vt, s_addMethod, s_villagebus_evaluate, package_evaluate);
  _Package = send(package_vt, s_allocate, 0);

  // register local symbols
  s_package_update  = (symbol*)symbol_intern(0, _Package, L"update"); 
  s_package_list    = (symbol*)symbol_intern(0, _Package, L"list"); 
  s_package_status  = (symbol*)symbol_intern(0, _Package, L"status"); 
  s_package_upgrade = (symbol*)symbol_intern(0, _Package, L"upgrade"); 
  send(package_vt, s_addMethod, s_package_update,  package_update);
  send(package_vt, s_addMethod, s_package_list,    package_list);
  send(package_vt, s_addMethod, s_package_status,  package_status);
  send(package_vt, s_addMethod, s_package_upgrade, package_upgrade);

  // global module instance vars
  Package = (package*)send(_Package->_vt[-1], s_allocate, sizeof(package));

  // register module with VillageBus
  s_package = (symbol*)symbol_intern(0, 0, L"package");
  fexp* module = (fexp*)send(Fexp, s_new, s_package, Package);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}


const fexp* package_evaluate(closure* c, package* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _Package, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 

  return (fexp*)send(VillageBus, 
                     s_villagebus_error, 
                     L"mod_package has no registered handler for requested name: %S", 
                     name->buffer);  
}


package* package_print(closure* c, package* self)
{
  wprintf(L"#<PACKAGE.%p>", self);
  return self;
}



/* - names -------------------------------------------------------------- */

/**
 *
 */
const fexp* package_update(closure* c, package* self, const fexp* message)
{
  char* argv[3];
  argv[0] = "opkg";
  argv[1] = "update";
  argv[2] = 0;
  return exec(argv[0], argv);
}


/**
 *
 */
const fexp* package_list(closure* c, package* self, const fexp* message)
{ 
  char* argv[3];
  argv[0] = "opkg";
  argv[1] = "list_upgradable";
  argv[2] = 0;
  return exec_parsed(argv[0], argv, ipkg_list_exec_parser);
}


/**
 *
 */
const fexp* package_status(closure* c, package* self, const fexp* message)
{
  return fexp_nil;
}


/**
 *
 */
const fexp* package_upgrade(closure* c, package* self, const fexp* message)
{
  // get parameters
  Request* request = VillageBus->request;
  char* name = (char*)send(send(message, s_fexp_car), s_string_tochar);
  if (request->json) {
    const char* param = json_object_get_string(json_object_object_get(request->json, "name"));
    name = param ? (char*)param : name;
  }

  char* argv[4];
  argv[0] = "opkg";
  argv[1] = "upgrade";
  argv[2] = name;
  argv[3] = 0;
  return exec(argv[0], argv);
}



/* - utilities ---------------------------------------------------------- */
struct json_object* ipkg_list_exec_parser(const char* line, size_t length)
{
  char* name;
  char* version;
  char* comment;
  const char* cursor = line;
  cursor = parse_field(cursor, length, (char[]){' ',-1}, &name) + 2;
  cursor = parse_field(cursor, length, (char[]){' ',-1}, &version) + 2;
  cursor = parse_field(cursor, length, (char[]){'\n',-1}, &comment);
  struct json_object* package = json_object_new_object();
  json_object_object_add(package, "name", json_object_new_string(name));
  json_object_object_add(package, "version", json_object_new_string(version));
  json_object_object_add(package, "comment", json_object_new_string(comment));
  return package;
}
