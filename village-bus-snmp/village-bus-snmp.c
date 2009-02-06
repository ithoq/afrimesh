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

#include <strings.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <json/json.h>
#include <json_cgi.h>


char* escape_oid(char* name) 
{
  size_t t;
  for (t = 0; t < 1024; t++) {
    if (name[t] == 0) {
      break;
    } else if (name[t] == '.') {
      name[t] = '_';
    } else if (name[t] == ':') {
      name[t] = '_';
    } else if (name[t] == '-') {
      name[t] = '_';
    }
  }
  return name;
}


void print_var(struct variable_list* vars) 
{
  if (vars->type == ASN_OCTET_STR) {
    char* sp = (char*)malloc(1 + vars->val_len);
    memcpy(sp, vars->val.string, vars->val_len);
    sp[vars->val_len] = '\0';
    printf("\"%s\"", sp);
    free(sp);
  } else if (vars->type == ASN_COUNTER) { /* ASN_COUNTER */
    printf("%lu", *vars->val.integer);
  } else {
    printf("\"unknown type: 0x%x\"", vars->type);
  }
}

void query_snmp(const char* host, const char* community)
{
  struct snmp_session session, *ss;
  struct snmp_pdu* pdu;
  struct snmp_pdu* response;
  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;
  oid OID_ifInOctets[MAX_OID_LEN];
  size_t OID_ifInOctets_len = MAX_OID_LEN;
  oid OID_ifOutOctets[MAX_OID_LEN];
  size_t OID_ifOutOctets_len = MAX_OID_LEN;
  struct variable_list* vars;
  int status;
  
  init_snmp("snmpapp");
  snmp_sess_init(&session);            
  session.peername = (char*)host;
  session.version = SNMP_VERSION_1;
  session.community = (unsigned char*)community;
  session.community_len = strlen(community);

  ss = snmp_open(&session);
  if (!ss) {
    snmp_perror("ack");
    snmp_log(LOG_ERR, "something horrible happened!!!\n");
    log_message("Could not initialize net-snmp\n");
    printf("\t{ error: \"Could not initialize net-snmp\" }\n");  /* TODO - better error reporting please */
    return;
  }  

  pdu = snmp_pdu_create(SNMP_MSG_GET);
  /* system.sysDescr.0 */
  read_objid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len);
  //read_objid("system.sysDescr.0", anOID, &anOID_len);
  //get_node("sysDescr.0", anOID, &anOID_len);
  snmp_add_null_var(pdu, anOID, anOID_len);

  /* interfaces.ifTable.ifEntry.ifInOctets.4  ->  eth0.1 == .4 */
  read_objid(".1.3.6.1.2.1.2.2.1.10.4", OID_ifInOctets, &OID_ifInOctets_len); 
  snmp_add_null_var(pdu, OID_ifInOctets, OID_ifInOctets_len);
  /* interfaces.ifTable.ifEntry.ifOutOctets.4  ->  eth0.1 == .4 */
  read_objid(".1.3.6.1.2.1.2.2.1.16.4", OID_ifOutOctets, &OID_ifOutOctets_len); 
  snmp_add_null_var(pdu, OID_ifOutOctets, OID_ifOutOctets_len);

  /* perform query */
  status = snmp_synch_response(ss, pdu, &response);
  if (response->errstat != SNMP_ERR_NOERROR) {
    log_message("There was a problem querying snmp\n");
    if (status == STAT_SUCCESS)  {
      log_message("1\n");
      log_message("Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
      log_message("2\n");
    } else {
      log_message("3\n");
      snmp_sess_perror("snmpget", ss);
      log_message("4\n");
    }
    if (response)
      snmp_free_pdu(response);
    snmp_close(ss);
    return;
  } 

  size_t index = 0;
  for (vars = response->variables; vars; vars = vars->next_variable) {
    //print_variable(vars->name, vars->name_length, vars);
    char buf[1024];
    snprint_objid(buf, 1024, vars->name, vars->name_length);
    char* name = escape_oid(buf);
    printf("%s\n\t%s : ", (vars == response->variables ? "" : ","), name);
    print_var(vars);
    printf(",\n\t%d : ", index);
    print_var(vars);
    index++;
  }
  
  if (response) {
    snmp_free_pdu(response);
  }
  snmp_close(ss);
}



int main(int argc, char** argv)
{
  //stderr = stdout;
  //printf("Content-type: application/json\n\n");
  printf("Content-type: text/plain\n\n");

  /* parse request */
  char* request = json_cgi_request();
  if (request == NULL || strcasecmp(request, "") == 0) {  /* TODO - return NULL on json_cgi_request always */
    log_message("NULL request\n");
    printf("[\n\t{ error: \"NULL request\" }\n]\n");
    json_cgi_release();
    return EXIT_FAILURE;
  }
  
  struct json_object* request_object = json_tokener_parse(request);
  if (request_object == NULL) {
    log_message("Could not parse request: %s\n", request);
    printf("[\n\t{ error: \"Could not parse request: %s\" }\n]\n", request);
    json_cgi_release();
    return EXIT_FAILURE;
  }
  char* address   = json_object_get_string(json_object_object_get(request_object, "address"));
  char* community = json_object_get_string(json_object_object_get(request_object, "community"));
  log_message("Querying: %s with community: %s\n", address, community);

  /* perform snmp query and reply -  IF-MIB::ifInOctets.4  ->  IF_MIB__ifInOctets_4 */
  printf("[ {");
  query_snmp(address, community);  /* TODO - pass oid with request !!! */
  /*printf("\tiso_3_6_1_2_1_2_2_1_10_4 : 0,\n");
    printf("\tiso_3_6_1_2_1_2_2_1_16_4 : 0\n");*/
  printf("\n} ]\n");

  /* release resources */
  json_object_put(request_object);
  json_cgi_release();

  return EXIT_SUCCESS;
}
