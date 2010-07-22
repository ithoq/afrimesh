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

#include <credis.h>

#include "mod_provision.h"


/* - provision ---------------------------------------------------------- */
vtable* provision_vt = 0;
object* _Provision = 0;
provision* Provision  = 0;
symbol* s_provision = 0;
symbol* s_provision_mac = 0;
symbol* s_provision_ip  = 0;

void provision_init()
{
  provision_vt = (vtable*)send(object_vt, s_delegated); 
  send(provision_vt, s_addMethod, s_print, provision_print);
  send(provision_vt, s_addMethod, s_villagebus_evaluate, provision_evaluate);
  _Provision = send(provision_vt, s_allocate, 0);

  // register local symbols
  s_provision_mac = (symbol*)symbol_intern(0, _Provision, L"mac"); 
  s_provision_ip  = (symbol*)symbol_intern(0, _Provision, L"ip");
  send(provision_vt, s_addMethod, s_provision_mac, provision_mac);
  send(provision_vt, s_addMethod, s_provision_ip,  provision_ip);

  // global module instance vars
  Provision = (provision*)send(_Provision->_vt[-1], s_allocate, sizeof(provision));
  Provision->handle = NULL;
  Provision->delimiter = (string*)send(String, s_new, L":", 1); // TODO - use mod_db's delimiter?

  // register module with VillageBus 
  s_provision = (symbol*)symbol_intern(0, 0, L"provision");
  fexp* module = (fexp*)send(Fexp, s_new, s_provision, Provision);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}



/**
 *
 */
const fexp* provision_evaluate(closure* c, provision* self, const fexp* expression)
{
  const fexp* reply = fexp_nil;

  // lazily initialize redis connection
  if ((self->handle == NULL) || (credis_ping(self->handle) != 0)) {
    self->handle = credis_connect("localhost", 6379, 2000);
  }
  
  // check connection
  if ((self->handle == NULL) || (credis_ping(self->handle) != 0)) {
    // TODO - try to start up redis server
    reply = (fexp*)send(VillageBus, s_villagebus_error, L"Could not connect to redis server");
    goto done;
  }

  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _Provision, name->buffer);
  if (channel) {
    reply = (fexp*)send(self, channel, message);
  } else {
    reply = (fexp*)send(VillageBus, s_villagebus_error, 
                        L"mod_provision has no registered handler for name: %S",
                        name->buffer);  
  }
  
  // close server connection & release resources
 done:   
  if (self->handle != NULL) {
    credis_close(self->handle);
    self->handle = NULL;
  }
  
  return reply;
}


/* - GET ---------------------------------------------------------------- */

/**
 *
 */
const string* provision_mac(closure* c, provision* self, const fexp* message)
{
  return (string*)fexp_nil;
}


 
/**
 *
 */
const string* provision_ip (closure* c, provision* self, const fexp* message)
{
  return (string*)fexp_nil;
}



/*const fexp* provision_keys(closure* c, provision* self, const fexp* message)
{
  fexp* reply = fexp_nil;

  string* key  = (string*)send(message, s_fexp_join, self->delimiter);  // generate a key from message
  wprintl(L"GET /provision/keys/%S\n", key->buffer);
  char*  keyc = (char*)send(key, s_string_tochar); // TODO - redis not support UNICODE so much

  char** bufferv;
  int i;
  for (i = 0; i < credis_keys(self->handle, keyc, &bufferv); i++) {
    if (strcmp(bufferv[i], "") == 0) continue; // getting an empty key on zero match result :-/
    string* item = (string*)send(String, s_string_fromchar, bufferv[i], strlen(bufferv[i]));
    reply = (fexp*)send(reply, s_fexp_cons, item);
  }
  free(keyc);

  return reply;
  }*/




/* - Global Handlers ---------------------------------------------------- */

/**
 *
 */
provision* provision_print(closure* c, provision* self)
{
  wprintf(L"#<Provision.%p>", self);
  return self;
}
