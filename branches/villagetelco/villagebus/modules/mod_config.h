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


#ifndef CONFIG_H
#define CONFIG_H

#include <villagebus.h>

/* - config ------------------------------------------------------------- */
typedef struct _config {
  struct vtable* _vt[0];
  struct uci_context* context;
  string* delimiter;
} config;
extern struct vtable* config_vt;
extern object* Config;
extern object* s_config;

void  config_init();

const fexp*  config_evaluate(struct closure* closure, config* self, const fexp* message);

const fexp* config_put(struct closure* closure, config* self, const fexp* message, const unsigned char* payload);
const fexp* config_get(struct closure* closure, config* self, const fexp* message);

config* config_print(struct closure* closure, config* self);


/* - uci utilities ------------------------------------------------------ */
int uci_show(struct uci_context* context, const char* package);
int uci_show_package(struct uci_context* context, const char* package);
bool uci_set_config(struct uci_context* context, const char* config, const char* section, const char* option, const char* value);
char* option_to_string(struct uci_option* option);
//static char* uci_lookup_section_ref(struct uci_section *s);
static void uci_reset_typelist(void);


#endif /* CONFIG_H */

