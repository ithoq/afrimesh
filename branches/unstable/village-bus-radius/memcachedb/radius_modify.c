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
#include "village-bus-radius.h"
#include <libmemcached/memcached.h>

/**
 * Modify radius user
 */
void radius_modify_memcachedb(const char* username, const char* new_username, const char* new_password, const char* new_type)
{ 
  char user_no_prefix [50];
  snprintf (user_no_prefix, 50, username);
  char temp[50];
  snprintf(temp, 50, "%s%s", "usr_", username);
  username = temp;
  memcached_st *memcached = memcached_create(NULL);
  memcached_server_st *servers = memcached_servers_parse(radius_mysql_server);
  memcached_return rc = memcached_server_push(memcached, servers);

  /* read user attributes */
  size_t value_length;
  uint32_t flags;
  memcached_return ret;

  char* attributes = memcached_get(memcached, username, strlen(username), &value_length, &flags, &ret);
  if (ret != MEMCACHED_SUCCESS) {
    printf("{ error : \"Failed to find user \"%s\" in memcachedb: %s\" }", username, attributes);
    return;
  }

  struct json_object* entries = json_tokener_parse(attributes);
    int i;
    int has_prepaid_attribute = 0;

  /* update relevant user information and concatenate everything into a new JSON string */
  char newval [1024] = "[";
  char tempstr [1024];

  for (i = 0; i < json_object_array_length(entries); i++) {
    struct json_object* thisentry = json_object_array_get_idx(entries, i);
    char* thisattribute = json_object_get_string(json_object_object_get(thisentry, "attribute"));
   
    if (strcasecmp(thisattribute, "Username") == 0) {
      snprintf(tempstr, 1024, newval);
      if (new_username) {
        snprintf(newval, 1024, "%s{ \"attribute\" : \"Username\", \"operation\" : \":=\", \"value\" : \"%s%s\" },", 
          tempstr, "usr_", new_username);
      } else {
        snprintf(newval, 1024, "%s{ \"attribute\" : \"Username\", \"operation\" : \":=\", \"value\" : \"%s%s\" },", 
          tempstr, "usr_", user_no_prefix);
      }
    } else if (strcasecmp(thisattribute, "ClearText-Password") == 0) {
      snprintf(tempstr, 1024, newval);
      char *cleartext_password = json_object_get_string(json_object_object_get(thisentry, "value"));
      if (new_password) {
        snprintf(newval, 1024, "%s{ \"attribute\" : \"ClearText-Password\", \"operation\" : \":=\", \"value\" : \"%s\" },", 
         tempstr, new_password);
      } else {
        snprintf(newval, 1024, "%s{ \"attribute\" : \"ClearText-Password\", \"operation\" : \":=\", \"value\" : \"%s\" },", 
          tempstr, cleartext_password);
      }
    } else if (strcasecmp(thisattribute, "Max-Prepaid-Session") == 0) {
        has_prepaid_attribute = 1;
        if ((strcasecmp(new_type, "flatrate") != 0) && (strcasecmp(new_type, "disabled") != 0)) {
          snprintf(tempstr, 1024, newval);
          char *max_prepaid_session = json_object_get_string(json_object_object_get(thisentry, "value"));
          snprintf(newval, 1024, "%s{ \"attribute\" : \"Max-Prepaid-Session\", \"operation\" : \":=\", \"value\" : \"%s\" }", 
              tempstr, max_prepaid_session);
        }
    }
  }

  if (new_type && (strcasecmp(new_type, "prepaid") == 0) && (has_prepaid_attribute == 0)) {
    snprintf(tempstr, 1024, newval);
    snprintf(newval, 1024, "%s{ \"attribute\" : \"Max-Prepaid-Session\", \"operation\" : \":=\", \"value\" : \"3600\" }", 
        tempstr);
  }

  snprintf(tempstr, 1024, newval); 
  if (tempstr[(strlen(tempstr)-1)] == ',') {
    tempstr[(strlen(tempstr)-1)] = ' ';      /* ensure there is no dangling comma at end of JSON string*/
  }
  snprintf(newval, 1024, "%s]", tempstr);

  if (new_username) {
    if (check_username_memcachedb (new_username) != 0) {
      printf("{ error : \"Cannot change username - a user with this name already exists\" }");
      return;
    }
    radius_delete_memcachedb (user_no_prefix);
    char tempp[50];
    snprintf(tempp, 50, "%s%s", "usr_", new_username);
    username = tempp;
  }

  ret = memcached_set(memcached, username, strlen(username), newval, strlen(newval), 0, 0);

  if (ret != MEMCACHED_SUCCESS) {
    printf("{ error : \"Failed to change user attribute\" }");
    return;
  }
  memcached_server_list_free(servers);
  memcached_free(memcached);
  printf("\t{ count : 1 }\n");
}


