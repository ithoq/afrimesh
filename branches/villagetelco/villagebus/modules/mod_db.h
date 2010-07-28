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


#ifndef MOD_DB_H
#define MOD_DB_H

#include <credis.h>
#include <villagebus.h>

/* - db ----------------------------------------------------------------- */
typedef struct _db {
  vtable* _vt[0];
  REDIS   handle;
  string* delimiter;
} db;
extern vtable* db_vt;
extern object* _DB;
extern db*     DB;
extern object* s_db;
extern object* s_db_connect;
extern object* s_db_close;
extern symbol* s_db_keys;
extern object* s_db_get;   // these guys need to be symbols
extern object* s_db_lrange; 
extern object* s_db_getset; 
extern object* s_db_lpush;   
extern object* s_db_incr; 
extern object* s_db_set; 
extern object* s_db_sadd;
extern object* s_db_del; 

void        db_init();
db*         db_print(closure* c, db* self);
const fexp* db_evaluate(closure* c, db* self, const fexp* expression);

// GET 
const fexp*   db_keys  (closure* c, db* self, const fexp* message); // any
const string* db_get   (closure* c, db* self, const fexp* message); // string
const fexp*   db_lrange(closure* c, db* self, const fexp* message); // list

// PUT
const string* db_getset(closure* c, db* self, const fexp* message, const unsigned char* data); // string
const string* db_incr  (closure* c, db* self, const fexp* message);                            // counter

// POST
const fexp*   db_lpush(closure* c, db* self, const fexp* message, const unsigned char* data);  // list

// DELETE
const fexp* db_del(closure* c, db* self, const fexp* message); // any

// Helper
const fexp* db_connect(closure* c, db* self);
const fexp* db_close  (closure* c, db* self);

// TODO - either rewrite these to 'fexp* message' style or write a request parser
const string* db_set (closure* c, db* self, const string* key, const string* value);           // string
const string* db_sadd(closure* c, db* self, const string* key, const string* member);          // set


#endif /* MOD_DB_H */

