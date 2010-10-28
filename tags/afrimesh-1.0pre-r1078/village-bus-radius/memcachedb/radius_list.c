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

#include <json_cgi.h> /* for log_message */
#include <libmemcached/memcached.h>
#include "village-bus-radius.h"


void radius_list_memcachedb()
{
  memcached_st *memcache;
  memcached_server_st *servers;
  memcached_return rc;

  char key[MEMCACHED_MAX_KEY];
  char value[1024];
  char return_key[MEMCACHED_MAX_KEY];
  char *return_value;
  size_t return_key_length;
  size_t return_value_length;
  uint32_t flags;

  char* username = NULL;
  char* cleartext_password = NULL;
  char* max_prepaid_session = NULL;
  int is_prepaid = 0;

  memcache = memcached_create(NULL);
  servers = memcached_servers_parse(radius_mysql_server);
  rc = memcached_server_push(memcache, servers);
  if (MEMCACHED_SUCCESS != rc){
    return;
  }
  rc= memcached_rget(memcache, "usr_", 4, "v", 1, 0, 0, 4);
 
  int counter = 0;

  while ((return_value= memcached_fetch(memcache, return_key, &return_key_length, &return_value_length, &flags, &rc))) {
    memcpy(key, return_key, return_key_length);
    key[return_key_length] = '\0';
    memcpy(value, return_value, return_value_length);
    value[return_value_length] = '\0';
    if (counter != 0) {
      printf(", ");
    }
    counter++;
    struct json_object* entries = json_tokener_parse(value);
    int i;

    for (i = 0; i < json_object_array_length(entries); i++) {
      struct json_object* thisentry = json_object_array_get_idx(entries, i);
      char* thisattribute = json_object_get_string(json_object_object_get(thisentry, "attribute"));
     
      if (strcasecmp(thisattribute, "Username") == 0) {
        username = json_object_get_string(json_object_object_get(thisentry, "value"));
        strncpy (username, username + 4, (strlen(username) - 4));
        username[strlen(username) - 4] = '\0';
      } else if (strcasecmp(thisattribute, "ClearText-Password") == 0) {
        cleartext_password = json_object_get_string(json_object_object_get(thisentry, "value"));
      } else if (strcasecmp(thisattribute, "Max-Prepaid-Session") == 0) {
        is_prepaid = 1;
        max_prepaid_session = json_object_get_string(json_object_object_get(thisentry, "value"));
      } else {
        log_message("Unknown attribute: %s %s %s\n", username, thisattribute, json_object_get_string(json_object_object_get(thisentry, "value")));
      }
    }
    if (is_prepaid == 1) {
      is_prepaid = 0;
      print_customer(username, cleartext_password, max_prepaid_session);
    } else {
      print_customer(username, cleartext_password, NULL);
    }
    free(return_value);
  }

  memcached_server_list_free(servers);
  memcached_free(memcache);
}

