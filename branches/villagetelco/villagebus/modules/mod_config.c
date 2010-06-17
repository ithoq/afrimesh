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


#include <uci.h> 

#include "mod_config.h"


/* - config ----------------------------------------------------------------- */
struct vtable* config_vt = 0;
object* Config = 0;
object* s_config = 0;


void config_init() 
{
  config_vt = (struct vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(config_vt, s_addMethod, s_print, config_print);
  send(config_vt, s_addMethod, s_villagebus_evaluate, config_evaluate);
  Config = send(config_vt, s_allocate, 0);
  ((config*)Config)->context = NULL;
  
  // register module with VillageBus
  s_config = symbol_intern(0, 0, L"config");
  fexp* module = (fexp*)send(Fexp, s_new, s_config, Config);
  ((villagebus*)VillageBus)->modules = (fexp*)send(((villagebus*)VillageBus)->modules, s_fexp_cons, module);

  // delimiter used for composing uci key names
  ((config*)Config)->delimiter = (string*)send(String, s_new, L".", 1);
}


const fexp* config_evaluate(struct closure* closure, config* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure
  // debug
  printf("config_evaluate: ");
  send(expression, s_print);
  printf("\n");

  // evaluate request 
  const Request* request = ((villagebus*)VillageBus)->request;
  switch (request->method) {
  case POST:
    expression = config_post(closure, self, expression, request->data);
    break;
  case GET:
    expression = config_get(closure, self, expression);
    break;
  default:
    printf("Config module has no registered handler for request method: %d\n", request->method);
    return fexp_nil;
  }

  return fexp_nil;
}


const fexp* config_post(struct closure* closure, config* self, const fexp* expression, const unsigned char* data)
{
  //printf("POST %s %s\n", key, value);
  return fexp_nil;
}


const fexp* config_get (struct closure* closure, config* self, const fexp* expression)
{
  string* key  = (string*)send(expression, s_fexp_join, self->delimiter);  // generate a key from expression
  char*   keyc = (char*)send(key, s_string_tochar); // TODO - uci not support UNICODE so much
  printf("GET %s\n", keyc);
  //self->context 
  return fexp_nil;
}


config* config_print(struct closure* closure, config* self)
{
  wprintf(L"#<CONFIG.%p>", self);
  return self;
}




/* - uci utilities ------------------------------------------------------ */
int uci_show(struct uci_context* context, const char* package)
{
  if (package && strcasecmp(package, "") != 0) {
    int num_sections = uci_show_package(context, package);
  } else {      
    char** packages = NULL;
    if ((uci_list_configs(context, &packages) != UCI_OK) || !packages) {
      // TODO - error handling
      uci_perror(context, "village-bus-uci"); 
      return EXIT_FAILURE;
    }
    bool first_package = true;      
    char** p;      
    for (p = packages; *p; p++) {
      printf("%s\n", (first_package ? "" : ",")); 
      int num_sections = uci_show_package(context, *p);
      if (first_package && (num_sections == 0)) { 
        first_package = true;
      } else {
        first_package = false;
      }
      printf("Queried package: %s -> %d\n", *p, num_sections);
    }
  }
  return 0;
}


int uci_show_package(struct uci_context* context, const char* package)
{

  return 0;
}

