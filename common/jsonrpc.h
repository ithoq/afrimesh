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

#ifndef JSON_RPC
#define JSON_RPC

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <json/json.h>


/* TODO */
/* jsonrpc_register(const char* module, const char* name, const char** arguments, void* function); */

/* method dispatch */
struct MethodDispatch {
  const char* module;
  const char* method;
  enum json_type signature[8];
  int numargs;
  struct json_object* (*dispatchp) (const char*, struct json_object*);
};
struct json_object* jsonrpc_dispatch(struct MethodDispatch* dispatch, 
                                     const char* module, 
                                     const char* method, 
                                     struct json_object* params);
struct json_object* jsonrpc_error(const char* message, ...);

/* simple type checker */
int json_typecheck(struct json_object* object, enum json_type type);
int json_typecheck_array(struct json_object* array, enum json_type type);

/* utilities */
const char* json_type_tostring(enum json_type type);
const char* signature_tostring(const enum json_type* signature);
const char* params_tostring(struct json_object* params);


#endif /* JSON_RPC */

