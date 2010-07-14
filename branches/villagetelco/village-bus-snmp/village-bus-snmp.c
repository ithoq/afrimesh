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


#include "village-bus-snmp.h"


/**
 * Initialize snmp session
 */
struct snmp_session* snmp_start(const char* address, const char* community)
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
void snmp_stop(struct snmp_session* session)
{
  snmp_close(session);
  SOCK_CLEANUP;
}



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
 * Print a SNMP variable to stdout
 */
void print_snmp_variable(struct variable_list* variable) 
{
  if (variable->type == ASN_OCTET_STR) {
    /*char* buf = (char*)malloc(1 + variable->val_len);  // TODO - check on all platforms
    memcpy(buf, variable->val.string, variable->val_len);
    buf[variable->val_len] = '\0';*/
    /*printf("\"%s\"", variable->val.string);*/
    /*free(buf);*/
    char buf[variable->val_len + 1];
    snprintf(buf, variable->val_len, "%s", variable->val.string);
    buf[variable->val_len] = '\0';
    printf("\"%s\"", buf);
  } else if (variable->type == ASN_COUNTER) { /* ASN_COUNTER */
    printf("%lu", *variable->val.integer);
  } else if (variable->type == ASN_NULL) {    /* ASN_NULL */
    printf("null");
  } else {
    printf("\"unknown type: 0x%x\"", variable->type);
  }
}


/* TODO - there's an annoying bug when performing a snmp query in snmp_get that expects
          an ASN_OCTET_STR back but actually gets an ASN_NULL :-P */
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


/**
 * Perform a snmpget on an array of oids
 */
void snmpget(struct snmp_session* session, struct json_object* oids)
{
  struct snmp_pdu *pdu, *response;
  struct variable_list *variable;
  int status, count;
  char buf[1024];
  oid name[MAX_OID_LEN];
  count = json_object_array_length(oids);
  size_t name_length[count];

  /* build query */
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  for (count = 0; count < json_object_array_length(oids); count++) {
    name_length[count] = MAX_OID_LEN;
    read_objid(json_object_get_string(json_object_array_get_idx(oids, count)), 
               name, 
               &(name_length[count]));   // TODO - check use get_node ?
    snmp_add_null_var(pdu, name, name_length[count]);
  }

  /* perform query */
  status = snmp_synch_response(session, pdu, &response); // TODO - better error logging
  if (status != STAT_SUCCESS)  {
    if (response && response->errstat != SNMP_ERR_NOERROR) {
      log_message("Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
      snmp_free_pdu(response);
    }
    int liberr, syserr;
    char* errstr;
    snmp_error(session, &liberr, &syserr, &errstr);
    printf(" \"error\" : \"%s\"", errstr);
    return;
  }

  /* print query results */
  count = 0;
  for (variable = response->variables; variable; variable = variable->next_variable) {
    snprint_objid(buf, 1024, variable->name, variable->name_length);
    printf("%s\n\t\"%s\" : ", (variable == response->variables ? "" : ","), escape_oid(buf));
    print_snmp_variable(variable);
    printf(",\n\t%d : ", count);
    print_snmp_variable(variable);
    count++;
  }
  
  if (response) {
    snmp_free_pdu(response);
  }
}


/** 
 * Perform a snmpget on an array of oids
 */
struct json_object* snmp_get(struct snmp_session* session, struct json_object* oids)
{
  struct snmp_pdu *pdu, *response;
  struct variable_list *variable;
  struct json_object* result;
  int status, count;
  char buf[1024];
  oid name[MAX_OID_LEN];
  count = json_object_array_length(oids);
  //size_t name_length[count];
  size_t name_len;

  /* initialize our result object */
  result = json_object_new_object();

  /* build query */
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  for (count = 0; count < json_object_array_length(oids); count++) {
    //name_length[count] = MAX_OID_LEN;
    name_len = MAX_OID_LEN;
    read_objid(json_object_get_string(json_object_array_get_idx(oids, count)), 
               name, 
               &name_len);
               //&(name_length[count]));   
    //snmp_add_null_var(pdu, name, name_length[count]);
    //log_message("FOOK: %s - %s (%d)\n", json_object_get_string(json_object_array_get_idx(oids, count)), name, name_len);
    snmp_add_null_var(pdu, name, name_len);
  }

  /* perform query */
  status = snmp_synch_response(session, pdu, &response); // TODO - better error logging
  if (status != STAT_SUCCESS)  {
    if (response && response->errstat != SNMP_ERR_NOERROR) {
      log_message("Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
      snmp_free_pdu(response);
    }
    return NULL;
  }

  /* print query results */
  count = 0;
  for (variable = response->variables; variable; variable = variable->next_variable) {
    /*snprint_objid(buf, 1024, variable->name, variable->name_length);
    printf("%s\n\t\"%s\" : ", (variable == response->variables ? "" : ","), escape_oid(buf));
    print_snmp_variable(variable);
    printf(",\n\t%d : ", count);
    print_snmp_variable(variable);*/
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
void snmpwalk(struct snmp_session* session, const char* name_oid)
{
  struct snmp_pdu *pdu, *response;
  struct variable_list* variable;
  int status, running, count;
  char buf[1024];
  oid name[MAX_OID_LEN], root[MAX_OID_LEN];
  size_t name_length, root_length;
  
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
        log_message("Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
        snmp_free_pdu(response);
      }
      int liberr, syserr;
      char* errstr;
      snmp_error(session, &liberr, &syserr, &errstr);
      printf(" \"error\" : \"%s\" } ]", errstr);
      exit(1);
    }

    /* check if we've reached the end of the trail */
    for (variable = response->variables; variable; variable = variable->next_variable) {
      if ((variable->name_length < root_length) || 
          (memcmp(root, variable->name, root_length * sizeof(oid)) != 0)) { /* fin */
        running = 0; 
        continue;
      }
        
      /* print current node */
      snprint_objid(buf, 1024, variable->name, variable->name_length);
      printf("%s\n\t\"%s\" : ", (count ? "," : ""), escape_oid(buf));
      print_snmp_variable(variable);
      count++;

      if ((variable->type != SNMP_ENDOFMIBVIEW) && 
          (variable->type != SNMP_NOSUCHOBJECT) && 
          (variable->type != SNMP_NOSUCHINSTANCE)) {
        if (snmp_oid_compare(name, name_length, variable->name, variable->name_length) >= 0) {
          log_message("Error: OID not increasing: "); 
          snprint_objid(buf, 1024, name, name_length);
          log_message("%s >= ", buf);
          snprint_objid(buf, 1024, variable->name, variable->name_length);
          log_message("%s\n", buf);
          running = 0;
        }
        memmove((char*)name, (char*)variable->name, variable->name_length * sizeof(oid));
        name_length = variable->name_length;
      } else {
        log_message("Unknown exception\n");
        running = 0;
      }
    }

    if (response) {
      snmp_free_pdu(response);
    }
  } /* while running */
}


/**
 * Perform a snmpwalk on each element in an array of oids
 */
struct json_object* snmp_walk(struct snmp_session* session, const char* name_oid)
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
        log_message("Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
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
          log_message("Error: OID not increasing: "); 
          snprint_objid(buf, 1024, name, name_length);
          log_message("%s >= ", buf);
          snprint_objid(buf, 1024, variable->name, variable->name_length);
          log_message("%s\n", buf);
          running = 0;
        }
        memmove((char*)name, (char*)variable->name, variable->name_length * sizeof(oid));
        name_length = variable->name_length;
      } else {
        log_message("Unknown exception\n");
        running = 0;
      }
    }

    if (response) {
      snmp_free_pdu(response);
    }
  } /* while running */

  return result;
}

 
