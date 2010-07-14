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


#ifndef MOD_PACKAGE_H
#define MOD_PACKAGE_H

#include <villagebus.h>

/* - package ---------------------------------------------------------------- */
typedef struct _package {
  vtable* _vt[0];
} package;
extern vtable*  package_vt;
extern object*  _Package;
extern package* Package;
extern symbol*  s_package;
extern symbol*  s_package_update;
extern symbol*  s_package_list;
extern symbol*  s_package_status;
extern symbol*  s_package_upgrade;

// base
void        package_init();
package*    package_print   (closure* c, package* self);
const fexp* package_evaluate(closure* c, package* self, const fexp* expression);

// names
const fexp* package_update (closure* c, package* self, const fexp* message);
const fexp* package_list   (closure* c, package* self, const fexp* message);
const fexp* package_status (closure* c, package* self, const fexp* message);
const fexp* package_upgrade(closure* c, package* self, const fexp* message);

// utilities
struct json_object* ipkg_list_exec_parser(const char* line, size_t length);

#endif /* MOD_PACKAGE_H */
