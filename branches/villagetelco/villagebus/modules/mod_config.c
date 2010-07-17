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


#include <uci.h> 
#include <common/jsonrpc.h>

#include "mod_config.h"


/* - config ----------------------------------------------------------------- */
vtable* config_vt = 0;
object* _Config = 0;
config* Config = 0;
object* s_config = 0;


void config_init() 
{
  config_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(config_vt, s_addMethod, s_print, config_print);
  send(config_vt, s_addMethod, s_villagebus_evaluate, config_evaluate);
  _Config = send(config_vt, s_allocate, 0);

  // global module instance vars
  Config = (config*)send(_Config->_vt[-1], s_allocate, sizeof(config));
  Config->context = NULL;
  Config->delimiter = (string*)send(String, s_new, L".", 1); // delimiter used for composing uci key names

  // register module with VillageBus
  s_config = symbol_intern(0, 0, L"config");
  fexp* module = (fexp*)send(Fexp, s_new, s_config, Config);
  ((villagebus*)VillageBus)->modules = (fexp*)send(((villagebus*)VillageBus)->modules, s_fexp_cons, module);
}


const fexp* config_evaluate(closure* c, config* self, const fexp* message)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // lazily initialize uci context
  self->context = uci_alloc_context();
  if (self->context == NULL) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"uci: out of memory"); 
  }

  // evaluate request 
  const Request* request = ((villagebus*)VillageBus)->request;
  switch (request->method) {
  case PUT:
    message = config_put(c, self, message, request->data);
    break;
  case GET:
    message = config_get(c, self, message);
    break;
  default:
    message = (fexp*)send(VillageBus, 
                          s_villagebus_error, 
                          L"mod_config has no registered handler for request method: %d", 
                          request->method);  // TODO method_to_string 
  }

  // clean up UCI 
  if (self->context) {
    uci_free_context(self->context);
  }

  return message;
}


const fexp* config_put(closure* c, config* self, const fexp* message, const unsigned char* payload)
{
  struct json_object* items = json_tokener_parse((unsigned char*)payload); 
  if (items == NULL) {
    return (fexp*)send(VillageBus, 
                       s_villagebus_error, 
                       L"PUT /config could not parse: %s",
                       payload);
  }

  message = fexp_nil;
  int i;
  for (i = 0; i < json_object_array_length(items); i++) {
    struct json_object* item = json_object_array_get_idx(items, i);
    struct json_object* config  = json_object_object_get(item, "config");
    struct json_object* section = json_object_object_get(item, "section");
    struct json_object* option  = json_object_object_get(item, "option");
    struct json_object* value   = json_object_object_get(item, "value");
    if (!json_typecheck(item,    json_type_object)  ||
        !json_typecheck(config,  json_type_string)  ||
        !json_typecheck(section, json_type_string)  ||
        !json_typecheck(option,  json_type_string)  ||
        !json_typecheck(value,   json_type_string)) {
      json_object_put(items);
      return (fexp*)send(VillageBus, 
                         s_villagebus_error, 
                         L"PUT /config expected [{config:String, section:String, option:String, value:String}, {..}] got %s",
                         payload);
    }
    if (!uci_set_config(self->context, 
                        json_object_get_string(config), 
                        json_object_get_string(section), 
                        json_object_get_string(option), 
                        json_object_get_string(value))) {
      json_object_put(items);
      return (fexp*)send(VillageBus, 
                         s_villagebus_error,
                         L"PUT /config could not set: %s.%s.%s=%s", 
                         json_object_get_string(config), 
                         json_object_get_string(section), 
                         json_object_get_string(option), 
                         json_object_get_string(value)); 
    } 
    object* result = send(String, 
                          s_string_fromwchar, 
                          L"{ \"%s\" : \"%s\" }", 
                          json_object_get_string(option), 
                          json_object_get_string(value));
    message = (fexp*)send(message, s_fexp_cons, result);
  }
  
  // clean up
  json_object_put(items);

  // tag reply as JSON
  message = (fexp*)send(message, s_fexp_cons, s_villagebus_json);  

  return message;
}


const fexp* config_get(closure* c, config* self, const fexp* message)
{
  string* s     = (string*)send(message, s_fexp_join, self->delimiter);  // generate a key from message
  char*   query = (char*)send(s, s_string_tochar); // TODO - uci not support UNICODE so much

  // TODO - This may be memory efficient, but it's not particularly pretty.
  //        Being profoundly and fundamentally document-orientated 
  //        HTTP error handling does not support streamed data so much. 
  //        Which kinda sucks totally.
  whttpd_out(L"%S(", VillageBus->request->callback);  
  int n = uci_show(self->context, (strchr(query, '*') ? NULL : query)); // TODO - some decent error handling inside there...
  whttpd_out(L")\n");
  free(query);
  
  return fexp_nil;
}


config* config_print(closure* c, config* self)
{
  wprintf(L"#<CONFIG.%p>", self);
  return self;
}


/* - uci utilities ------------------------------------------------------ */
int uci_show(struct uci_context* context, const char* package)
{
  wprintf(L"["); 
  wprintf(L"\t{");

  int num_sections;
  if (package && strcasecmp(package, "") != 0) {
    num_sections = uci_show_package(context, package);
  } else {      
    char** packages = NULL;
    if ((uci_list_configs(context, &packages) != UCI_OK) || !packages) {
      // TODO - error handling
      uci_perror(context, "village-bus-uci"); 
      return EXIT_FAILURE;
    }
    bool first_package = true;      
    char** p;      
    for (p = packages; *p; p++) {
      wprintf(L"%s\n", (first_package ? "" : ",")); 
      num_sections = uci_show_package(context, *p);
      if (first_package && (num_sections == 0)) { 
        first_package = true;
      } else {
        first_package = false;
      }
    }
  }
  wprintf(L"\t}");
  wprintf(L" ]");

  return num_sections;
}



/**
 *
 */
static char *typestr = NULL;
static const char *cur_section_ref = NULL;
struct uci_type_list {
  unsigned int idx;
  const char *name;
  struct uci_type_list *next;
};
static struct uci_type_list* type_list = NULL;

int uci_show_package(struct uci_context* context, const char* package)
{
  int num_sections = 0;
  struct uci_ptr ptr;
  if (uci_lookup_ptr(context, &ptr, (char*)package, true) != UCI_OK) {
    printl("uci_show: could not lookup package\n");
    uci_perror(context, "village-bus-uci");
  }

  /* (PACKAGE) -> SECTION -> OPTION -> VALUE */
  bool first_section = true;
  struct uci_element* element_section;
  uci_reset_typelist();
  uci_foreach_element(&ptr.p->sections, element_section) { /* for each section */
    num_sections++;
    struct uci_section* section;
    section = uci_to_section(element_section);
    //cur_section_ref = uci_lookup_section_ref(section);  // TODO - better handling for lists
    if (first_section) {
      wprintf(L"  %s\t: { ", section->package->e.name);
      first_section = false;
    } else {
      wprintf(L",\n\t\t    ");
    }
    wprintf(L"%s\t: { _sectiontype\t: \"%s\"", (cur_section_ref ? cur_section_ref : section->e.name), section->type);
    struct uci_element* element_option;
    uci_foreach_element(&section->options, element_option) { /* for each option */
      struct uci_option* option;
      option = uci_to_option(element_option);
      wprintf(L",\n");
      wprintf(L"\t\t\t\t    %s\t: ", option->e.name);
      wprintf(L"\"%s\"", option_to_string(option));
    } /* end for each option */
    wprintf(L" }"); /* end options */
  } /* end for each section */
  uci_reset_typelist();
  if (num_sections > 0) wprintf(L"\n\t\t  }"); /* end sections */

  uci_unload(context, ptr.p);

  return num_sections;
}


/**
 *
 */
bool uci_set_config(struct uci_context* context, const char* config, const char* section, const char* option, const char* value)
{
  char* msg;
  char buf[1024];
  sprintf(buf, "%s.%s.%s=%s", config, section, option, value);

  struct uci_ptr ptr;
  if (uci_lookup_ptr(context, &ptr, buf, true) != UCI_OK) {
    uci_get_errorstr(context, &msg, "uci_set_config");
    printl("Could not lookup config %s - %s\n", buf, msg);
    free(msg);
    return false;
  }

  if (uci_set(context, &ptr) != UCI_OK) {
    uci_get_errorstr(context, &msg, "uci_set_config");
    printl("Could not set config %s - %s\n", buf, msg);
    free(msg);
    return false;
  }

  if (uci_commit(context, &ptr.p, false) != UCI_OK) {
    uci_get_errorstr(context, &msg, "uci_set_config");
    printl("Could not commit config %s - %s\n", buf, msg);
    free(msg);
    return false;
  }

  //log_message("{ message: \"set: %s.%s.%s=%s\" }\n", config, section, option, value, ptr);
  return true;
}



/**
 * Convert an uci_option to a string
 *
 * Manages it's own memory
 */
char* option_to_string(struct uci_option* option) 
{
  static char* ret = NULL;
  if (!ret) {
    ret = malloc(1024);
  }
  switch (option->type) {
  case UCI_TYPE_STRING:
    sprintf(ret, "%s", option->v.string);
    break;
  case UCI_TYPE_LIST:
    sprintf(ret, "<list>");
    struct uci_element* element_value;
    uci_foreach_element(&option->v.list, element_value) {
      //sprintf(ret, "%s%s", (sep ? "<sep>" : ""), element_value->name);
      //sep = true;
    }
    break;
  default:
    sprintf(ret, "<unknown>");
  }
  return ret;
}


/**
 *
 */
static void uci_reset_typelist(void)
{
  struct uci_type_list *type;
  while (type_list != NULL) {
    type = type_list;
    type_list = type_list->next;
    free(type);
  }
  if (typestr) {
    free(typestr);
    typestr = NULL;
  }
  cur_section_ref = NULL;
}
