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


#include "jsonrpc.h"


/**
 * Given a dispatch table and a request... dispatch!
 *
 * JSON/RPC response format: { "id" : 1234, "result" : "091f1a6a7e49861850f98680659510d2", "error" : null }
 */
struct json_object* jsonrpc_dispatch(struct MethodDispatch* dispatch, 
                                     const char* module, 
                                     const char* method, 
                                     struct json_object* params)
{
  /* no module specified */
  if (module == NULL) {
    return jsonrpc_error("No module specified");
  }

  /* typecheck method parameters */
  if (json_object_get_type(params) != json_type_array) { /* TODO - full version 2.0 support w/ named args */
    return jsonrpc_error("JSON/RPC expects parameters to be an array: %s.%s (%s) ", 
                         module, method, json_object_get_string(params)); 
  }

  /* resolve method, typecheck params and dispatch method to module handler */
  size_t i;
  for (i = 0; dispatch[i].module != 0; i++) {
    const char* jsonrpc_module = dispatch[i].module;
    const char* jsonrpc_method = dispatch[i].method;
    const enum json_type* jsonrpc_signature = dispatch[i].signature;
    if (strncmp(module, jsonrpc_module, strlen(jsonrpc_module)) == 0 &&
        strncmp(method, jsonrpc_method, strlen(jsonrpc_method)) == 0) {              /* resolve method   */
      if (json_object_array_length(params) != dispatch[i].numargs) {           /* typecheck params */
        return jsonrpc_error("Type error: %s.%s (%s) expected %d arguments: %s",
                             module, method,
                             params_tostring(params),
                             dispatch[i].numargs,
                             signature_tostring(jsonrpc_signature));
      }
      size_t p;
      for (p = 0; p < json_object_array_length(params) && dispatch[i].signature[p]; p++) {
        struct json_object* param = json_object_array_get_idx(params, p); 
        enum json_type param_type = jsonrpc_signature[p];
        if (json_object_get_type(param) != param_type) {
          return jsonrpc_error("Type error: %s.%s (%s) expected: %s", 
                               module, method, 
                               params_tostring(params), 
                               signature_tostring(jsonrpc_signature));
        }
      }
      /* dispatch method */
      return dispatch[i].dispatchp(method, params);
    }
  }

  /* no module handler for request */
  return jsonrpc_error("Unknown module or method name: '%s.%s'", module, method);
}


/**
 * Construct a jsonrpc error
 */
struct json_object* jsonrpc_error(const char* message, ...)
{
  static char jsonrpc_error_message[512];
  va_list args;
  va_start(args, message);
  vsnprintf(jsonrpc_error_message, 512, message, args);
  va_end(args);
  struct json_object* error = json_object_new_object();
  json_object_object_add(error, "error",  json_object_new_string(jsonrpc_error_message));
  json_object_object_add(error, "result", NULL);
  return error;
}


/**
 * Convert a json_type enumeration to a string
 */
const char* json_type_tostring(enum json_type type) 
{
  switch (type) {
  case json_type_null:    return "Null";
  case json_type_boolean: return "Boolean";
  case json_type_double:  return "Double";
  case json_type_int:     return "Integer";
  case json_type_object:  return "Object";
  case json_type_array:   return "Array";
  case json_type_string:  return "String";
  };
  return "Unknown";
}


/**
 * Convert an array of json_type's that define a method signature to a
 * string
 */
const char* signature_tostring(const enum json_type* signature) 
{
  static char buf[1024];
  size_t t;
  for (t = 0; signature[t] != 0; t++) {
    enum json_type type = signature[t];
    strcat(buf, (t == 0 ? "" : ", "));
    strcat(buf, json_type_tostring(type));
  }
  return buf;
}


const char* params_tostring(struct json_object* params)
{
  static char buf[1024];
  size_t t;
  for (t = 0; t < json_object_array_length(params); t++) {
    enum json_type type = json_object_get_type(json_object_array_get_idx(params, t));
    strcat(buf, (t == 0 ? "" : ", "));
    strcat(buf, json_type_tostring(type));
  }
  return buf;
}


/**
 * Given json object 'object' check that it is of type 'type'
 */
int json_typecheck(struct json_object* object, enum json_type type)
{
  return json_object_get_type(object) == type;
}


/**
 * Given json object 'array' check that it is an array and that all
 * elements of the array are of type 'type'
 */
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


