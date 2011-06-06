/**
 * Afrimesh: easy management for mesh networks
 *
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

#include "mod_http.h"
#include "mod_sys.h" // for exec - TODO split into a common dependency


/* - http --------------------------------------------------------------- */
vtable* http_vt = 0;
object* _Http = 0;
http* Http = 0;
symbol* s_http = 0;
symbol* s_http_get = 0;

void http_init()
{
  http_vt = (vtable*)send(object_vt, s_delegated); 
  send(http_vt, s_addMethod, s_print, http_print);
  send(http_vt, s_addMethod, s_villagebus_evaluate, http_evaluate);
  _Http = send(http_vt, s_allocate, 0);

  // register local symbols
  s_http_get  = (symbol*)symbol_intern(0, _Http, L"get");
  send(http_vt, s_addMethod, s_http_get,  http_get);
  
  // global module instance vars
  Http = (http*)send(_Http->_vt[-1], s_allocate, sizeof(http));
  Http->delimiter   = (string*)send(String, s_new, L"/", 1); // URL delimiter


  // register module with VillageBus 
  s_http = (symbol*)symbol_intern(0, 0, L"http");
  fexp* module = (fexp*)send(Fexp, s_new, s_http, Http);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}


/**
 *
 */
const fexp* http_evaluate(closure* c, http* self, const fexp* expression)
{
  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _Http, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 
  return (fexp*)send(VillageBus, s_villagebus_error, 
                     L"mod_http has no registered handler for name: %S",
                     name->buffer);  
}



/* - GET ---------------------------------------------------------------- */

/**
 * Return result of http get request as a string
 */
const fexp* http_get(closure* c, http* self, const fexp* message)
{
  Request* request = VillageBus->request; 
  string* search = (string*)fexp_nil;
  if (request->search) {
    search = (string*)send(String, s_string_fromwchar, L"?%S", request->search);
  }

  // parse arguments
  string* url  = (string*)send(message, s_fexp_join, self->delimiter);
  if ((fexp*)search != fexp_nil) {
    url = (string*)send(url, s_string_add, search);
  }
  char* urlc = (char*)send(url, s_string_tochar);
  wprintl(L"GET /http/get/%s\n", urlc);

  // use curl for request for now  TODO - do proper async socket calls
  char* argv[4];
  argv[0] = "curl";
  argv[1] = "-s";
  argv[2] = urlc;
  argv[3] = 0;
  return exec_string(argv[0], argv);
}


/* - Global Handlers ---------------------------------------------------- */

/**
 *
 */
http* http_print(closure* c, http* self)
{
  wprintf(L"#<Http.%p>", self);
  return self;
}


