/**
 * Afrimesh: easy management for mesh networks
 *
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "mod_config.h"
#include "mod_provision.h"
#include "mod_db.h"
#include "mod_http.h"

#define PROVISION_DEFAULT_ROOT L"10.130.1.1"

/* - provision ---------------------------------------------------------- */
vtable* provision_vt = 0;
object* _Provision = 0;
provision* Provision  = 0;
symbol* s_provision = 0;
symbol* s_provision_interface = 0;
symbol* s_provision_handset   = 0;
symbol* s_provision_release   = 0;


void provision_init()
{
  provision_vt = (vtable*)send(object_vt, s_delegated); 
  send(provision_vt, s_addMethod, s_print, provision_print);
  send(provision_vt, s_addMethod, s_villagebus_evaluate, provision_evaluate);
  _Provision = send(provision_vt, s_allocate, 0);

  // register local symbols
  s_provision_interface  = (symbol*)symbol_intern(0, _Provision, L"interface");
  s_provision_handset    = (symbol*)symbol_intern(0, _Provision, L"handset");
  s_provision_release    = (symbol*)symbol_intern(0, _Provision, L"release");
  send(provision_vt, s_addMethod, s_provision_interface,  provision_interface);
  send(provision_vt, s_addMethod, s_provision_handset,    provision_handset);
  send(provision_vt, s_addMethod, s_provision_release,    provision_release);

  // global module instance vars
  Provision = (provision*)send(_Provision->_vt[-1], s_allocate, sizeof(provision));
  Provision->delimiter   = (string*)send(String, s_new, L":", 1); // MAC delimiter
  Provision->device_id   = (string*)send(String, s_string_fromwchar, L"device:id");
  Provision->handset_id  = (string*)send(String, s_string_fromwchar, L"handset:id");
  Provision->provision_device  = L"provision:%S:device";
  Provision->provision_mac     = L"provision:%S:mac";
  Provision->provision_handset = L"provision:%S:handset";
  Provision->provision_person  = L"provision:%S:person";
  Provision->message_device    = L"message:device:%S:provision";
  Provision->message_handset   = L"message:handset:%S:provision";

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
  // read config for root server address - TODO macros for easy villagebus calls
  string* val  = (string*)send(String, s_string_fromwchar, L"val");
  string* key  = (string*)send(String, s_string_fromwchar, L"afrimesh.settings.root");
  fexp* m = fexp_nil;
  m = (fexp*)send(m, s_fexp_cons, key);
  m = (fexp*)send(m, s_fexp_cons, val);
  string* root = (string*)send(Config, s_villagebus_evaluate, m);
  self->provision_root = root->buffer;
  key = (string*)send(String, s_string_fromwchar, L"afrimesh.settings.a2billing");
  m = fexp_nil;
  m = (fexp*)send(m, s_fexp_cons, key);
  m = (fexp*)send(m, s_fexp_cons, val);
  string* a2billing = (string*)send(Config, s_villagebus_evaluate, m);
  self->provision_a2billing = a2billing->buffer;
  //wprintf(L"root: %S  a2billing: %S\n", self->provision_root, self->provision_a2billing);

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
const fexp* provision_interface (closure* c, provision* self, const fexp* message)
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
  wprintl(L"PROVISIONING WITH MAC: %S ADDRESS: %S NETWORK: %S\n\n", mac->buffer, address->buffer, network->buffer);

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
    fexp* message_device = fexp_nil;
    string* s = (string*)send(String, s_string_fromwchar, 
                              self->message_device, 
                              mac->buffer);
    message_device = (fexp*)send(message_device, s_fexp_cons, s);
    string* notification = (string*)send(DB, s_db_get, message_device);
    //wprintl(L"GOT NOTIFICATION: %S\n", notification->buffer);
    if (notification != (string*)fexp_nil) {
      wprintl(L"WE HAVE ONE ALREADY, don't make a new notification\n");
      char* notificationc = (char*)send(notification, s_string_tochar);
      struct json_object* json = json_tokener_parse(notificationc);
      reply = (fexp*)send(String, s_string_fromwchar, 
                          L"%s %s %s %S", 
                          json_object_get_string(json_object_object_get(json, "id")),
                          json_object_get_string(json_object_object_get(json, "mac")),
                          json_object_get_string(json_object_object_get(json, "ip")),
                          self->provision_root); 
      goto done;
    }
    
    // register interface details with database
    fexp* device_id = fexp_nil;
    device_id = (fexp*)send(device_id, s_fexp_cons, self->device_id);
    string* id = (string*)send(DB, s_db_incr, device_id);  // incr device:id
    string* provision_device = (string*)send(String, s_string_fromwchar, 
                                             self->provision_device,
                                             address->buffer);
    string* provision_mac    = (string*)send(String, s_string_fromwchar,
                                             self->provision_mac, 
                                             address->buffer);
    send(DB, s_db_set,  provision_device, id);                   // set  provision:<ip>:device device.id
    string* json_mac = (string*)send(mac, s_tojson, false);
    send(DB, s_db_set,  provision_mac, json_mac);                // set  provision:<ip>:mac    mac
    reply = (fexp*)send(String, s_string_fromwchar, L"%S %S %S %S",
                        id->buffer, 
                        mac->buffer, 
                        address->buffer,  
                        self->provision_root);    // TODO - read from VTE server UCI config

    // register provisioning request w/ notification queue
    // TODO - we should really only be setting this once the device has
    //        been provisioned, rebooted and successfully connected to
    //        the network. This is fine for the demo though.
    notification = (string*)send(String, s_string_fromwchar, 
                                 L"{ 'source' : 'device', "
                                 "'name' : 'provision', "
                                 "'id' : %S, "
                                 "'ip' : '%S', "
                                 "'mac' : '%S' }",
                                 id->buffer, address->buffer, mac->buffer);
    s = (string*)send(String, s_string_fromwchar, 
                      self->message_device, 
                      mac->buffer);
    send(DB, s_db_set, s, notification);
    
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

  string* ret = (string*)send(String, s_string_fromwchar, L"\"10.130.1.1\"");
  return (fexp*)ret;
}


const fexp* provision_handset(closure* c, provision* self, const fexp* message)
{
  // get parameters
  string* mac = (string*)send(message, s_fexp_car);
  // string* device = TODO - read json request
  // string* a2billing = TODO - read json request for a2billing.id ? Else ask Redis?
  wprintl(L"GET /provision/handset/%S\n", mac->buffer);

  // TODO lookup device id for MAC & validate against json request 
  
  // TODO - check if this handset is already linked to a device

  // TODO - Tell A2Billing @ trunk about this handset

  // Ask A2Billing for a trunk&username&secret&codec
  string* url = (string*)send(String, s_string_fromwchar, 
                              L"http://%S/a3glue/a3g_provisioning.php?mac_addr=%S", 
                              self->provision_a2billing, mac->buffer);
  fexp* urlf = (fexp*)send(fexp_nil, s_fexp_cons, url);
  string* response  = (string*)send(Http, s_http_get, urlf);
  if ((size_t)send(response, s_length) == 0) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"Zero length: Invalid provisioning response from a3glue");
  }

  // TODO - error handling for non-existent MAC addresses
  char*   responsec = (char*)send(response, s_string_tochar);
  struct json_object* a3glue = json_tokener_parse(responsec);
  if (a3glue == NULL) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"Syntax Error: Invalid provisioning response from a3glue");
  }

  const char* did      = json_object_get_string(json_object_object_get(a3glue, "did"));
  const char* username = json_object_get_string(json_object_object_get(a3glue, "username"));
  const char* secret   = json_object_get_string(json_object_object_get(a3glue, "uipass"));
  const char* codec    = json_object_get_string(json_object_object_get(a3glue, "codec"));

  // connect to data store
  fexp* error = (fexp*)send(DB, s_db_connect);
  if (error != fexp_nil) {
    return error;
  }

  // register handset details with data store
  fexp*   handset    = (fexp*)send(fexp_nil, s_fexp_cons, self->handset_id);
  string* handset_id = (string*)send(DB, s_db_incr, handset);  // incr handset:id
  string* provision_handset = (string*)send(String, s_string_fromwchar, 
                                            self->provision_handset,
                                            mac->buffer);
  send(DB, s_db_set, provision_handset, handset_id);           // set provision:<device>:handset <handset.id>

  // TODO - register provisioning request w/ notification queue ?

  // Return IP address, username, secret & any other config for the given handset
  string* reply = (string*)send(String, s_string_fromwchar, 
                                L"%S %S %s %s %s %s",
                                handset_id->buffer,
                                self->provision_a2billing,
                                did,
                                username,
                                secret,
                                codec);
 done:
  // clean up
  send(DB, s_db_close);

  return (fexp*)reply; 
}



/* - DELETE ------------------------------------------------------------- */
const fexp* provision_release(closure* c, provision* self, const fexp* message)
{
  fexp* reply = fexp_nil;

  // parse arguments
  string* address = (string*)send(message, s_fexp_car);

  // connect to database
  fexp* error = (fexp*)send(DB, s_db_connect);
  if (error != fexp_nil) {
    return error;
  }

  // build queries
  fexp* provision_device = fexp_nil;
  fexp* provision_mac    = fexp_nil;
  fexp* provision_person = fexp_nil;
  string* s;
  s = (string*)send(String, s_string_fromwchar, 
                    self->provision_device, address->buffer);
  provision_device = (fexp*)send(provision_device, s_fexp_cons, s);
  s = (string*)send(String, s_string_fromwchar, 
                    self->provision_mac, address->buffer);
  provision_mac    = (fexp*)send(provision_mac, s_fexp_cons, s);
  s = (string*)send(String, s_string_fromwchar, 
                    self->provision_person, address->buffer);
  provision_person = (fexp*)send(provision_person, s_fexp_cons, s);

  // get provision:<ip>:device - TODO - pass in device rather than address 
  string* device = (string*)send(DB, s_db_get, provision_device);

  // del provision:<device.id>:handset
  if (device != (string*)fexp_nil) {
    fexp* provision_handset = fexp_nil;
    s = (string*)send(String, s_string_fromwchar,
                      self->provision_handset, device->buffer);
    provision_handset = (fexp*)send(provision_handset, s_fexp_cons, s);
    send(DB, s_db_del, provision_handset);
  } else {
    wprintl(L"could not resolve device id for: %S", address->buffer);
  }

  // del provision:<ip>:device
  send(DB, s_db_del, provision_device);

  // del provision:<ip>:mac
  send(DB, s_db_del, provision_mac);

  // del provision:<ip>:person
  send(DB, s_db_del, provision_person);

 done:
  // clean up
  send(DB, s_db_close);
  
  // send back id, mac, ip, root
  return reply; // TODO - pull from site config
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
