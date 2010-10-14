/**
 * Afrimesh: easy management for mesh networks
 *
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


#include "mod_telephony.h"
#include "mod_sys.h" // for exec_parsed - TODO split into a common dependency


/* - telephony ----------------------------------------------------------------- */
vtable*    telephony_vt = 0;
object*    _Telephony   = 0;
telephony* Telephony    = 0;
symbol*    s_telephony  = 0;
symbol*    s_telephony_sip    = 0;
symbol*    s_telephony_callme = 0;

void telephony_init() 
{
  telephony_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(telephony_vt, s_addMethod, s_print, telephony_print);
  send(telephony_vt, s_addMethod, s_villagebus_evaluate, telephony_evaluate);
  _Telephony = send(telephony_vt, s_allocate, 0);

  // register local symbols
  s_telephony_sip    = (symbol*)symbol_intern(0, _Telephony, L"sip"); 
  s_telephony_callme = (symbol*)symbol_intern(0, _Telephony, L"callme"); 
  send(telephony_vt, s_addMethod, s_telephony_sip, telephony_sip_asterisk);
  send(telephony_vt, s_addMethod, s_telephony_callme, telephony_callme);

  // global module instance vars
  Telephony = (telephony*)send(_Telephony->_vt[-1], s_allocate, sizeof(telephony));

  // register module with VillageBus
  s_telephony = (symbol*)symbol_intern(0, 0, L"telephony");
  fexp* module = (fexp*)send(Fexp, s_new, s_telephony, Telephony);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}


const fexp* telephony_evaluate(closure* c, telephony* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _Telephony, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 

  return (fexp*)send(VillageBus, 
                     s_villagebus_error, 
                     L"mod_telephony has no registered handler for requested name: %S", 
                     name->buffer);  
}


telephony* telephony_print(closure* c, telephony* self)
{
  wprintf(L"#<TELEPHONY.%p>", self);
  return self;
}



/* - names -------------------------------------------------------------- */

/**
 *
 */
const fexp* telephony_sip_asterisk(closure* c, telephony* self, const fexp* message)
{
  Request* request = VillageBus->request; 

  // get parameters
  char* command = (char*)send(send(message, s_fexp_car), s_string_tochar);
  if (request->json) {
    const char* param = json_object_get_string(json_object_object_get(request->json, "command"));
    command = param ? (char*)param : command;
  }

  char* argv[4];
  argv[0] = "asterisk";
  argv[1] = "-rx";
  argv[2] = 0;
  argv[3] = 0;
  if (strncasecmp(command, "peers", 5) == 0) {
    argv[2] = "sip show peers";
    return exec_parsed(argv[0], argv, asterisk_sip_peers_parser);
  }

  return (fexp*)send(VillageBus, s_villagebus_error, L"Unknown asterisk sip command: %s", command);
}


/**
 */
const fexp* telephony_callme(closure* c, telephony* self, const fexp* message)
{
  // get parameters
  string* handset = (string*)send(message, s_fexp_car); 
  wprintl(L"GET /telephony/callme/%S\n", handset->buffer);

  return fexp_nil;
}


/* - utilities ---------------------------------------------------------- */
struct json_object* asterisk_sip_peers_parser(const char* line, size_t length)
{
  char* name;
  char* host;
  char* port;
  char* status;
  char* latency;
  const char* cursor = line;
  if (strncasecmp(line, "Name/username", 13) == 0 ||
      strcasestr(line, "[Monitored:")        != 0)  {
    return NULL;
  }  
  /*        1         2         3         4         5         6         7         8
  012345678901234567890123456789012345678901234567890123456789012345678901234567890  
  Name/username              Host            Dyn Nat ACL Port     Status         */
  struct json_object* peer = json_object_new_object();
  json_object_object_add(peer, "name",   json_object_new_string (trimr(cut_field(line,  0, 26))));
  json_object_object_add(peer, "host",   json_object_new_string (trimr(cut_field(line, 27, 42))));
  json_object_object_add(peer, "port",   json_object_new_string (trimr(cut_field(line, 55, 63))));
  cursor = parse_field(line + 64, length, (char[]){' ',-1}, &status);
  cursor = parse_field(cursor, length, (char[]){'(',')',' ',-1}, &latency);
  json_object_object_add(peer, "status", json_object_new_string (status));
  json_object_object_add(peer, "latency", json_object_new_string(latency));
  return peer;
}

