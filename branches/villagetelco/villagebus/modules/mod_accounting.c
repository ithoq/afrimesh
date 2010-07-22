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


#include "mod_accounting.h"
#include "mod_sys.h" // for exec_parsed - TODO split into a common dependency

/* - accounting ----------------------------------------------------------------- */
vtable*     accounting_vt = 0;
object*     _Accounting   = 0;
accounting* Accounting    = 0;
symbol*     s_accounting  = 0;
symbol*     s_accounting_gateway = 0;

void accounting_init() 
{
  accounting_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(accounting_vt, s_addMethod, s_print, accounting_print);
  send(accounting_vt, s_addMethod, s_villagebus_evaluate, accounting_evaluate);
  _Accounting = send(accounting_vt, s_allocate, 0);

  // register local symbols
  s_accounting_gateway = (symbol*)symbol_intern(0, _Accounting, L"gateway"); 
  send(accounting_vt, s_addMethod, s_accounting_gateway, accounting_gateway);

  // global module instance vars
  Accounting = (accounting*)send(_Accounting->_vt[-1], s_allocate, sizeof(accounting));

  // register module with VillageBus
  s_accounting = (symbol*)symbol_intern(0, 0, L"accounting");
  fexp* module = (fexp*)send(Fexp, s_new, s_accounting, Accounting);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}


const fexp* accounting_evaluate(closure* c, accounting* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _Accounting, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 

  return (fexp*)send(VillageBus, 
                     s_villagebus_error, 
                     L"mod_accounting has no registered handler for requested name: %S", 
                     name->buffer);  
}


accounting* accounting_print(closure* c, accounting* self)
{
  wprintf(L"#<ACCOUNTING.%p>", self);
  return self;
}



/* - names -------------------------------------------------------------- */

/**
 *
 */
const fexp* accounting_gateway(closure* c, accounting* self, const fexp* message)
{
  char* argv[3];
  argv[0] = "pmacct";
  argv[1] = "-s";
  argv[2] = 0;
  return exec_parsed(argv[0], argv, pmacct_exec_parser);
}



/* - utilities ---------------------------------------------------------- */
struct json_object* pmacct_exec_parser(const char* line, size_t length)
{
  ((char*)line)[length - 1] = '\0';   // TODO - supremely evil - fix when there's time to debug

  /* parse header */
  static struct json_object* header_fields = NULL;
  if (header_fields == NULL) {
    header_fields = json_object_new_array();
    char* field = strdup(line);
    while (field = strtok(field, " ")) {
      json_object_array_add(header_fields, json_object_new_string(field));
      field = NULL;
    }
    return NULL;
  }

  /* lose junk lines */
  if (length <= 1 ||
      strncasecmp(line, "For a total", 11) == 0) {
    return NULL;
  }
  
  /* parse line */
  struct json_object* entry = json_object_new_object();
  char*  field = strdup(line);
  size_t count = 0;
  while ((field = strtok(field, " ")) && 
         count < json_object_array_length(header_fields)) {
    struct json_object* field_name = json_object_array_get_idx(header_fields, count);
    json_object_object_add(entry,
                           json_object_get_string(field_name),  
                           json_object_new_string(field));
    field = NULL;
    count++;
  }

  return entry;
}

