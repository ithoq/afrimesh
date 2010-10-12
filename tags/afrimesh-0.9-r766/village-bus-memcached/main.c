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


#include "village-bus-memcached.h"


void httpd_out(const char* message, ...)
{
  va_list args;
  va_start(args, message);
  vprintf(message, args);
  vlog_message(message, args);
  va_end(args);
}




int main(int argc, char** argv)
{
  log_message("");
  httpd_out("Content-type: application/json\n\n");
  /*httpd_out("Content-type: text/plain\n\n");*/

  /* parse request */
  char* request = json_cgi_request(); 
  if (request == NULL || strcasecmp(request, "") == 0) { /* TODO */
    request = "{ \"servers\"  : \"localhost\", \"commands\" : [ ]";
  }

  struct json_object* request_object = json_tokener_parse(request);
  if (request_object == NULL) {
    log_message("Could not parse request: %s\n", request);
    httpd_out("[\n\t{ error: \"Could not parse request: %s\" }\n]\n", request);
    json_cgi_release();
    return EXIT_FAILURE;
  }

  /* parse options */
  char* servers = json_object_get_string(json_object_object_get(request_object, "servers"));
  if (servers == NULL) { 
    httpd_out("[ { error: \"invalid servers\" } ]"); 
    return EXIT_FAILURE;  
  }

  /* initialize libmemcached */
  memcached_st* memcached;
  memcached = memcached_create(NULL);

  /* set server list */
  memcached_server_st* server_list = memcached_servers_parse(servers);
  memcached_server_push(memcached, server_list);
  memcached_server_list_free(server_list);

 
 /* process set */
  struct json_object* set_object = json_object_object_get(request_object, "set");
  /*log_message("\nSet: |%s|\n\n", json_object_get_string(set_object));*/
  httpd_out("[ "); 
  if (set_object) {
    struct json_object* out_object = json_object_new_object();
    json_object_object_foreach(set_object, json_key, json_value) {
      char* set_value = json_object_to_json_string(json_value);
      char* memcached_value = set(memcached, json_key, set_value);
      if (memcached_value != set_value) {
        json_key = "error";
      }
      json_object_object_add(out_object, json_key, json_tokener_parse(memcached_value));
    }
    httpd_out("%s", json_object_to_json_string(out_object));
    json_object_put(out_object);      
  }


  /* process get */
  struct json_object* get_object = json_object_object_get(request_object, "get");
  /*log_message("Get: %s\n", json_object_get_string(get_object));*/
  if (get_object && set_object) {
    httpd_out(", ");
  } 
  if (get_object) {
    struct json_object* out_object = json_object_new_object();
    json_object_object_foreach(get_object, property, memcached_key) {
      char* memcached_value = get(memcached, json_object_get_string(memcached_key));
      json_object_object_add(out_object, property, json_tokener_parse(memcached_value));
      free(memcached_value);
    }
    httpd_out("%s", json_object_to_json_string(out_object));
    json_object_put(out_object);      
  }
  
  httpd_out(" ]\n");

  /* release resources */
  json_object_put(request_object);
  json_cgi_release();

  return EXIT_SUCCESS;
}


