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


/* - provision ---------------------------------------------------------- */
vtable* provision_vt = 0;
object* _Provision = 0;
provision* Provision  = 0;
symbol* s_provision = 0;
symbol* s_provision_ip  = 0;
symbol* s_provision_mac = 0;

void provision_init()
{
  provision_vt = (vtable*)send(object_vt, s_delegated); 
  send(provision_vt, s_addMethod, s_print, provision_print);
  send(provision_vt, s_addMethod, s_villagebus_evaluate, provision_evaluate);
  _Provision = send(provision_vt, s_allocate, 0);

  // register local symbols
  s_provision_ip  = (symbol*)symbol_intern(0, _Provision, L"ip");
  s_provision_mac = (symbol*)symbol_intern(0, _Provision, L"mac"); 
  send(provision_vt, s_addMethod, s_provision_ip,  provision_ip);
  send(provision_vt, s_addMethod, s_provision_mac, provision_mac);

  // global module instance vars
  Provision = (provision*)send(_Provision->_vt[-1], s_allocate, sizeof(provision));
  Provision->delimiter = (string*)send(String, s_new, L":", 1); // MAC delimiter

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
const string* provision_ip (closure* c, provision* self, const fexp* message)
{
  Request* request = VillageBus->request; 

  // parse arguments
  string* mac    = (string*)send(message, s_fexp_car);
  fexp*   octets = (fexp*)  send(mac, s_string_split, self->delimiter);
  if ((size_t)send(octets, s_length) != 6) {
    return (string*)fexp_nil;
  }

  // get any parameters
  const char* address = NULL;
  const char* network = "10.130.1.0";
  if (request->json) {
    address = json_object_get_string(json_object_object_get(request->json, "address"));
    network = json_object_get_string(json_object_object_get(request->json, "network"));
  }
  //wprintf(L"MAC: %S ADDRESS: %s NETWORK: %s\n\n", mac->buffer, address, network);
    

  /* Strategy 1 - If an address is specified, register w/ database and 
                  simply return it again */
  if (address) {
    // is it there?

    // incr provision:id
    // set  provision:id:<mac>  <id>
    /*

 
  device:123:info
  device:123:stat
  device:124:info
  device:124:stat

  provision:00:18:0A:01:10:2F:id = 123
  provision:00:18:0A:01:10:2F:id = 124
  provision:123:info             = { mac : 00:18:0A:01:10:2F, ip : 10.0.0.5 }
  provision:123:ip               = 10.0.0.5
  provision:123:mac              = 00:18:0A:01:10:2F
  provision:124:info             = { mac : 10:18:0A:01:10:2F, ip : 10.0.0.6 }
  provision:124:ip               = 10.0.0.6
  provision:124:mac              = 10:18:0A:01:10:2F


    */
    // set provision:ip:<mac>  <ip>
    // set provision:mac:<ip>  <mac>
    // hset provision:id:<id> mac <mac>
    // hset provision:id:<id> ip  <ip>

    return (string*)send(String, s_string_fromwchar, L"%s", address);
  }

  /* Strategy 2 - Sequential allocation within a given network backed
                  by database.
     . Check for MAC in database
     . If it's there return allocated IP
     . Otherwise, increment allocation counter, generate an IP
       and store it in database */

  // convert network address to int
  struct in_addr _in_addr;
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
  printf("inet_ntop:          %s\n", buf);

  // Check for MAC in database
  // If there, look up IP and return
  // If not, increment counter, generate IP and store in database
  string* ret = (string*)send(String, s_string_fromwchar, L"\"10.130.1.2\"");

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

  return ret;
}


/**
 * Given an IP address return the MAC associated with it.
 */
const string* provision_mac(closure* c, provision* self, const fexp* message)
{
  return (string*)fexp_nil;
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
