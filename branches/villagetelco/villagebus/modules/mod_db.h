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


#ifndef DB_H
#define DB_H

#include <villagebus.h>

/* - db ----------------------------------------------------------------- */
typedef struct _db {
  vtable* _vt[0];
  REDIS   handle;
  string* delimiter;
} db;
extern vtable* db_vt;
extern object* DB;
extern object* s_db;
extern struct symbol* s_db_keys;
extern object* s_db_get;   // these guys need to be symbols
extern object* s_db_lrange; 
extern object* s_db_getset; 
extern object* s_db_lpush;   

void db_init();
db*  db_print(struct closure* closure, db* self);

const fexp* db_evaluate(struct closure* closure, db* self, const fexp* expression);

// GET 
const fexp* db_keys  (struct closure* closure, db* self, const fexp* message); // any
const fexp* db_get   (struct closure* closure, db* self, const fexp* message); // string
const fexp* db_lrange(struct closure* closure, db* self, const fexp* message); // list

// PUT
const fexp* db_getset(struct closure* closure, db* self, const fexp* message, const unsigned char* data); // string

// POST
const fexp* db_lpush(struct closure* closure, db* self, const fexp* message, const unsigned char* data);  // list


#endif /* DB_H */
