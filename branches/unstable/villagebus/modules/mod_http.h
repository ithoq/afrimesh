/**
 * Afrimesh: easy management for mesh networks
 *
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


#ifndef MOD_HTTP_H
#define MOD_HTTP_H

#include <villagebus.h>

/* - http ---------------------------------------------------------- */
typedef struct _http {
  vtable* _vt[0];
  string* delimiter;
} http;
extern vtable* http_vt;
extern object* _Http;
extern http* Http;
extern symbol* s_http;
extern symbol* s_http_get;

// base
void        http_init();
http*       http_print   (closure* c, http* self);
const fexp* http_evaluate(closure* c, http* self, const fexp* expression);

// names
const fexp* http_get(closure* c, http* self, const fexp* message); 

#endif /* MOD_HTTP_H */
