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


#ifndef MOD_SNMP_H
#define MOD_SNMP_H

#include <villagebus.h>

/* - snmp ---------------------------------------------------------------- */
typedef struct _snmp {
  vtable* _vt[0];
} snmp;
extern vtable* snmp_vt;
extern object* _SNMP;
extern snmp*   SNMP;
extern symbol* s_snmp;
//extern symbol* s_snmp_;

// base
void        snmp_init();
snmp*       snmp_print(closure* c, snmp* self);
const fexp* snmp_evaluate(closure* c, snmp* self, const fexp* expression);

// names
const fexp* snmp_    (closure* c, snmp* self, const fexp* message);
const fexp* snmp_get (closure* c, snmp* self, const fexp* message);

// snmp helpers
struct snmp_session* start_snmp(const char* address, const char* community);
void stop_snmp(struct snmp_session* session);
struct json_object* snmpget(struct snmp_session* session, struct json_object* oids);
struct json_object* snmpwalk(struct snmp_session* session, const char* name_oid);

// utilities
char* escape_oid(char* name);
void json_object_snmp_add(struct json_object* object, struct variable_list* variable, int index);


#endif /* MOD_SNMP_H */
