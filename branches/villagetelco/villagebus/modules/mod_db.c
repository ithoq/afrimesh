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

#include "mod_db.h"


/* - db ----------------------------------------------------------------- */
struct vtable* db_vt = 0;
object* DB = 0;
object* s_db = 0;


void db_init() 
{
  db_vt = (struct vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(db_vt, s_addMethod, s_print, db_print);
  send(db_vt, s_addMethod, s_villagebus_evaluate, db_evaluate);
  DB = send(db_vt, s_allocate, 0);
  ((db*)DB)->handle = NULL;
  
  // register module with VillageBus
  s_db = symbol_intern(0, 0, L"db");
  fexp* module = (fexp*)send(Fexp, s_new, s_db, DB);
  ((villagebus*)VillageBus)->modules = (fexp*)send(((villagebus*)VillageBus)->modules, s_fexp_cons, module);

  // delimiter used for composing redis key names
  ((db*)DB)->delimiter = (string*)send(String, s_new, L":", 1);
}


const fexp* db_evaluate(struct closure* closure, db* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // debug
  printf("db_evaluate: ");
  send(expression, s_print);
  printf("\n");

  // lazily initialize redis connection
  if ((self->handle == NULL) || (credis_ping(self->handle) != 0)) {
    self->handle = credis_connect("localhost", 6379, 2000);
  }
  
  // check connection
  if ((self->handle == NULL) || (credis_ping(self->handle) != 0)) {
    // TODO - try to start up redis server
    // TODO - http error handling
    printf("Could not connect to redis server\n");
    return fexp_nil;
  }

  // evaluate request 
  const Request* request = ((villagebus*)VillageBus)->request;
  string* key  = (string*)send(expression, s_fexp_join, self->delimiter);  // generate a key from expression
  switch (request->method) {
  case POST:
    expression = db_post(closure, self, key, request->data);
    break;
  case GET:
    expression = db_get(closure, self, key);
    break;
  default:
    printf("Redis has no registered handler for request method: %d\n", request->method);
    return fexp_nil;
  }

  /*char** valuev;
  int i;
  int n;
  n = credis_keys(self->handle, "*", &valuev);
  printf("keys returned: %d\n", n);
  for (i = 0; i < n; i++)
  printf(" % 2d: %s\n", i, valuev[i]);*/

  // close server connection & release resources
  if (self->handle != NULL) {
    credis_close(self->handle);
    self->handle = NULL;
  }

  return expression;
}


const fexp* db_post(struct closure* closure, db* self, const string* key, const unsigned char* data)
{
  char* keyc = (char*)send(key, s_string_tochar); // TODO - redis not support UNICODE so much
  printf("POST %s %s\n", keyc, data);
  int n = 0;
  if ((n = credis_lpush(self->handle, keyc, data)) != 0) {
    // TODO - http error handling
    printf("Could not set %s: %s \n", keyc, data);
  }
  printf("List has %d elements\n", n);
  // TODO - make number class to take 'n'
  // TODO free keyc
  return fexp_nil;
}


const fexp* db_get (struct closure* closure, db* self, const string* key)
{
  char* keyc = (char*)send(key, s_string_tochar); // TODO - redis not support UNICODE so much
  printf("GET %s\n", keyc);
  char* buffer;
  char** bufferv;
  //if (credis_get(self->handle, key, &buffer) != 0 || buffer == NULL) {
  int n = credis_lrange(self->handle, keyc, 0, -1, &bufferv); 
  if (bufferv == NULL) {
    // TODO - http error handling
    printf("Could not get %s \n", keyc);
    return fexp_nil;
  }
  printf("keys returned: %d\n", n);
  int i;
  for (i = 0; i < n; i++)
    printf(" % 2d: %s\n", i, bufferv[i]);  
  return fexp_nil;

  object* value = send(String, s_string_fromchar, buffer, strlen(buffer));
  // TODO free keyc
  return (fexp*)send(fexp_nil, s_fexp_cons, value);
}


db* db_print(struct closure* closure, db* self)
{
  wprintf(L"#<DB.%p>", self);
  return self;
}
