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
      //printf("Queried package: %s -> %d\n", *p, num_sections);
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
#ifndef DISABLE_SNMP
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
  if (strncasecmp(name, "get", 3) == 0 &&
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
  if (strncasecmp(name, "get", 3) == 0) {
    struct json_object* get = snmp_get(session, oid);
    if (!get) {
      int liberr, syserr;
      char* errstr;
      snmp_error(session, &liberr, &syserr, &errstr);
      return jsonrpc_error("net-snmp error: %s", errstr);      
    }
    json_object_object_add(response, "result", get);

  } else if (strncasecmp(name, "walk", 4) == 0) {
    struct json_object* walk = snmp_walk(session, json_object_get_string(oid));
    if (!walk) {
      int liberr, syserr;
      char* errstr;
      snmp_error(session, &liberr, &syserr, &errstr);
      return jsonrpc_error("net-snmp error: %s", errstr);
    }
    json_object_object_add(response, "result", walk);

  } else {
    return jsonrpc_error("/snmp: %s unknown command", name);
  }

  /* cleanup */
  snmp_stop(session);

  return response;
}
#else
struct json_object* jsonrpc_dispatch_snmp(const char* name, struct json_object* arguments)
{
  const char* message = "SNMP has been disabled in this build. Please recompile without the DISABLE_SNMP flag.";
  log_message(message);
  return jsonrpc_error(message);
}
#endif

/** - sys --------------------------------------------------------------- */

struct json_object* jsonrpc_dispatch_sys_syslog(const char* name, struct json_object* arguments)
{
  /* dispatch request */
  int count = json_object_get_int(json_object_array_get_idx(arguments, 0));
  return sys_syslog(count);
}

struct json_object* jsonrpc_dispatch_sys_uname(const char* name, struct json_object* arguments)
{
  char* argv[3];
  argv[0] = "uname";
  argv[1] = "-srm";
  argv[2] = 0;
  return sys_exec(argv[0], argv); /* TODO - add flags for arguments ? */
}

struct json_object* jsonrpc_dispatch_sys_version(const char* name, struct json_object* arguments)
{
  return sys_version();
}

struct json_object* jsonrpc_dispatch_sys_upgrade(const char* name, struct json_object* arguments)
{
  char* filename = json_object_get_string(json_object_array_get_idx(arguments, 0));
  // TODO - deprecate
  return NULL;
}

struct json_object* jsonrpc_dispatch_sys_service(const char* name, struct json_object* arguments)
{
  char* service = json_object_get_string(json_object_array_get_idx(arguments, 0));
  char* command = json_object_get_string(json_object_array_get_idx(arguments, 1));
  char path[32];
  snprintf(path, 32, "/etc/init.d/%s", service);
  char* argv[3];
  argv[0] = path; 
  argv[1] = command;
  argv[2] = 0;

  if (strncasecmp(command, "reload", 6) == 0) {
    return sys_exec(argv[0], argv);
  } 

  return jsonrpc_error("%s: %s unknown command", command, service);  
}


/** ipkg ----------------------------------------------------------------- */
struct json_object* jsonrpc_dispatch_ipkg_update (const char* name, struct json_object* arguments)
{
  char* argv[3];
  argv[0] = "opkg";
  argv[1] = "update";
  argv[2] = 0;
  return sys_exec(argv[0], argv);
}


struct json_object* ipkg_list_exec_parser(const char* line, size_t length)
{
  char* name;
  char* version;
  char* comment;
  char* cursor = line;
  cursor = parse_field(cursor, length, (char[]){' ',-1}, &name) + 2;
  cursor = parse_field(cursor, length, (char[]){' ',-1}, &version) + 2;
  cursor = parse_field(cursor, length, (char[]){'\n',-1}, &comment);
  struct json_object* package = json_object_new_object();
  json_object_object_add(package, "name", json_object_new_string(name));
  json_object_object_add(package, "version", json_object_new_string(version));
  json_object_object_add(package, "comment", json_object_new_string(comment));
  return package;
}

struct json_object* jsonrpc_dispatch_ipkg_list   (const char* name, struct json_object* arguments)
{ 
  char* argv[3];
  argv[0] = "opkg";
  argv[1] = "list_upgradable";
  argv[2] = 0;
  return sys_exec_parsed(argv[0], argv, ipkg_list_exec_parser);
}

struct json_object* jsonrpc_dispatch_ipkg_status (const char* name, struct json_object* arguments)
{
  return NULL;
}

struct json_object* jsonrpc_dispatch_ipkg_upgrade(const char* name, struct json_object* arguments)
{
  char* argv[4];
  argv[0] = "opkg";
  argv[1] = "upgrade";
  argv[2] = json_object_get_string(json_object_array_get_idx(arguments, 0));
  argv[3] = 0;
  return sys_exec(argv[0], argv);
}

/** accounting ----------------------------------------------------------- */
struct json_object* acct_pmacct_exec_parser(const char* line, size_t length)
{
  ((char*)line)[length - 1] = '\0';   // TODO - supremely evil - fix when there's time to debug

  /* parse header */
  static struct json_object* header_fields = NULL;
  if (header_fields == NULL) {
    header_fields = json_object_new_array();
    char* field = line;
    while (field = strtok(field, " ")) {
      json_object_array_add(header_fields, json_object_new_string(field));
      field = NULL;
    }
    return NULL;
  }

  /* lose junk lines */
  if (length <= 1 ||
      strncasecmp(line, "For a total", 11) == 0) {
    return NULL;
  }
  
  /* parse line */
  struct json_object* entry = json_object_new_object();
  char*  field = line;
  size_t count = 0;
  while ((field = strtok(field, " ")) && 
         count < json_object_array_length(header_fields)) {
    struct json_object* field_name = json_object_array_get_idx(header_fields, count);
    json_object_object_add(entry,
                           json_object_get_string(field_name),  
                           json_object_new_string(field));
    field = NULL;
    count++;
  }

  return entry;
}

struct json_object* jsonrpc_dispatch_acct_gateway(const char* name, struct json_object* arguments)
{
  char* argv[5];
  argv[0] = "pmacct";
  argv[1] = "-s";
  argv[2] = "-p";
  argv[3] = "/tmp/in.pipe";
  argv[4] = 0;
  return sys_exec_parsed(argv[0], argv, acct_pmacct_exec_parser);
}


/** voip ----------------------------------------------------------------- */
struct json_object* voip_sip_peers_parser(const char* line, size_t length)
{
  char* name;
  char* host;
  char* port;
  char* status;
  char* latency;
  char* cursor = line;
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

struct json_object* jsonrpc_dispatch_voip_sip(const char* name, struct json_object* arguments)
{
  char* command  = json_object_get_string(json_object_array_get_idx(arguments, 0));
  char* argv[4];
  argv[0] = "asterisk";
  argv[1] = "-rx";
  argv[2] = 0;
  argv[3] = 0;
  if (strncasecmp(command, "show peers", 10) == 0) {
    argv[2] = "sip show peers";
    return sys_exec_parsed(argv[0], argv, voip_sip_peers_parser);
  }
  return jsonrpc_error("sip: %s: unknown command", command);
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
