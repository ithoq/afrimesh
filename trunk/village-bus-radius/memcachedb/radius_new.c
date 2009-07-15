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

#include "village-bus-radius.h"
#include <libmemcached/memcached.h>

int generate_username_memcachedb (char** ret, size_t n, const char* base)
{
  size_t key_length = 9;
  uint32_t offset = 1;
  uint64_t value;
  char *countkey = "cnt_prepaidID";
  memcached_st *memcache = memcached_create(NULL);
  memcached_server_st *servers = memcached_servers_parse(radius_mysql_server);
  memcached_return rc = memcached_server_push(memcache, servers);
  memcached_increment(memcache, countkey, key_length, offset, &value);

  if (value == 0) {
    memcached_set(memcache, countkey, strlen(countkey), "0", 1, (time_t)0, (uint32_t)0); /* create key if it doesn't exist */
  }

  if(value == 0) {
    sprintf(ret, base);
    return 0;
  }
  char count [30];
  snprintf(ret, n, "%s%s", base, count);

  return 0;
}

int check_username_memcachedb (const char* username)
{
  size_t value_length;
  uint32_t flags;
  memcached_st *memcache = memcached_create(NULL);
  memcached_server_st *servers = memcached_servers_parse(radius_mysql_server);
  memcached_return rc = memcached_server_push(memcache, servers);
  if (MEMCACHED_SUCCESS != rc){
    return -1;
  }
  memcached_get(memcache, username, strlen(username), &value_length, &flags, &rc);
  if (rc == MEMCACHED_NOTFOUND) {
    return 0;
  }
  return -1;
}

void radius_new_memcachedb(const char* username, const char* type, int seconds)
{
  char user_no_prefix [50];
  snprintf (user_no_prefix, 50, username);
  char temp[50];
  snprintf(temp, 50, "%s%s", "usr_", username);
  username = temp;

  /* if type is prepaid - generate a username */
  if (strcmp(type, "prepaid") == 0) { 
    char buf[256];
    if (generate_username_memcachedb (buf, sizeof(buf), username) != 0) {
      return;
    }
    username = buf;
  }

  /* check that username is unique */
  if (check_username_memcachedb (username) != 0) {
    printf("\t{\n\t\terror : \"The username '%s' already exists\"\n\t}\n", user_no_prefix);
    return;
  }
 
  /* generate a password */
  char password[8];
  generate_password(password, 8);

  /* create a JSON string for the user attributes */
  char *user_attributes = malloc (1024);
  char *user_attributes1  = "[ { \"attribute\" : \"Username\", \"operation\" : \":=\", \"value\" : \"";
  char *user_attributes2 = "\" }, { \"attribute\" : \"ClearText-Password\", \"operation\" : \":=\", \"value\" : \"";
  char *user_attributes3 = malloc(64);
  if (strcmp(type, "prepaid") == 0) {
    user_attributes3 = "\" }, { \"attribute\" : \"Max-Prepaid-Session\", \"operation\" : \":=\", \"value\" : \"";
  }  else {
    user_attributes3 = " ";
  }
  char *user_attributes4 = "\" }]";
  
  snprintf(user_attributes, 1024, "%s%s%s%s%s%d%s", user_attributes1, username, user_attributes2, 
              password, user_attributes3, seconds, user_attributes4);

  memcached_st *memcache = memcached_create(NULL);
  memcached_server_st *servers = memcached_servers_parse(radius_mysql_server);
  memcached_return rc = memcached_server_push(memcache, servers);
  rc = memcached_set(memcache, username, strlen(username), user_attributes, strlen(user_attributes), 0, 0);

  if (rc != MEMCACHED_SUCCESS) {
    printf("\t{\n\t\terror : \"Could not update database\"\n\t}\n");
  }

  /* release libmemcachedb */
  if (memcache) {
    free (memcache); 
  }
    
  /* output result */
  printf("\t{\n");
  printf("\t\tusername : \"%s\",\n", user_no_prefix);
  printf("\t\tpassword : \"%s\"\n", password);
  printf("\t}\n");
}
