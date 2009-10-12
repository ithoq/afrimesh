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


#include "json_rpc.h"


struct Module jsonrpc_modules[] = {
  { "/uci",  4, jsonrpc_dispatch_uci  },
  { "/snmp", 5, jsonrpc_dispatch_snmp },
  { 0, 0, 0 }
};

/**
 * Handle request dispatch for uci module
 */
struct json_object* jsonrpc_dispatch_uci(const char* name, struct json_object* arguments)
{
  log_message("dispatching uci request: %s(%s)\n", name, json_object_get_string(arguments));
  return NULL;
}


/**
 * Handle request dispatch for snmp module
 */
struct json_object* jsonrpc_dispatch_snmp(const char* name, struct json_object* arguments)
{
  size_t i;
  struct snmp_session* session;

  // typecheck request arguments
  if (json_object_array_length(arguments) != 3) {
    return jsonrpc_error("snmp.%s expects %d arguments - got: %s", name, 3, json_object_get_string(arguments));
  } 
  struct json_object* address   = json_object_array_get_idx(arguments, 0);
  struct json_object* community = json_object_array_get_idx(arguments, 1);
  struct json_object* oids      = json_object_array_get_idx(arguments, 2);
  if (!json_typecheck(address,   json_type_string)  ||
      !json_typecheck(community, json_type_string)  ||
      !json_typecheck(address,   json_type_string)  ||
      !json_typecheck_array(oids, json_type_string) ||
      json_object_array_length(oids)  <  1) {
    return jsonrpc_error("snmp.%s expects arguments of type (string, string, [string]) - got: %s", 
                         name, json_object_get_string(arguments));
  }
  //log_message("dispatching snmp request: %s(%s)\n", name, json_object_get_string(arguments));

  // initialize snmp
  session = snmp_start(json_object_get_string(address), json_object_get_string(community));
  if (!session) {
    int liberr, syserr;
    char* errstr;
    snmp_error(session, &liberr, &syserr, &errstr);
    log_message("Failed to initialize net-snmp: %s\n", errstr);
    return jsonrpc_error("Failed to initialize net-snmp: %s", errstr);
  }

  // dispatch request
  struct json_object* response = json_object_new_object();
  if (strncmp(name, "get", 3) == 0) {
    struct json_object* get = snmp_get(session, oids);
    if (!get) {
      int liberr, syserr;
      char* errstr;
      snmp_error(session, &liberr, &syserr, &errstr);
      return jsonrpc_error("net-snmp error: %s", errstr);      
    }
    json_object_object_add(response, "result", get);

  } else if (strncmp(name, "walk", 4) == 0) {
    struct json_object* result = json_object_new_array(); // TODO - do we really have to accept&return an array?
    for (i = 0; i < json_object_array_length(oids); i++) {
      struct json_object* walk = snmp_walk(session, json_object_get_string(json_object_array_get_idx(oids, i)));
      if (!walk) {
        int liberr, syserr;
        char* errstr;
        snmp_error(session, &liberr, &syserr, &errstr);
        return jsonrpc_error("net-snmp error: %s", errstr);
      }
      json_object_array_add(result, walk);
    }
    json_object_object_add(response, "result", result);

  } else {
    return jsonrpc_error("Could not find name '%s' in module '/snmp'", name);
  }

  json_object_object_add(response, "error",  NULL);

  // cleanup
  snmp_stop(session);

  return response;
}


/**
 * Dispatch request to module handler
 *
 * JSON/RPC response format: { "id" : 1234, "result" : "091f1a6a7e49861850f98680659510d2", "error" : null }
 */
struct json_object* jsonrpc_dispatch(const char* module, const char* name, struct json_object* arguments)
{
  /* no module specified */
  if (module == NULL) {
    return jsonrpc_error("No module specified");
  }
  
  /* dispatch name to module handler */
  size_t i;
  for (i = 0; jsonrpc_modules[i].module != 0; i++) {
    if (strncmp(module, jsonrpc_modules[i].module, jsonrpc_modules[i].length) == 0) {
      //log_message("%s %s %d %p\n", module, jsonrpc_modules[i].module, jsonrpc_modules[i].length, jsonrpc_modules[i].dispatchp);
      if (json_object_get_type(arguments) != json_type_array) { // TODO - full version 2.0 support w/ named args
        return jsonrpc_error("Module %s call to %s has invalid arguments: %s ", module, name, json_object_get_string(arguments)); 
      }
      return jsonrpc_modules[i].dispatchp(name, arguments);
    }
  }

  /* no module handler for request */
  return jsonrpc_error("Unknown module: %s", module);
}


char jsonrpc_error_message[512];
struct json_object* jsonrpc_error(const char* message, ...)
{
  va_list args;
  va_start(args, message);
  vsnprintf(jsonrpc_error_message, 512, message, args);
  va_end(args);
  struct json_object* error = json_object_new_object();
  json_object_object_add(error, "error",  json_object_new_string(jsonrpc_error_message));
  json_object_object_add(error, "result", NULL);
  return error;
}


int json_typecheck(struct json_object* object, enum json_type type)
{
  return json_object_get_type(object) == type;
}


int json_typecheck_array(struct json_object* array, enum json_type type)
{
  if (json_object_get_type(array) != json_type_array) {
    return FALSE;
  }
  size_t i, length = json_object_array_length(array);
  for (i = 0; i < length; i++) {
    if (!json_typecheck(json_object_array_get_idx(array, i), type)) {
      return FALSE;
    }
  }
  return TRUE;
}
