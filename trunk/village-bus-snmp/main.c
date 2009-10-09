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
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF * THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "village-bus-snmp.h"


/**
 * village-bus-snmp provides a json interface to SNMP
 */
int main(int argc, char** argv)
{
  char *request, *command, *address, *community;
  struct json_object* request_object;
  struct json_object* oids;
  struct snmp_session session, *sessionp;
  int i;

  printf("Content-type: application/json\n\n");
  /*printf("Content-type: text/plain\n\n");*/

  /* parse request */
  request = json_cgi_request();
  if (request == NULL || strcasecmp(request, "") == 0) {  /* TODO - return NULL on json_cgi_request always */
    log_message("NULL request\n");
    printf("[\n\t{ error: \"NULL request\" }\n]\n");
    json_cgi_release();
    return EXIT_FAILURE;
  }
  request_object = json_tokener_parse(request);
  if (request_object == NULL) {
    log_message("Could not parse request: %s\n", request);
    printf("[\n\t{ error: \"Could not parse request: %s\" }\n]\n", request);
    json_cgi_release();
    return EXIT_FAILURE;
  }
  command   = json_object_get_string(json_object_object_get(request_object, "command"));
  address   = json_object_get_string(json_object_object_get(request_object, "address"));
  community = json_object_get_string(json_object_object_get(request_object, "community"));
  oids = json_object_object_get(request_object, "oids");
  log_message("Querying: %s with community: %s\n", address, community);

  /* initialize snmp */
  init_snmp("village-bus-snmp");
  snmp_enable_syslog(); 
  snmp_sess_init(&session);            
  session.peername = (char*)address;
  session.version = SNMP_VERSION_1;
  session.community = (unsigned char*)community;
  session.community_len = strlen(community);
  sessionp = snmp_open(&session); 
  if (!sessionp) {
    /*snmp_perror("ack");
      snmp_log(LOG_ERR, "something horrible happened!!!\n");*/
    log_message("Failed to initialize net-snmp\n");

    int liberr, syserr;
    char* errstr;
    snmp_error(&session, &liberr, &syserr, &errstr);

    printf("[ { \n\t\"error\" : \"%s\" \n } ]\n", errstr);
    return;
  } 
  SOCK_STARTUP;

  /* perform snmp query and return results */
  printf("[ {");
  if (strncasecmp("get", command, 3) == 0) {
    snmpget(sessionp, oids);  
  } else if (strncasecmp("walk", command, 4) == 0) {
    for (i = 0; i < json_object_array_length(oids); i++) {
      printf("%s", (i ? "," : ""));
      snmpwalk(sessionp, json_object_get_string(json_object_array_get_idx(oids, i)));  
    }
  } else {
    printf("{ error: \"unknown command\" }");
  }
  printf("\n} ]\n");

  /* clean up and release resources */
  snmp_close(sessionp);
  SOCK_CLEANUP;
  json_object_put(request_object);
  json_cgi_release();

  return EXIT_SUCCESS;
}


