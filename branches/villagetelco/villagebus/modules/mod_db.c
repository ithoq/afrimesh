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

#include "mod_db.h"


/* - db ----------------------------------------------------------------- */
vtable* db_vt = 0;
object* _DB = 0;
db*     DB  = 0;
object* s_db = 0;
object* s_db_connect = 0;
object* s_db_close   = 0;
symbol* s_db_keys    = 0;
object* s_db_get     = 0;
object* s_db_lrange  = 0; 
object* s_db_getset  = 0; 
object* s_db_lpush   = 0;
object* s_db_incr    = 0; 
object* s_db_set     = 0; 
object* s_db_sadd    = 0;
object* s_db_del     = 0;


void db_init()
{
  db_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(db_vt, s_addMethod, s_print, db_print);
  send(db_vt, s_addMethod, s_villagebus_evaluate, db_evaluate);
  _DB = send(db_vt, s_allocate, 0);

  // register local symbols
  s_db_connect = symbol_intern(0, _DB, L"connect");
  s_db_close   = symbol_intern(0, _DB, L"close");
  s_db_keys    = (symbol*)symbol_intern(0, _DB, L"keys"); // TODO - symbol_intern should return symbol*
  s_db_get     = symbol_intern(0, _DB, L"get");
  s_db_lrange  = symbol_intern(0, _DB, L"lrange");
  s_db_getset  = symbol_intern(0, _DB, L"set");
  s_db_lpush   = symbol_intern(0, _DB, L"lpush");
  s_db_incr    = symbol_intern(0, _DB, L"incr");
  s_db_set     = symbol_intern(0, _DB, L"_set"); // TODO - don't alias getset to set
  s_db_sadd    = symbol_intern(0, _DB, L"sadd");
  s_db_del     = symbol_intern(0, _DB, L"del");
  send(db_vt, s_addMethod, s_db_connect, db_connect);
  send(db_vt, s_addMethod, s_db_close,   db_close);
  send(db_vt, s_addMethod, s_db_keys,    db_keys);
  send(db_vt, s_addMethod, s_db_get,     db_get);
  send(db_vt, s_addMethod, s_db_lrange,  db_lrange);
  send(db_vt, s_addMethod, s_db_getset,  db_getset);
  send(db_vt, s_addMethod, s_db_lpush,   db_lpush);
  send(db_vt, s_addMethod, s_db_incr,    db_incr);
  send(db_vt, s_addMethod, s_db_set,     db_set);
  send(db_vt, s_addMethod, s_db_sadd,    db_sadd);
  send(db_vt, s_addMethod, s_db_del,     db_del);

  // global module instance vars
  DB = (db*)send(_DB->_vt[-1], s_allocate, sizeof(db));
  DB->handle = NULL;
  DB->delimiter = (string*)send(String, s_new, L":", 1); // delimiter used for composing redis key names

  // register module with VillageBus - TODO lose vb->modules & register directly in vtable perhaps?
  s_db = symbol_intern(0, 0, L"db");
  fexp* module = (fexp*)send(Fexp, s_new, s_db, DB);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}



/**
 *
 */
const fexp* db_evaluate(closure* c, db* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // debug
  /*printf("db_evaluate: ");
  send(message, s_print);
  printf("\n");*/

  // lazily initialize redis connection
  fexp* error = (fexp*)send(self, s_db_connect);
  if (error != fexp_nil) {
    return error;
  }

  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _DB, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 

  // evaluate expression w/ default handlers
  Request* request = VillageBus->request;
  const fexp* reply = fexp_nil;
  switch (request->method) {
  case PUT:
    reply = (fexp*)send(self, s_db_getset, expression, request->data);
    break;
  case POST:
    reply = (fexp*)send(self, s_db_lpush, expression, request->data);
    break;
  case GET:
    reply = (fexp*)send(self, s_db_get, expression);
    break;
  case DELETE:
    reply = (fexp*)send(self, s_db_del, expression);
    break;
  default:
    reply = (fexp*)send(VillageBus, 
                        s_villagebus_error, 
                        L"mod_db has no registered handler for request method: %d", 
                        request->method);  // TODO method_to_string 
  }

  // close server connection & release resources
  send(self, s_db_close);

  return reply;
}


const fexp* db_connect(closure* c, db* self)
{
  if ((self->handle == NULL) || (credis_ping(self->handle) != 0)) {
    self->handle = credis_connect("localhost", 6379, 2000);  // TODO - get redis DB address from config
  }

  // check connection
  if ((self->handle == NULL) || (credis_ping(self->handle) != 0)) {
    // TODO - try to start up redis server
    return (fexp*)send(VillageBus, s_villagebus_error, L"Could not connect to redis server");
  }

  return fexp_nil;
}


const fexp* db_close(closure* c, db* self)
{
  if (self->handle != NULL) {
    credis_close(self->handle);
    self->handle = NULL;
  }
  return fexp_nil;
}


/* - GET ---------------------------------------------------------------- */

/**
 *
 */
const fexp* db_keys(closure* c, db* self, const fexp* message)
{
  fexp*   reply = fexp_nil;
  string* key   = (string*)send(message, s_fexp_join, self->delimiter);  // generate a key from message
  char*   keyc  = (char*)send(key, s_string_tochar); // TODO - redis not support UNICODE so much
  wprintl(L"GET /db/keys/%S\n", key->buffer);
  char** bufferv;
  int i;
  for (i = 0; i < credis_keys(self->handle, keyc, &bufferv); i++) {
    if (strcmp(bufferv[i], "") == 0) continue; // getting an empty key on zero match result :-/
    string* item = (string*)send(String, s_string_fromchar, bufferv[i], strlen(bufferv[i]));
    reply = (fexp*)send(reply, s_fexp_cons, item);
  }
  free(keyc);
  return reply;
}


/**
 * TODO - parameter polymorphism:
 *          when message is a list, first item is the key
 *          when message is a string then message is the key
 */
const string* db_get(closure* c, db* self, const fexp* message)
{
  object* reply = (object*)fexp_nil;
  string* key   = (string*)send(message, s_fexp_join, self->delimiter); 
  char*   keyc  = (char*)send(key, s_string_tochar); 
  char*   val;
  int rc = credis_get(self->handle, keyc, &val);
  wprintl(L"GET /db/get/%S -> %d %s\n", key->buffer, rc, val);
  if (rc == -1) {                       // key does not exist
    reply = (object*)fexp_nil;
  } else if (rc != 0 || val == NULL) {  // error
    reply = send(VillageBus, s_villagebus_error, L"get failed %s", keyc);
  } else {                              // success
    reply = send(String, s_string_fromchar, val, strlen(val));
  }
  free(keyc);
  return (string*)reply;
}


// TODO - debug info for error messages
#define DMSG L"%s:%d %s() - "L""
#define DARG __FILE__, __LINE__, __FUNCTION__

/**
 *
 */
const fexp* db_lrange(closure* c, db* self, const fexp* message)
{
  Request* request = VillageBus->request;
  fexp* reply = fexp_nil;

  // get any parameters
  int start = 0;
  int end   = -1;
  if (request->json) {
    start = json_object_get_int(json_object_object_get(request->json, "start"));
    end   = json_object_get_int(json_object_object_get(request->json, "end"));
  }

  // build key
  string* skey = (string*)send(message, s_fexp_join, self->delimiter);  // generate a key from message
  char*   key  = (char*)send(skey, s_string_tochar); // TODO - redis not support UNICODE so much

  // make query
  wprintl(L"GET /db/lrange/%S %d %d\n", skey->buffer, start, end);
  char** bufferv;
  int n = credis_lrange(self->handle, key, start, end, &bufferv); 
  if (bufferv == NULL) {
    reply = (fexp*)send(VillageBus, s_villagebus_error, DMSG"query failed for %s", key);
    free(key);
    return reply;
  }

  // TODO IMPORTANT - all values stored in redis to be well-formed JSON please :)
  //                  our best bet would probably be to do mandatory conversion from
  //                  fexp*'s inside mod_db.
  int i;
  for (i = 0; i < n; i++) {
    string* item = (string*)send(String, s_string_fromchar, bufferv[i], strlen(bufferv[i]));
    reply = (fexp*)send(reply, s_fexp_cons, item);
  }
  free(key);
  if (n) {
    reply = (fexp*)send(reply, s_fexp_cons, s_villagebus_json);  // tag reply as JSON
  }

  return reply;
}


/* - PUT ---------------------------------------------------------------- */

const string* db_incr(closure* c, db* self, const fexp* message)
{  
  string* reply;

  string* key  = (string*)send(message, s_fexp_join, self->delimiter);  // generate a key from message
  char* keyc = (char*)send(key, s_string_tochar);
  int value  = 0;
  int rc     = credis_incr(self->handle, keyc, &value);
  if (rc != 0) {
    reply = (string*)send(VillageBus, s_villagebus_error, L"incr failed %ss", keyc);
  } else {
    reply = (string*)send(String, s_string_fromwchar, L"%d", value);
  }
  free(keyc);
  return reply;
}


// TODO - we need a better solution
#include <time.h>
const unsigned char* fix_timestamps(const unsigned char* data) 
{
  struct json_object* json = json_tokener_parse((unsigned char*)data);
  if (json == NULL || !json_object_is_type(json, json_type_object)) { // if it doesn't segfault :-)
    wprintl(L"fix_timestamps utterly failed to convert |%s| to a json object\n", data);
    return data;
  }
  struct json_object* timestamp = json_object_object_get(json, "timestamp");
  if (timestamp == NULL) {
    wprintl(L"fix_timestamps has come to the conclusion that |%s| probably has no timestamp\n", data);
  }
  time_t t;
  t = time(&t);
  timestamp = json_object_new_int(t);
  json_object_object_add(json, "timestamp", timestamp);
  return json_object_get_string(json);
}


/**
 *
 */
const string* db_getset(closure* c, db* self, const fexp* message, const unsigned char* data)
{
  object* reply = (object*)fexp_nil;

  // generate key
  string* skey  = (string*)send(message, s_fexp_join, self->delimiter);  // generate a key from message
  char*   key   = (char*)send(skey, s_string_tochar); // TODO - redis not support UNICODE so much

  // make query
  wprintl(L"PUT /db/getset/%S %s\n", skey->buffer, data);
  char* val;
  const unsigned char* fixed = fix_timestamps(data);
  int rc = credis_getset(self->handle, key, fixed, &val);
  
  /* credis is a bit crummy here... getset on new keys return rc==-1 && val==NULL, 
     on existing keys rc==0 && val=data */
  if (rc == -1) {
    val = (char*)data;
  } else if (rc != 0) {
    reply = send(VillageBus, s_villagebus_error, L"getset failed %s: %s", key, data);
    free(key);
    return (string*)reply;
  }
  free(key);
  reply = send(String, s_string_fromchar, val, strlen(val));
  
  return (string*)reply;
}



/* - POST --------------------------------------------------------------- */

/**
 *
 */
const fexp* db_lpush(closure* c, db* self, const fexp* message, const unsigned char* data)
{
  fexp* reply = fexp_nil;

  string* key  = (string*)send(message, s_fexp_join, self->delimiter);  // generate a key from message
  wprintl(L"POST /db/lpush/%S %s\n", key->buffer, data);

  char* keyc = (char*)send(key, s_string_tochar); // TODO - redis not support UNICODE so much
  const unsigned char* fixed = fix_timestamps(data);
  if (credis_lpush(self->handle, keyc, fixed) != 0) {
    reply = (fexp*)send(VillageBus, s_villagebus_error, L"lpush failed %s: %s", keyc, data);
  }
  free(keyc);

  return reply;
}


/* - DELETE ------------------------------------------------------------- */
const fexp* db_del(closure* c, db* self, const fexp* message)
{
  fexp*   reply = fexp_nil;
  string* key   = (string*)send(message, s_fexp_join, self->delimiter); 
  char*   keyc  = (char*)send(key, s_string_tochar); 
  int     rc    = credis_del(self->handle, keyc);
  wprintl(L"DELETE /db/del/%S -> %d\n", key->buffer, rc);
  if (rc == 0) {           // success             
  } else if (rc == -1) {   // key does not exist
  } else {                 // error
    reply = (fexp*)send(VillageBus, s_villagebus_error, L"del failed %s", keyc);
  }
  free(keyc);
  return reply;
}



/* - TODO --------------------------------------------------------------- */

const string* db_sadd(closure* c, db* self, const string* key, const string* member)
{
  char* keyc    = (char*)send(key,    s_string_tochar);
  char* memberc = (char*)send(member, s_string_tochar);
  int rc = credis_sadd(self->handle, keyc, memberc);
  if (rc == -1) {       // already a member of the set
    return member;
  } else if (rc == 0) { // not a member
    return (string*)fexp_nil;
  } 
  return (string*)send(VillageBus, s_villagebus_error, L"sadd failed %s: %s", keyc, memberc);
}


const string* db_set (closure* c, db* self, const string* key, const string* value)
{
  string* reply = (string*)value;
  char* keyc    = (char*)send(key,   s_string_tochar);
  char* valuec  = (char*)send(value, s_string_tochar);
  int rc        = credis_set(self->handle, keyc, valuec);
  if (rc != 0) { 
    reply = (string*)send(VillageBus, s_villagebus_error, L"set failed %s: %s", keyc, valuec);
  } 
  free(keyc);
  free(valuec);
  return reply;
}




/* - Global Handlers ---------------------------------------------------- */

/**
 *
 */
db* db_print(closure* c, db* self)
{
  wprintf(L"#<DB.%p>", self);
  return self;
}
