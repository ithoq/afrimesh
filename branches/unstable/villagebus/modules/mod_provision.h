/**
 * Afrimesh: easy management for mesh networks
 *
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


#ifndef MOD_PROVISION_H
#define MOD_PROVISION_H

#include <villagebus.h>

/* - provision ---------------------------------------------------------- */
typedef struct _provision {
  vtable* _vt[0];
  string* delimiter;
  string* device_id;
  string* handset_id;
  wchar_t* provision_device;
  wchar_t* provision_mac;
  wchar_t* provision_handset;
  wchar_t* provision_person;
  wchar_t* provision_root;
  wchar_t* provision_a2billing;
  wchar_t* message_device;
  wchar_t* message_handset;
} provision;
extern vtable* provision_vt;
extern object* _Provision;
extern provision* Provision;
extern symbol* s_provision;
extern symbol* s_provision_interface;
extern symbol* s_provision_handset;
extern symbol* s_provision_release;

void        provision_init();
provision*  provision_print   (closure* c, provision* self);
const fexp* provision_evaluate(closure* c, provision* self, const fexp* expression);

// GET
const fexp* provision_interface(closure* c, provision* self, const fexp* message); 
const fexp* provision_handset  (closure* c, provision* self, const fexp* message); 

// DELETE
const fexp* provision_release(closure* c, provision* self, const fexp* message);


#endif /* MOD_PROVISION_H */

