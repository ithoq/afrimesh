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


#include "village-bus.h"


/** - uci --------------------------------------------------------------- */

/**
 * Handle request dispatch for uci module
 */
struct json_object* jsonrpc_dispatch_uci_show(const char* name, struct json_object* arguments)
{
  /* initialize uci */
  UCI_CONTEXT = uci_alloc_context();
  if (UCI_CONTEXT == NULL) {
    return jsonrpc_error("Failed to initialize uci: Out of memory");
  }

  /* dispatch request */
  struct json_object* result = json_object_new_object();
  char* config = json_object_get_string(json_object_array_get_idx(arguments, 0));
  if (config && strcasecmp(config, "") != 0) {
    int num_sections = _uci_show_package(config, result);
  } else {
    char** configs = NULL;
    if ((uci_list_configs(UCI_CONTEXT, &configs) != UCI_OK) || !configs) {
      uci_perror(UCI_CONTEXT, "village-bus-uci");
      return jsonrpc_error("TODO: UCI error");
    }
    char** p;
    for (p = configs; *p; p++) {
      int num_sections = _uci_show_package(*p, result);
      //log_message("Queried package: %s -> %d\n", *p, num_sections);
    }
  }
  struct json_object* response = json_object_new_object();
  json_object_object_add(response, "result", result);

  /* cleanup */
  if (UCI_CONTEXT) {
    uci_free_context(UCI_CONTEXT);
  }

  return response;
}


struct json_object* jsonrpc_dispatch_uci_set(const char* name, struct json_object* arguments)
{
  /* initialize uci */
  UCI_CONTEXT = uci_alloc_context();
  if (UCI_CONTEXT == NULL) {
    return jsonrpc_error("Failed to initialize uci: Out of memory");
  }

  /* dispatch request */
  struct json_object* result = json_object_new_object();
  struct json_object* settings = json_object_array_get_idx(arguments, 0);
  size_t t;
  for (t = 0; t < json_object_array_length(settings); t++) {   /* We're expecting: [ { config, section, option, value }, {..} ] */
    /* typecheck setting */
    struct json_object* setting = json_object_array_get_idx(settings, t);
    struct json_object* config  = json_object_object_get(setting, "config");
    struct json_object* section = json_object_object_get(setting, "section");
    struct json_object* option  = json_object_object_get(setting, "option");
    struct json_object* value   = json_object_object_get(setting, "value");
    if (!json_typecheck(setting, json_type_object)  ||
        !json_typecheck(config,  json_type_string)  ||
        !json_typecheck(section, json_type_string)  ||
        !json_typecheck(option,  json_type_string)  ||
        !json_typecheck(value,   json_type_string)) {
      return jsonrpc_error("/uci.set (%s) expected ([{config:String, section:String, option:String, value:String}, {..}])", 
                           params_tostring(arguments));
    }

    /* set it */
    if (!uci_set_config(json_object_get_string(config), 
                        json_object_get_string(section), 
                        json_object_get_string(option), 
                        json_object_get_string(value))) {
      return jsonrpc_error("/uci.set (%s) - could not set: %s.%s.%s=%s", 
                           params_tostring(arguments),
                           json_object_get_string(config), 
                           json_object_get_string(section), 
                           json_object_get_string(option), 
                           json_object_get_string(value));
    }
    /* TODO - consider making this return value just a tad more consistent with what we're passing in no? */
    json_object_object_add(result, json_object_get_string(option), value);

  } 
  struct json_object* response = json_object_new_object();
  json_object_object_add(response, "result", result);
  json_object_object_add(response, "error",  NULL);

  /* cleanup */
  if (UCI_CONTEXT) {
    uci_free_context(UCI_CONTEXT);
  }

  return response;
}


/** - snmp -------------------------------------------------------------- */

/**
 * Handle request dispatch for snmp module
 */
struct json_object* jsonrpc_dispatch_snmp(const char* name, struct json_object* arguments)
{
  size_t i;
  struct snmp_session* session;

  struct json_object* address   = json_object_array_get_idx(arguments, 0);
  struct json_object* community = json_object_array_get_idx(arguments, 1);
  struct json_object* oid       = json_object_array_get_idx(arguments, 2);

  /* typecheck request arguments */
  if (strncmp(name, "get", 3) == 0 &&
      !json_typecheck_array(oid, json_type_string)) {
    return jsonrpc_error("/snmp.get (%s) expected (String, String, [String])", 
                         params_tostring(arguments));
  }

  /* initialize snmp */
  session = snmp_start(json_object_get_string(address), json_object_get_string(community));
  if (!session) {
    int liberr, syserr;
    char* errstr;
    snmp_error(session, &liberr, &syserr, &errstr);
    log_message("Failed to initialize net-snmp: %s\n", errstr);
    return jsonrpc_error("Failed to initialize net-snmp: %s", errstr);
  }

  /* dispatch request */
  struct json_object* response = json_object_new_object();
  if (strncmp(name, "get", 3) == 0) {
    struct json_object* get = snmp_get(session, oid);
    if (!get) {
      int liberr, syserr;
      char* errstr;
      snmp_error(session, &liberr, &syserr, &errstr);
      return jsonrpc_error("net-snmp error: %s", errstr);      
    }
    json_object_object_add(response, "result", get);

  } else if (strncmp(name, "walk", 4) == 0) {
    struct json_object* walk = snmp_walk(session, json_object_get_string(oid));
    if (!walk) {
      int liberr, syserr;
      char* errstr;
      snmp_error(session, &liberr, &syserr, &errstr);
      return jsonrpc_error("net-snmp error: %s", errstr);
    }
    json_object_object_add(response, "result", walk);

  } else {
    return jsonrpc_error("Could not find name '%s' in module '/snmp'", name);
  }

  /* cleanup */
  snmp_stop(session);

  return response;
}


/** - sys --------------------------------------------------------------- */

struct json_object* jsonrpc_dispatch_sys_syslog(const char* name, struct json_object* arguments)
{
  /* dispatch request */
  int count = json_object_get_int(json_object_array_get_idx(arguments, 0));
  return sys_syslog(count);
}

struct json_object* jsonrpc_dispatch_sys_uname(const char* name, struct json_object* arguments)
{
  return sys_exec("uname -srm"); /* TODO - add flags for arguments */
}

struct json_object* jsonrpc_dispatch_sys_version(const char* name, struct json_object* arguments)
{
  return sys_version();
}



/**
 */
void httpd_out(const char* message, ...)
{
  log_message("<- ");
  va_list args;
  va_start(args, message);
  vprintf(message, args);
  vlog_message(message, args);
  va_end(args);
}


/**
 *
 */
void httpd_error(const char* message, ...)
{
  va_list args;
  printf("json_error_handler([ { \"error\" : \"");
  log_message("ERROR: error([ { \"error\" : \"");
  va_start(args, message);
  vprintf(message, args);
  vlog_message(message, args);
  va_end(args);  
  printf("\" } ])\n");
  log_message("\" } ])\n");
}
