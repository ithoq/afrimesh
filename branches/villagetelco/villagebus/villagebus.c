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


/* - external wrappers -------------------------------------------------- */

/**
 * Compile the parsed HTTP request
 */
const fexp* compile(const Request* request) 
{
  if (request == NULL || request->pathname == NULL) {
    return fexp_nil;
  }
  
  // save the request context
  ((villagebus*)VillageBus)->request = request;

  // compile path
  tconc* path = (tconc*)send(Tconc, s_new, fexp_nil);
  size_t index = 0;
  size_t last  = 0;
  for (index = 0; request->pathname[index] != '\0'; index++) {
    if (request->pathname[index] == '/') {
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
  object* entry = send(String, s_new, request->pathname + last, index - last);
  path = (tconc*)send(path, s_tconc_append, entry);
  fexp* expression = (fexp*)send(path, s_tconc_tconc);

  return expression;
}


/**
 * Dispatch wrapper
 */ 
const fexp* evaluate(const fexp* expression)
{
  return (fexp*)send(VillageBus, s_villagebus_evaluate, expression);  
}



/* - villagebus --------------------------------------------------------- */
struct vtable* villagebus_vt = 0;
object* VillageBus = 0;
object* s_villagebus_evaluate = 0;

void villagebus_init() 
{
  s_villagebus_evaluate = symbol_intern(0, 0, L"evaluate");
  villagebus_vt = (struct vtable*)send(object_vt, s_delegated);
  send(villagebus_vt, s_addMethod, s_print, villagebus_print);
  send(villagebus_vt, s_addMethod, s_villagebus_evaluate, villagebus_evaluate);
  VillageBus = send(villagebus_vt, s_allocate, 0);
  ((villagebus*)VillageBus)->modules = fexp_nil;
}


const fexp* villagebus_evaluate(struct closure* closure, villagebus* self, const fexp* expression)
{
  if (expression == fexp_nil) {
    return expression;
  } 

  // evaluate expression
  string* channel = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);

  printf("evaluate:   ");
  send(channel, s_print);
  printf(" <- ");
  send(message, s_print);
  printf("\n");
  
  // search for name in global context
  object* name = symbol_lookup(0, 0, channel->buffer);
  if (!name) { // I don't know you. Ignoring you for now.
    return (fexp*)send(self, s_villagebus_evaluate, message);
  }

  // search for name in registered modules
  fexp* iter; 
  for (iter = self->modules; iter != fexp_nil; iter = (fexp*)send(self->modules, s_fexp_cdr)) {
    object* entry  = send(iter,  s_fexp_car);
    object* module = send(entry, s_fexp_car);
    if (name == module) {
      object* target = send(entry, s_fexp_cdr);
      // return send(target, s_villagebus_evaluate, message);
      message = (fexp*)send(target, s_villagebus_evaluate, message);
      printf("Module returned: ");
      send(message, s_print);
      printf("\n");
      return message;
    }
  }

  return (fexp*)send(self, s_villagebus_evaluate, message);
}


villagebus* villagebus_print(struct closure* closure, villagebus* self)
{
  wprintf(L"[#<VILLAGEBUS.%p> ", self);
  send(self->modules, s_print);
  wprintf(L"]");
  return self;
}

