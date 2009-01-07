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


#include <stdio.h>

#include <json/json.h>
#include <uci.h> 

#include <json_cgi.h>


static struct uci_context* UCI_CONTEXT;



bool uci_set_config(const char* config, const char* section, const char* option, const char* value)
{

  char buf[1024];
  sprintf(buf, "%s.%s.%s=%s", config, section, option, value);

  struct uci_ptr ptr;
  if (uci_lookup_ptr(UCI_CONTEXT, &ptr, buf, true) != UCI_OK) {
    log_message("uci_set_config: could not lookup config %s\n", buf);
    uci_perror(UCI_CONTEXT, "village-bus-uci");
    return false;
  }

  if (uci_set(UCI_CONTEXT, &ptr) != UCI_OK) {
    log_message("uci_set_config: could not set config %s\n", buf);
    uci_perror(UCI_CONTEXT, "village-bus-uci");
    return false;
  }

  if (uci_commit(UCI_CONTEXT, &ptr.p, false) != UCI_OK) {
    log_message("uci_set_config: could not commit config %s\n", buf);
    uci_perror(UCI_CONTEXT, "village-bus-uci");
    return false;
  }

  log_message("{ message: \"set: %s.%s.%s=%s\" }\n", config, section, option, value, ptr);
  return true;
}




bool uci_show_package(const char* package)
{
  int num_sections = 0;
  struct uci_ptr ptr;
  if (uci_lookup_ptr(UCI_CONTEXT, &ptr, package, true) != UCI_OK) {
    log_message("uci_show: could not lookup package\n");
    uci_perror(UCI_CONTEXT, "village-bus-uci");
  }

  /* check if package has any sections - actually, we can just count them, much simpler :) */
  /*struct uci_element* element_section;
  element_section = list_to_element((&ptr.p->sections)->next);
  if (&element_section->list == &ptr.p->sections) {
    log_message("No children\n");
  }*/


  /* (PACKAGE) -> SECTION -> OPTION -> VALUE */
  bool first_section = true;
  struct uci_element* element_section;
  uci_foreach_element(&ptr.p->sections, element_section) { /* for each section */
    num_sections++;
    struct uci_section* section;
    section = uci_to_section(element_section);
    if (first_section) {
      printf("  %s\t: { ", section->package->e.name);
      first_section = false;
    } else {
      printf(",\n\t\t    ");
    }
    printf("%s\t: { type\t: \"%s\"", section->e.name, section->type);
    struct uci_element* element_option;
    uci_foreach_element(&section->options, element_option) { /* for each option */
      struct uci_option* option;
      option = uci_to_option(element_option);
      printf(",\n");
      printf("\t\t\t\t    %s\t: ", option->e.name);
      struct uci_element* element_value;
      bool sep = false;
      switch (option->type) {
      case UCI_TYPE_STRING:
        printf("\"%s\"", option->v.string);
        break;
      case UCI_TYPE_LIST:
        printf("<list>");
        uci_foreach_element(&option->v.list, element_value) {
          //printf("%s%s", (sep ? "<sep>" : ""), element_value->name);
          sep = true;
        }
        break;
      default:
        printf("<unknown>");
      }
    }
    printf(" }"); /* end options */
  }
  if (num_sections > 0) printf("\n\t\t  }"); /* end sections */

  uci_unload(UCI_CONTEXT, ptr.p);

  return num_sections;
}



int main(int argc, char** argv)
{
  //printf("Content-type: application/json\n\n");
  printf("Content-type: text/plain\n\n");

  /* parse request */
  char* request = json_cgi_request(); 
  if (request == NULL || strcasecmp(request, "") == 0) { // TODO a default for everything or some command line parsing instead ?
    request = "{ \"command\" : \"show\" }";
  }
  if (request == NULL || strcasecmp(request, "") == 0) {  /* TODO - return NULL 
                                                             on json_cgi_request always */
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
  char* command = json_object_get_string(json_object_object_get(request_object, "command"));

  /* Initialize UCI */
  if (!UCI_CONTEXT) {
    UCI_CONTEXT = uci_alloc_context();
    if (UCI_CONTEXT == NULL) {
      log_message("uci: Out of memory\n");
      return EXIT_FAILURE;
    }
  }

  /* execute uci command and output json reply */
  printf("[\n"); 
  if (strncasecmp("show", command, 4) == 0) {
    printf("\t{\n");
    /* TODO - only query command args */
    /* Get uci package list */
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
