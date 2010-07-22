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


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "mod_snmp.h"


/* - snmp ----------------------------------------------------------------- */
vtable* snmp_vt = 0;
object* _SNMP   = 0;
snmp*   SNMP    = 0;
symbol* s_snmp  = 0;
//symbol* s_snmp_ = 0;

void snmp_init() 
{
  snmp_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(snmp_vt, s_addMethod, s_print, snmp_print);
  send(snmp_vt, s_addMethod, s_villagebus_evaluate, snmp_evaluate);
  _SNMP = send(snmp_vt, s_allocate, 0);

  // register local symbols
  //s_snmp_ = (symbol*)symbol_intern(0, _SNMP, L"_"); 
  //send(snmp_vt, s_addMethod, s_snmp_, snmp_);

  // global module instance vars
  SNMP = (snmp*)send(_SNMP->_vt[-1], s_allocate, sizeof(snmp));

  // register module with VillageBus
  s_snmp = (symbol*)symbol_intern(0, 0, L"snmp");
  fexp* module = (fexp*)send(Fexp, s_new, s_snmp, SNMP);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}


const fexp* snmp_evaluate(closure* c, snmp* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // search for name in local context
  const string* name    = (string*)send(expression, s_fexp_car);
  const fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _SNMP, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 

  // evaluate request 
  Request* request = VillageBus->request;
  switch (request->method) {
  case GET:
    message = snmp_get(c, self, expression);
    break;
  default:
    message = (fexp*)send(VillageBus, 
                          s_villagebus_error, 
                          L"mod_snmp has no registered handler for request method: %d", 
                          request->method);  // TODO method_to_string 
  }

  return message;
}


snmp* snmp_print(closure* c, snmp* self)
{
  wprintf(L"#<SNMP.%p>", self);
  return self;
}



/* - names -------------------------------------------------------------- */

/**
 *
 */
#ifdef ENABLE_MOD_SNMP
const fexp* snmp_get(closure* c, snmp* self, const fexp* message)
{
  Request* request = VillageBus->request; 

  size_t i;
  struct snmp_session* session;

  // get parameters
  char* name      = (char*)send(send(message, s_fexp_nth, 0), s_string_tochar);
  char* address   = (char*)send(send(message, s_fexp_nth, 1), s_string_tochar);
  char* community = (char*)send(send(message, s_fexp_nth, 2), s_string_tochar);
  fexp* oids      = (fexp*)send(message, s_fexp_nthcdr, 3);
  wprintl(L"GET /snmp %s %s:%s %s\n", name, address, community, send(send(oids, s_tojson, false), s_string_tochar));

  /* TODO - typecheck request arguments */
  /*if (strncasecmp(name, "get", 3) == 0 &&
    !json_typecheck_array(oid, json_type_string)) {
    return jsonrpc_error("/snmp.get (%s) expected (String, String, [String])", 
    params_tostring(arguments));
    }*/
  
  /* initialize snmp */
  session = start_snmp(address, community);
  if (!session) {
    int   liberr, syserr;
    char* errstr;
    snmp_error(session, &liberr, &syserr, &errstr);
    return (fexp*)send(VillageBus, s_villagebus_error, L"Failed to initialize net-snmp: %s", errstr);
  }

  /* dispatch request */
  struct json_object* response = NULL;
  if (strncasecmp(name, "get", 3) == 0) {
    struct json_object* oids_json = json_tokener_parse((char*)send(send(oids, s_tojson, false), s_string_tochar));
    response = snmpget(session, oids_json);

  } else if (strncasecmp(name, "walk", 4) == 0) {
    char* oid = (char*)send(send(oids, s_fexp_car), s_string_tochar);
    response = snmpwalk(session, oid);

  } else {
    return (fexp*)send(VillageBus, s_villagebus_error, L"net-snmp unknown name: %s", name);
  }

  if (!response) {
    int liberr, syserr;
    char* errstr;
    snmp_error(session, &liberr, &syserr, &errstr);
    return (fexp*)send(VillageBus, s_villagebus_error, L"net-snmp %s error: %s", name, errstr);
  }

  /* output response */
  request->out(request, L"%s", json_object_get_string(response));

  /* cleanup */
  stop_snmp(session);

  //return response;
  return fexp_nil;
}
#else
const fexp* snmp_get(closure* c, snmp* self, const fexp* message)
{
  const wchar_t* error = L"SNMP has been disabled in this build. Please recompile with the ENABLE_MOD_SNMP flag.";
  return (fexp*)send(VillageBus, s_villagebus_error, L"%S", error);
}
#endif // ENABLE_MOD_SNMP


/**
 *
 */
const fexp* snmp_(closure* c, snmp* self, const fexp* message)
{
  return fexp_nil;
}


/* - SNMP helpers ------------------------------------------------------- */

/**
 * Initialize snmp session
 */
struct snmp_session* start_snmp(const char* address, const char* community)
{
  struct snmp_session session, *sessionp;
  init_snmp("village-bus-snmp");
  snmp_enable_syslog(); 
  snmp_sess_init(&session);            
  session.peername = (char*)address;
  session.version = SNMP_VERSION_1;
  session.community = (unsigned char*)community;
  session.community_len = strlen(community);
  sessionp = snmp_open(&session); 
  if (!sessionp) {
    return NULL;
  } 
  SOCK_STARTUP;
  return sessionp;
}


/** 
 * Clean up snmp session
 */
void stop_snmp(struct snmp_session* session)
{
  snmp_close(session);
  SOCK_CLEANUP;
}


/** 
 * Perform a snmpget on an array of oids
 */
struct json_object* snmpget(struct snmp_session* session, struct json_object* oids)
{
  struct snmp_pdu *pdu, *response;
  struct variable_list *variable;
  struct json_object* result;
  int status, count;
  char buf[1024];
  oid name[MAX_OID_LEN];
  count = json_object_array_length(oids);
  size_t name_len;

  /* initialize our result object */
  result = json_object_new_object();

  /* build query */
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  for (count = 0; count < json_object_array_length(oids); count++) {
    name_len = MAX_OID_LEN;
    read_objid(json_object_get_string(json_object_array_get_idx(oids, count)), 
               name, 
               &name_len);
    snmp_add_null_var(pdu, name, name_len);
  }

  /* perform query */
  status = snmp_synch_response(session, pdu, &response); // TODO - better error logging
  if (status != STAT_SUCCESS)  {
    if (response && response->errstat != SNMP_ERR_NOERROR) {
      wprintl(L"Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
      snmp_free_pdu(response);
    }
    return NULL;
  }

  /* print query results */
  count = 0;
  for (variable = response->variables; variable; variable = variable->next_variable) {
    json_object_snmp_add(result, variable, count);
    count++;
  }
  
  if (response) {
    snmp_free_pdu(response);
  }

  return result;
}


/**
 * Perform a snmpwalk on each element in an array of oids
 */
struct json_object* snmpwalk(struct snmp_session* session, const char* name_oid)
{
  struct snmp_pdu *pdu, *response;
  struct variable_list* variable;
  struct json_object* result;
  int status, running, count;
  char buf[1024];
  oid name[MAX_OID_LEN], root[MAX_OID_LEN];
  size_t name_length, root_length;

  /* initialize our result object */
  result = json_object_new_object();
  
  /* set the root oid from which we'll start our meander */
  root_length = MAX_OID_LEN;
  read_objid(name_oid, root, &root_length);
  memmove(name, root, root_length * sizeof(oid));
  name_length = root_length;
  
  /* take a stroll down the mib */
  running = 1;
  count = 0;
  while (running) {
    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
    snmp_add_null_var(pdu, name, name_length);
    status = snmp_synch_response(session, pdu, &response); // TODO - better error logging
    if (status != STAT_SUCCESS)  {
      if (response && response->errstat != SNMP_ERR_NOERROR) {
        wprintl(L"Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
        snmp_free_pdu(response);
      }
      return NULL;
    }

    /* check if we've reached the end of the trail */
    for (variable = response->variables; variable; variable = variable->next_variable) {
      if ((variable->name_length < root_length) || 
          (memcmp(root, variable->name, root_length * sizeof(oid)) != 0)) { /* fin */
        running = 0; 
        continue;
      }
        
      /* add current node to result */
      json_object_snmp_add(result, variable, -1);
      count++;

      if ((variable->type != SNMP_ENDOFMIBVIEW) && 
          (variable->type != SNMP_NOSUCHOBJECT) && 
          (variable->type != SNMP_NOSUCHINSTANCE)) {
        if (snmp_oid_compare(name, name_length, variable->name, variable->name_length) >= 0) {
          wprintl(L"Error: OID not increasing: "); 
          snprint_objid(buf, 1024, name, name_length);
          wprintl(L"%s >= ", buf);
          snprint_objid(buf, 1024, variable->name, variable->name_length);
          wprintl(L"%s\n", buf);
          running = 0;
        }
        memmove((char*)name, (char*)variable->name, variable->name_length * sizeof(oid));
        name_length = variable->name_length;
      } else {
        wprintl(L"Unknown exception\n");
        running = 0;
      }
    }

    if (response) {
      snmp_free_pdu(response);
    }
  } /* while running */

  return result;
}



/* - utilities ---------------------------------------------------------- */

/**
 * Convert a SNMP oid to something more json friendly
 *
 * TODO: Decide on whether to deprecate and force 
 *       other side to address by string subscript ?
 */
char* escape_oid(char* name) 
{
  /*size_t t;
  for (t = 0; t < 1024; t++) {
    if      (name[t] ==   0) break;
    else if (name[t] == '.') name[t] = '_';
    else if (name[t] == ':') name[t] = '_';
    else if (name[t] == '-') name[t] = '_';
  }*/
  return name;
}


/**
 * TODO - there's an annoying bug when performing a snmp query in snmp_get that expects
 *        an ASN_OCTET_STR back but actually gets an ASN_NULL :-P 
 */
void json_object_snmp_add(struct json_object* object, struct variable_list* variable, int index)
{
  struct json_object* value;
  char buf[1024];
  if (variable->type == ASN_OCTET_STR) {
    snprintf(buf, variable->val_len + 1, "%s", variable->val.string);
    value = json_object_new_string(buf);
  } else if (variable->type == ASN_COUNTER) { 
    sprintf(buf, "%lu", *variable->val.integer);
    value = json_object_new_string(buf);
    //value = json_object_new_int(*variable->val.integer); // TODO ASN_COUNTER overflows json integer 
  } else if (variable->type == ASN_NULL) {   
    value = NULL;
  } else {
    sprintf(buf, "unknown type: 0x%x", variable->type);
    value = json_object_new_string(buf);
  }

  // by name
  snprint_objid(buf, 1024, variable->name, variable->name_length);
  json_object_object_add(object, escape_oid(buf), value);
  
  // by numeric index
  if (index != -1) {
    snprintf(buf, 1024, "%d", index);
    json_object_object_add(object, buf, value);
  }
}

