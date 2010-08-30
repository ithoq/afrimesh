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


#include "villagebus.h"


/* - villagebus --------------------------------------------------------- */
vtable* villagebus_vt = 0;
object* _VillageBus = 0;
villagebus* VillageBus = 0;
object* s_villagebus_compile  = 0;
object* s_villagebus_evaluate = 0;
object* s_villagebus_error    = 0;
object* s_villagebus_json     = 0;

void villagebus_init() 
{
  s_villagebus_compile  = symbol_intern(0, 0, L"compile");
  s_villagebus_evaluate = symbol_intern(0, 0, L"evaluate");
  s_villagebus_error    = symbol_intern(0, 0, L"error");
  s_villagebus_json     = symbol_intern(0, 0, L"json");
  villagebus_vt = (vtable*)send(object_vt, s_delegated);
  send(villagebus_vt, s_addMethod, s_print, villagebus_print);
  send(villagebus_vt, s_addMethod, s_villagebus_compile,  villagebus_compile);
  send(villagebus_vt, s_addMethod, s_villagebus_evaluate, villagebus_evaluate);
  send(villagebus_vt, s_addMethod, s_villagebus_error,    villagebus_error);
  _VillageBus = send(villagebus_vt, s_allocate, 0);
  //VillageBus = send(villagebus_vt, s_allocate, sizeof(villagebus));

  // global module instance vars
  VillageBus = (villagebus*)send(_VillageBus->_vt[-1], s_allocate, sizeof(villagebus));
  VillageBus->modules = fexp_nil;
}



/**
 * Compile the parsed HTTP request
 *
 * TODO - parse ',' properly to produce nested fexp's e.g.
 *
 *   /foo/1,2,3,4          =>  ( foo ( 1 2 3 4 ) )
 *   /bar/a,b/c            =>  ( bar ( a b ) c )
 *   /bar/a,b,/c/d/,e/f    =>  ( bar ( a b ( c d ) e ) f )
 *
 */
const fexp* villagebus_compile(closure* c, villagebus* self, Request* request)
{
  if (request == NULL || request->pathname == NULL) {
    return fexp_nil;
  }
  
  // save the request context
  self->request = request;

  // compile path
  tconc* path = (tconc*)send(Tconc, s_new, fexp_nil);
  size_t index = 0;
  size_t last  = 0;
  for (index = 0; request->pathname[index] != L'\0'; index++) {
    if ((request->pathname[index] == L'/') || (request->pathname[index] == L',')) {
      string* entry;
      if (index - last == 0) {
        entry = (string*)send(String, s_new, L"/", 1);
      } else {
        entry = (string*)send(String, s_new, request->pathname + last, index - last);
      }
      path = (tconc*)send(path, s_tconc_append, entry);
      last = index + 1;
    }
  }
  string* entry = (string*)send(String, s_new, request->pathname + last, index - last);
  path = (tconc*)send(path, s_tconc_append, entry);
  fexp* expression = (fexp*)send(path, s_tconc_tconc);

  return expression;
}


/**
 *
 */
const fexp* villagebus_evaluate(closure* c, villagebus* self, const fexp* expression)
{
  if (expression == fexp_nil) {
    return expression;
  } 

  // search for name in global context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, 0, name->buffer);
  if (channel == NULL) { // I don't know you. Ignoring you for now.
    return (fexp*)send(self, s_villagebus_evaluate, message);
  }

  // search for channel in registered modules
  fexp* iter; 
  for (iter = self->modules; iter != fexp_nil; iter = (fexp*)send(iter, s_fexp_cdr)) {
    object* entry  = send(iter,  s_fexp_car);
    object* module = send(entry, s_fexp_car);
    if (channel == module) {
      object* target = send(entry, s_fexp_cdr);
      // return send(self, s_villagebus_evaluate, message);
      return (fexp*)send(target, s_villagebus_evaluate, message);
    }
  }

  return (fexp*)send(self, s_villagebus_evaluate, message);
}


// TODO - modules should inherit from villagebus, then we can do a: send(self, s_error, "foo message");
//        which will allow for custom cleanup rather than a: send(VillageBus, s_error, "foo message");
const fexp* villagebus_error(closure* c, villagebus* self, const wchar_t* format, ...)
{
  static wchar_t buffer[512];
  va_list args;
  va_start(args, format);
  size_t length = vswprintf(buffer, 512, format, args);
  va_end(args);
  object* message = send(String, s_new, buffer, length);
  object* error = (object*)fexp_nil;
  error = send(error, s_fexp_cons, message);
  error = send(error, s_fexp_cons, s_villagebus_error);
  return (fexp*)error;
}


villagebus* villagebus_print(closure* c, villagebus* self)
{
  wprintf(L"[#<VILLAGEBUS.%p> ", self);
  send(self->modules, s_print);
  wprintf(L"]");
  return self;
}

