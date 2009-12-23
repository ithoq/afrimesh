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


/**
 * JSON/RPC method registration and interface specification
 */
struct MethodDispatch dispatch_table[] = {
  { "/uci",    "show",    {json_type_string}, 1, jsonrpc_dispatch_uci_show },
  { "/uci",    "set",     {json_type_array }, 1, jsonrpc_dispatch_uci_set  },
  { "/snmp",   "get",     {json_type_string,  json_type_string, json_type_array }, 3, jsonrpc_dispatch_snmp   },
  { "/snmp",   "walk",    {json_type_string,  json_type_string, json_type_string}, 3, jsonrpc_dispatch_snmp   },
  { "/sys",    "syslog",  {json_type_int},    1, jsonrpc_dispatch_sys_syslog  },
  { "/sys",    "uname",   {},                 0, jsonrpc_dispatch_sys_uname   },
  { "/sys",    "version", {},                 0, jsonrpc_dispatch_sys_version },
  { "/sys",    "service", {json_type_string, json_type_string}, 2, jsonrpc_dispatch_sys_service },
  { "/ipkg",   "update",  {},                 0, jsonrpc_dispatch_ipkg_update },
  { "/ipkg",   "list",    {},                 0, jsonrpc_dispatch_ipkg_list },
  { "/ipkg",   "status",  {},                 0, jsonrpc_dispatch_ipkg_status },
  { "/ipkg",   "upgrade", {json_type_string}, 1, jsonrpc_dispatch_ipkg_upgrade },
  { "/voip",   "sip",     {json_type_string}, 1, jsonrpc_dispatch_voip_sip },
  { 0, 0, 0, 0 }
};


/**
 * 
 */
int exit_failure() 
{
  json_cgi_release();
  return EXIT_FAILURE;
}


/**
 *
 */
int main(int argc, char** argv)
{
  /** - output HTTP header ------------------------------------------------ */
  httpd_out("Content-type: application/json\n\n");
  //httpd_out("Content-type: text/plain\n\n");

  /** - read request ------------------------------------------------------ */
  const char* request = cgi_request(argc, argv);
  if (request == NULL || strcmp(request, "") == 0) { 
    httpd_error("Null or empty request - %s", request);
    return exit_failure();
  }

  /** - get request path -------------------------------------------------- */
  const char* request_path = "";
  if (argc == 3 && argv && argv[1]) {
    request_path = argv[1];
  } else if (getenv("PATH_INFO")) {
    request_path = getenv("PATH_INFO");
  } else {
    request_path = NULL;
  }

  log_message("-> %s\n", request);
  if (request_path) log_message("-> path: %s\n", request_path);

  /** - parse request ----------------------------------------------------- */
  struct json_object* request_object = json_tokener_parse(cgi_decode(request, strlen(request)));
  if (request_object == NULL || is_error(request_object)) {
    log_message("-> CGI (%s)\n", json_tokener_errors[-(unsigned long)request_object]);
    request_object = json_tokener_parse(request_to_json(request, strlen(request)));
    if (request_object == NULL || is_error(request_object)) {
      httpd_error("JSON error - %s", (const char*)json_tokener_errors[-(unsigned long)request_object]);
      return exit_failure();
    } 
  }

  /** - handle jsonp ----------------------------------------------------- */
  struct json_object* jsonp = json_object_object_get(request_object, "jsonp");
  char* jsonp_name = (jsonp ? json_object_get_string(jsonp) : "callback");
  char* jsonp_callback = json_object_get_string(json_object_object_get(request_object, jsonp_name));
  /* httpd_error("No jsonp key named '%s' found. You can specify alternate callback keys by adding '&jsonp=altkey' to your URL", jsonp_name, request); */

  /** - read request payload --------------------------------------------- */
  struct json_object* payload_object = NULL;
  if (jsonp_callback != NULL) { /** - request is JSONP ------------------- */
    char* json_payload = json_object_get_string(json_object_object_get(request_object, "payload"));
    if (json_payload == NULL) {
      httpd_error("JSONP request missing payload key.");
      return exit_failure();
    } 
    payload_object = json_tokener_parse(json_payload);
    if (payload_object == NULL || is_error(payload_object)) {
      httpd_error("JSONP payload error - %s", (const char*)json_tokener_errors[-(unsigned long)payload_object]);
      return exit_failure();
    } 
  } else {                      /** - request is JSON --------------------- */
    payload_object = request_object;
  }
  /* log_message("GOT PAYLOAD: %s\n", json_object_get_string(payload_object)); */
  if (payload_object == NULL) {
    httpd_error("Could not determine payload in request: %s", request);
    return exit_failure();
  }

  /** - handle response ------------------------------------------------- */
  int jsonrpc_id = json_object_get_int(json_object_object_get(payload_object, "id"));
  char* jsonrpc_version = json_object_get_string(json_object_object_get(payload_object, "version"));
  char* jsonrpc_method  = json_object_get_string(json_object_object_get(payload_object, "method"));
  struct json_object* jsonrpc_params  = json_object_object_get(payload_object, "params");
  if (jsonrpc_version && jsonrpc_method && jsonrpc_params) { /** - request is JSON/RPC --- */
    log_message("-> JSON/RPC\n");
    log_message("\tid: %u\n",      jsonrpc_id);
    log_message("\tversion: %s\n", jsonrpc_version);
    log_message("\tmodule: %s\n",  request_path);
    log_message("\tmethod: %s\n",  jsonrpc_method);
    log_message("\tparams: %s\n",  json_object_get_string(jsonrpc_params));

    /* dispatch request */
    struct json_object* jsonrpc_response = jsonrpc_dispatch(dispatch_table, request_path, jsonrpc_method, jsonrpc_params);

    /* return request response */
    json_object_object_add(jsonrpc_response, "id",     json_object_new_int(jsonrpc_id));
    httpd_out("%s\n", json_object_get_string(jsonrpc_response));

  } else if (jsonp_callback) {                               /** - request is JSONP --- */
    log_message("-> JSONP\n");
    httpd_out("%s(%s)\n", jsonp_callback, json_object_get_string(payload_object));  

  } else {                                                   /** - request is JSON ---- */
    log_message("-> JSON\n");
    httpd_out("%s\n", json_object_get_string(payload_object));  

  }

  /** - release resources -------------------------------------------------- */
  json_object_put(payload_object);
  json_object_put(request_object);
  json_cgi_release();

  return EXIT_SUCCESS;
}



