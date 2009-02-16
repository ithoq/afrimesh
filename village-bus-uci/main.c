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


#include "village-bus-uci.h"

int main(int argc, char** argv)
{
  //printf("Content-type: application/json\n\n");
  printf("Content-type: text/plain\n\n");

  /* parse request */
  char* request = json_cgi_request(); 
  if (request == NULL || strcasecmp(request, "") == 0) { // TODO - add support for set as well
    request = malloc(512);
    if (argc == 3) {
      sprintf(request, 
              "{ \"command\" : \"%s\", \"config\" : \"%s\"  }",
              argv[1], argv[2]);
    } else {
      sprintf(request, "{ \"command\" : \"show\", \"config\" : \"\" }");
    }
  }

  struct json_object* request_object = json_tokener_parse(request);
  if (request_object == NULL) {
    log_message("Could not parse request: %s\n", request);
    printf("[\n\t{ error: \"Could not parse request: %s\" }\n]\n", request);
    json_cgi_release();
    return EXIT_FAILURE;
  }

  /* Initialize UCI */
  UCI_CONTEXT = uci_alloc_context();
  if (UCI_CONTEXT == NULL) {
    log_message("uci: Out of memory\n");
    return EXIT_FAILURE;
  }

  /* execute uci command and output json reply */
  char* command  = json_object_get_string(json_object_object_get(request_object, "command"));
  printf("[\n"); 
  if (strncasecmp("show", command, 4) == 0) {
    printf("\t{\n");

    char* config = json_object_get_string(json_object_object_get(request_object, "config"));
    if (config && strcasecmp(config, "") != 0) {
      int num_sections = uci_show_package(config);
    } else {
      char** configs = NULL;
      char** p;
      if ((uci_list_configs(UCI_CONTEXT, &configs) != UCI_OK) || !configs) {
        uci_perror(UCI_CONTEXT, "village-bus-uci");
        return EXIT_FAILURE;
      }
      bool first_package = true;
      for (p = configs; *p; p++) {
        printf("%s\n", (first_package ? "" : ",")); 
        int num_sections = uci_show_package(*p);
        if (first_package && (num_sections == 0)) { 
          first_package = true;
        } else {
          first_package = false;
        }
        //log_message("Queried package: %s -> %d\n", *p, num_sections);
      }
    }
    printf("\n\t}\n");

  } else if (strncasecmp("set", command, 4) == 0) {
    int i;
    struct json_object* arguments_object = json_object_object_get(request_object, "arguments");
    for (i = 0; i < json_object_array_length(arguments_object); i++) {
      struct json_object* argument_object = json_object_array_get_idx(arguments_object, i);
      char* config  = json_object_get_string(json_object_object_get(argument_object, "config"));
      char* section = json_object_get_string(json_object_object_get(argument_object, "section"));
      char* option  = json_object_get_string(json_object_object_get(argument_object, "option"));
      char* value   = json_object_get_string(json_object_object_get(argument_object, "value"));
      if (!uci_set_config(config, section, option, value)) {
        printf("%s{ error : \"could not set: %s.%s.%s=%s\" }", (i ? "," : ""), config, section, option, value);
        break;
      } else {
        printf("%s{ %s : %s }", (i ? "," : ""), option, value);
      }
    }
    struct uci_element *e;

  } else {
    printf("{ error: \"unknown command\" }");
  }

  printf("]\n");


  /* clean up UCI */
  if (UCI_CONTEXT) {
    uci_free_context(UCI_CONTEXT);
  }

  /* release resources */
  json_object_put(request_object);
  json_cgi_release();

  return EXIT_SUCCESS;
}


/*

[ { 

  "network" : { "eth0"      : { "type"     : "switch",        
                                "vlan0"    : "0 1 2 3 5*",        
                                "vlan1"    : "4 5"  },            
                "loopback"  : { "type"     : "interface",     
                                "ifname"   : "lo",              
                                "proto"    : "static",          
                                "ipaddr"   : "127.0.0.1",       
                                "netmask"  : "255.0.0.0" }      
              },                                                      
  "system"  : { "cfg02f02f" : { "type"     : "system",        
                                "hostname" : "OpenWrt",           
                                "timezone" : "UTC" }              
              }                                                       
}] 

*/

