/**
 * Afrimesh: easy management for mesh networks
 *
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
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
