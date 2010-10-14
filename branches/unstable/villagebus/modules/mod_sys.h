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


#ifndef SYS_H
#define SYS_H

#include <villagebus.h>

/* - sys ---------------------------------------------------------------- */
typedef struct _sys {
  vtable* _vt[0];
} sys;
extern vtable* sys_vt;
extern object* _Sys;
extern sys*    Sys;
extern symbol* s_sys;
extern symbol* s_sys_service;
extern symbol* s_sys_syslog;
extern symbol* s_sys_uname;
extern symbol* s_sys_version;

void sys_init();
sys* sys_print(closure* c, sys* self);

const fexp* sys_evaluate(closure* c, sys* self, const fexp* expression);

// names
const fexp*   sys_service(closure* c, sys* self, const fexp* message);
const fexp*   sys_syslog (closure* c, sys* self, const fexp* message);
const fexp*   sys_uname  (closure* c, sys* self, const fexp* message);
const string* sys_version(closure* c, sys* self, const fexp* message); 

// utilities
const fexp* exec(char* command, char** arguments);
const fexp* exec_out(char* command, char** arguments);
const fexp* exec_string(char* command, char** arguments);
const fexp* exec_parsed(char* command, char** arguments, 
                        struct json_object* (*parser)(const char*, size_t));


#endif /* SYS_H */
