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

#include <freeradius-devel/ident.h>
#include <freeradius-devel/radiusd.h>
#include <freeradius-devel/modules.h>
#include <stdio.h>
#include <libmemcached/memcached.h>
#include <json/json.h>

static char* db_set(const char* key, const char* value);
static char* db_get(const char* key);
static char* get_json_value (char* jsonstr, char* attribute);

/*
 *	Structure for module config
 */
typedef struct rlm_memcachedb_t {
  const char *name;
	char *servers; 
  int some_flag;
  int auth_type;
} rlm_memcachedb_t;

/*
 *  Helper function to get value from memcached database
 */
static char* db_get(const char* key)
{
  /* initialize libmemcached */
  memcached_st* memcached = memcached_create(NULL);

  /* set server list */
  memcached_server_st* server_list = memcached_servers_parse("localhost");
  memcached_server_push(memcached, server_list);
  memcached_server_list_free(server_list);

  size_t value_length;
  uint32_t flags;
  memcached_return ret;

  char* value = memcached_get(memcached, key, strlen(key), &value_length, &flags, &ret);
  if (ret == MEMCACHED_SUCCESS) {
    //printf("key: %s\nflags: %x\nlength: %zu\nvalue: %s\n", key, flags, (int)value_length, value);
  } else if (ret == MEMCACHED_NOTFOUND) {
    value = NULL;
  } else { 
    value = malloc(512);
    snprintf(value, 512, "\"'%s' - %s\"", 
             key, 
             memcached_strerror(memcached, ret), 
             ((memcached->cached_errno) ? strerror(memcached->cached_errno) : ""));
  }
  /* release libmemcachedb */
  if (memcached) {
    free (memcached); 
  }
  return value;
}

/*
 *  Helper function to set value in memcached database
 */
static char* db_set(const char* key, const char* value)
{
  /* initialize libmemcached */
  memcached_st* memcached = memcached_create(NULL);

  /* set server list */
  memcached_server_st* server_list = memcached_servers_parse("localhost");
  memcached_server_push(memcached, server_list);
  memcached_server_list_free(server_list);

  memcached_return ret;
  char* msg = (char*)value;

  ret = memcached_set(memcached, key, strlen(key), value, strlen(value), 0, 0);

  if (ret != MEMCACHED_SUCCESS) {
    msg = malloc(512);
    snprintf(msg, 512, "\"'%s' - %s\"", 
             key, 
             memcached_strerror(memcached, ret), 
             ((memcached->cached_errno) ? strerror(memcached->cached_errno) : ""));
  }
  /* release libmemcachedb */
  if (memcached) {
    free (memcached); 
  }
  return msg;
}

/*
 *  Helper function to parse a json string and return a specified attribute
 */
static char* get_json_value (char* jsonstr, char* attribute)
{
  char *value;
  int i;
  struct json_object* entries = json_tokener_parse(jsonstr);
  
  if (entries == NULL) {
    return NULL;
  }

  for (i = 0; i < json_object_array_length(entries); i++) {
      struct json_object* thisentry = json_object_array_get_idx(entries, i);
      char* thisattribute = json_object_get_string(json_object_object_get(thisentry, "attribute"));
     
      if (strcmp(thisattribute, attribute) == 0) {
        value = json_object_get_string(json_object_object_get(thisentry, "value"));
        break;
      }
  }
  return value;
}

/*
 *	Map config data to variables
 */
static const CONF_PARSER module_config[] = {
  { "servers",   PW_TYPE_STRING_PTR, offsetof(rlm_memcachedb_t, servers),   NULL, "auto" },
  { "some_flag", PW_TYPE_BOOLEAN,    offsetof(rlm_memcachedb_t, some_flag), NULL, "no"   },
  { NULL, -1, 0, NULL, NULL }
};

/*
 *	Module instantiation
 */
static int memcachedb_instantiate(CONF_SECTION *conf, void **instance)
{
  rlm_memcachedb_t *data;
  DICT_VALUE *dval;

	data = rad_malloc(sizeof(*data));
	if (!data) {
    radlog(L_ERR, "rlm_memcachedb: can't alloc instance");
		return -1;
	}
	memset(data, 0, sizeof(*data));

	if (cf_section_parse(conf, data, module_config) < 0) {
    radlog(L_ERR, "rlm_memcachedb: can't parse config");
		free(data);
		return -1;
	}

  data->name = cf_section_name2(conf);
  if (!data->name) {
    data->name = cf_section_name1(conf);
  }

  dval = dict_valbyname(PW_AUTH_TYPE, data->name);
  if (dval) {
    data->auth_type = dval->value;
  } else {
    data->auth_type = 0;
  }

  *instance = data; 

	return 0;
}

/*
 *  Authenticate the user with the given password (check against memcachedb).
 */
static int memcachedb_authenticate (void *instance, REQUEST *request)
{
  if(!(request->username->vp_strvalue)) {
    RDEBUG ("Rejecting authorization request - username is invalid.");
    return RLM_MODULE_REJECT;
  }

  /* check whether user is in the database */
  char temp[50];
  snprintf(temp, 50, "%s%s", "usr_", (request->username->vp_strvalue));
  char* memcached_user = db_get(temp);

  if (!memcached_user) {
    RDEBUG ("Rejecting authorization request - username does not exist.");
    free(memcached_user);
    return RLM_MODULE_REJECT;
  }
  RDEBUG ("Username \"%s\" matched. Checking password.", request->username->vp_strvalue);

  /* parse password from value in memcachedb */
  char *userpass = get_json_value (memcached_user, "ClearText-Password");

  /* check whether passwords match */
  if (strcmp (userpass, request->password->vp_strvalue) == 0) {
    RDEBUG ("User credentials verified. Authorization request granted.");
    free(memcached_user);
    return RLM_MODULE_OK;  
  }
  RDEBUG ("Rejecting authorization request - passwords do not match.");
  free(memcached_user);

  return RLM_MODULE_REJECT;
}
 
static int memcachedb_authorize (void *instance, REQUEST *request)
{
  char auth_msg [1024];
  char *max_prepaid_session;
  char temp[50];
  snprintf(temp, 50, "%s%s", "usr_", request->username->vp_strvalue);
  char *username = db_get(temp);

  if (username != NULL) {
    char *max_prepaid_session = get_json_value (db_get(temp), "Max-Prepaid-Session");
    if (max_prepaid_session) {
      sprintf (auth_msg, "%s", max_prepaid_session);
      VALUE_PAIR *vp = pairmake("Max-Prepaid-Session", auth_msg, T_OP_SET);
      pairadd(&request->config_items, vp);
    }
  }
  rlm_memcachedb_t *inst = instance;
  VALUE_PAIR *valp = radius_paircreate(request, &request->config_items, PW_AUTH_TYPE, PW_TYPE_INTEGER);
  valp->vp_integer = inst->auth_type;
  
  return RLM_MODULE_OK;
}

/*
 *	Free memory
 */
static int memcachedb_detach(void *instance)
{
  if (instance) free(instance);
	return 0;
}

/*
 *	Module definition
 */
module_t rlm_memcachedb = {
	RLM_MODULE_INIT,
	"MEMCACHEDB",
	RLM_TYPE_CHECK_CONFIG_SAFE | RLM_TYPE_HUP_SAFE | RLM_TYPE_THREAD_SAFE,		/* type */
	memcachedb_instantiate,		/* instantiation */
	memcachedb_detach,			/* detach */
	{
		memcachedb_authenticate,	/* authentication */
		memcachedb_authorize,	/* authorization */
		NULL,	/* preaccounting */
		NULL,	/* accounting */
		NULL,	/* checksimul */
		NULL,			/* pre-proxy */
		NULL,			/* post-proxy */
		NULL			/* post-auth */
	},
};
