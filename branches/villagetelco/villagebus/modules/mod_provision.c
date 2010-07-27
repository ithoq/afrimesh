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

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "mod_provision.h"
#include "mod_db.h"


/* - provision ---------------------------------------------------------- */
vtable* provision_vt = 0;
object* _Provision = 0;
provision* Provision  = 0;
symbol* s_provision = 0;
symbol* s_provision_ip  = 0;

void provision_init()
{
  provision_vt = (vtable*)send(object_vt, s_delegated); 
  send(provision_vt, s_addMethod, s_print, provision_print);
  send(provision_vt, s_addMethod, s_villagebus_evaluate, provision_evaluate);
  _Provision = send(provision_vt, s_allocate, 0);

  // register local symbols
  s_provision_ip  = (symbol*)symbol_intern(0, _Provision, L"ip");
  send(provision_vt, s_addMethod, s_provision_ip,  provision_ip);

  // global module instance vars
  Provision = (provision*)send(_Provision->_vt[-1], s_allocate, sizeof(provision));
  Provision->delimiter = (string*)send(String, s_new, L":", 1); // MAC delimiter
  Provision->device_id  = (string*)send(String, s_string_fromwchar, L"device:id");
  Provision->device_ids = (string*)send(String, s_string_fromwchar, L"device:ids");
  Provision->provision_device  = (string*)send(String, s_string_fromwchar, L"provision:device:");
  Provision->provision_mac     = (string*)send(String, s_string_fromwchar, L"provision:mac:");
  Provision->message_provision = (string*)send(String, s_string_fromwchar, L"message:device:provision:");
  // register module with VillageBus 
  s_provision = (symbol*)symbol_intern(0, 0, L"provision");
  fexp* module = (fexp*)send(Fexp, s_new, s_provision, Provision);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}



/**
 *
 */
const fexp* provision_evaluate(closure* c, provision* self, const fexp* expression)
{
  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _Provision, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 
  return (fexp*)send(VillageBus, s_villagebus_error, 
                     L"mod_provision has no registered handler for name: %S",
                     name->buffer);  
}


/* - GET ---------------------------------------------------------------- */

/**
 * Given a MAC address return the IP associated with it.
 *
 *   in_addr_t          ->  u_int
 *   struct in_addr     -> { in_addr_t s_addr; }
 *   struct sockaddr_in -> { __uint8_t       sin_len;
 *                           sa_family_t     sin_family;
 *                           in_port_t       sin_port;
 *                           struct  in_addr sin_addr;
 *                           char            sin_zero[8];
 *   };
 */
const fexp* provision_ip (closure* c, provision* self, const fexp* message)
{
  Request* request = VillageBus->request; 

  // parse arguments
  string* mac    = (string*)send(message, s_fexp_car);
  fexp*   octets = (fexp*)  send(mac, s_string_split, self->delimiter);
  if ((size_t)send(octets, s_length) != 6) {
    return fexp_nil;
  }

  // get any parameters
  string* address = (string*)fexp_nil;
  string* network = (string*)fexp_nil;
  if (request->json) {
    const char* s;
    s = json_object_get_string(json_object_object_get(request->json, 
                                                      "address"));
    if (s) address = (string*)send(String, s_string_fromwchar, L"%s", s);
    s = json_object_get_string(json_object_object_get(request->json, 
                                                      "network"));
    if (s) network = (string*)send(String, s_string_fromwchar, L"%s", s);
  }
  //wprintf(L"MAC: %S ADDRESS: %S NETWORK: %S\n\n", mac->buffer, address->buffer, network->buffer);

  /* Strategy 1 - If an address is specified, simply register it w/ the
                  provisioning database and return it */
  if (address != (string*)fexp_nil) {

    fexp* reply = fexp_nil;

    // connect to database
    fexp* error = (fexp*)send(DB, s_db_connect);
    if (error != fexp_nil) {
      return error;
    }

    // TODO - check if this mac is already linked to a device

    // check for an outstanding provisioning notification for this mac
    fexp* f = fexp_nil;
    string* message_provision = (string*)send(self->message_provision, 
                                              s_string_add, mac);
    f = (fexp*)send(f, s_fexp_cons, message_provision);
    string* notification = (string*)send(DB, s_db_get, f);
    if ((fexp*)notification != fexp_nil) {
      char* notificationc = (char*)send(notification, s_string_tochar);
      struct json_object* json = json_tokener_parse(notificationc);
      reply = (fexp*)send(String, s_string_fromwchar, 
                          L"%s %s %s %S", 
                          json_object_get_string(json_object_object_get(json, "id")),
                          json_object_get_string(json_object_object_get(json, "mac")),
                          json_object_get_string(json_object_object_get(json, "ip")),
                          L"10.0.0.1"); 
      goto done;
    }
    
    // register interface details with database
    string* id = (string*)send(DB, s_db_incr, self->device_id);  // incr device:id
    send(DB, s_db_sadd, self->device_ids, id);                   // sadd device:ids
    string* provision_device = (string*)send(self->provision_device,
                                             s_string_add, address);
    string* provision_mac    = (string*)send(self->provision_mac, 
                                             s_string_add, address);
    send(DB, s_db_set,  provision_device, id);                   // set  provision:device:<ip> id
    send(DB, s_db_set,  provision_mac,    mac);                  // set  provision:mac:<ip>    mac
    reply = (fexp*)send(String, s_string_fromwchar, 
                        L"%S %S %S %S", 
                        id->buffer, 
                        mac->buffer, 
                        address->buffer, 
                        L"10.0.0.1"); 

    // register provisioning request w/ notification queue
    notification = (string*)send(String, s_string_fromwchar, 
                                 L"{ 'source' : 'device', "
                                 "'name' : 'provision', "
                                 "'id' : %S, "
                                 "'ip' : '%S', "
                                 "'mac' : '%S' }",
                                 id->buffer, address->buffer, mac->buffer);
    send(DB, s_db_set, message_provision, notification);

    // TODO - poll notification queue for new devices
    // TODO - link/register user to device
    // TODO - provision voip
    
  done:
    // clean up
    send(DB, s_db_close);

    // send back id, mac, ip, root
    return reply; // TODO - pull from site config
  }


  /* Strategy 2 - Sequential allocation within a given network backed
                  by database.
     . Check for MAC in database
     . If it's there return allocated IP
     . Otherwise, increment allocation counter, generate an IP
       and store it in database */
  // convert network address to int
  /*struct in_addr _in_addr;
  if(inet_aton(network, &_in_addr) == 0) {
    wprintf(L"Error parsing %s: %s\n", network, strerror(errno));
    // TODO - error handling
  }
  wprintf(L"inet_aton:   %d, %s\n", _in_addr.s_addr, inet_ntoa(_in_addr.s_addr));  
  // construct IP from network address and sequence#
  unsigned int sequence = 258;
  _in_addr.s_addr = ntohl(ntohl(_in_addr.s_addr) + sequence);
  wprintf(L"inet_aton:   %d, %s\n", _in_addr.s_addr, inet_ntoa(_in_addr.s_addr));  
  // convert int ip address to string
  struct sockaddr_in sa;
  sa.sin_addr = _in_addr;
  char buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(sa.sin_addr), buf, INET_ADDRSTRLEN);
  if (buf == NULL) {
    wprintf(L"Error parsing int %s: %s\n", network, strerror(errno));
  }
  printf("inet_ntop:          %s\n", buf); */
  // Check for MAC in database
  // If there, look up IP and return
  // If not, increment counter, generate IP and store in database
  // Strategy 3 - Dumb MAC2IP
  /*string* octet_1 = (string*)send(octets, s_fexp_nth, 3);
  string* octet_2 = (string*)send(octets, s_fexp_nth, 4);
  string* octet_3 = (string*)send(octets, s_fexp_nth, 5);
  int n1 = wcstoimax(octet_1->buffer, NULL, 16);
  int n2 = wcstoimax(octet_2->buffer, NULL, 16);
  int n3 = wcstoimax(octet_3->buffer, NULL, 16);
  // TODO - the default for Strings in main.c should be to print as json *sigh*
  string* ret = (string*)send(String, s_string_fromwchar, L"\"10.%d.%d.%d\"", n1, n2, n3);*/
  // Strategy 4 - ability to specify an external hook for IP generation

  string* ret = (string*)send(String, s_string_fromwchar, L"\"10.130.1.20\"");
  return (fexp*)ret;
}


 


/* - Global Handlers ---------------------------------------------------- */

/**
 *
 */
provision* provision_print(closure* c, provision* self)
{
  wprintf(L"#<Provision.%p>", self);
  return self;
}
