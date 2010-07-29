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


#ifndef MOD_TELEPHONY_H
#define MOD_TELEPHONY_H

#include <villagebus.h>

// TODO -> s/mod_telephony/mod_handset

/* - telephony ---------------------------------------------------------- */
typedef struct _telephony {
  vtable* _vt[0];
} telephony;
extern vtable*    telephony_vt;
extern object*    _Telephony;
extern telephony* Telephony;
extern symbol*    s_telephony;
extern symbol*    s_telephony_sip;
extern symbol*    s_telephony_callme;

// base
void        telephony_init();
telephony*  telephony_print(closure* c, telephony* self);
const fexp* telephony_evaluate(closure* c, telephony* self, const fexp* expression);

// names
const fexp* telephony_sip_asterisk(closure* c, telephony* self, const fexp* message);
const fexp* telephony_callme      (closure* c, telephony* self, const fexp* message);

// utilities
struct json_object* asterisk_sip_peers_parser(const char* line, size_t length);


#endif /* MOD_TELEPHONY_H */
