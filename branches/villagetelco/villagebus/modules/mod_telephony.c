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


#include "mod_telephony.h"
#include "mod_sys.h" // for exec_parsed - TODO split into a common dependency


/* - telephony ----------------------------------------------------------------- */
vtable*    telephony_vt = 0;
object*    _Telephony   = 0;
telephony* Telephony    = 0;
symbol*    s_telephony  = 0;
symbol*    s_telephony_sip = 0;

void telephony_init() 
{
  telephony_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(telephony_vt, s_addMethod, s_print, telephony_print);
  send(telephony_vt, s_addMethod, s_villagebus_evaluate, telephony_evaluate);
  _Telephony = send(telephony_vt, s_allocate, 0);

  // register local symbols
  s_telephony_sip = (symbol*)symbol_intern(0, _Telephony, L"sip"); 
  send(telephony_vt, s_addMethod, s_telephony_sip, telephony_sip_asterisk);

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
  /*
  // evaluate request 
  const Request* request = VillageBus->request;
  switch (request->method) {
    case PUT:
    message = telephony_put(c, self, message, request->data);
    break;
  case GET:
    message = telephony_get(c, self, message);
    break;
  default:
    message = (fexp*)send(VillageBus, 
                          s_villagebus_error, 
                          L"mod_telephony has no registered handler for request method: %d", 
                          request->method);  // TODO method_to_string 
  }

  return message;*/
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
  const Request* request = VillageBus->request; 

  // get parameters
  char* command = (char*)send(send(message, s_fexp_car), s_string_tochar);
  if (request->json) {
    const char* param = json_object_get_string(json_object_object_get(request->json, "command"));
    command = param ? (char*)param : command;
  }

  char* argv[3];
  argv[0] = "asterisk";
  argv[1] = "-rx";
  argv[2] = 0;
  if (strncasecmp(command, "peers", 5) == 0) {
    argv[2] = "sip show peers";
    return exec_parsed(argv[0], argv, asterisk_sip_peers_parser);
  }

  return (fexp*)send(VillageBus, s_villagebus_error, L"Unknown asterisk sip command: %s", command);
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
      strcasestr (line, "[Monitored:")       != NULL)  {
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

