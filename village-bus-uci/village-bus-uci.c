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


static char *typestr = NULL;
static const char *cur_section_ref = NULL;
struct uci_type_list {
  unsigned int idx;
  const char *name;
  struct uci_type_list *next;
};
static struct uci_type_list* type_list = NULL;



/**
 *
 */
int _uci_show_package(const char* package, struct json_object* result)
{
  int num_sections = 0;
  struct uci_ptr ptr;
  if (uci_lookup_ptr(UCI_CONTEXT, &ptr, package, true) != UCI_OK) {
    log_message("uci_show: could not lookup package\n");
    //uci_perror(UCI_CONTEXT, "village-bus-uci");
    return 0;
  }

  struct json_object* json_package = json_object_new_object();

  /* (PACKAGE) -> SECTION -> OPTION -> VALUE */
  struct uci_element* element_section;
  uci_reset_typelist();
  uci_foreach_element(&ptr.p->sections, element_section) { /* for each section */
    num_sections++;
    struct uci_section* section;
    section = uci_to_section(element_section);
    //cur_section_ref = uci_lookup_section_ref(section);  // TODO - better handling for lists
    struct uci_element* element_option;
    struct json_object* json_section = json_object_new_object();
    json_object_object_add(json_section, "_sectiontype", json_object_new_string(section->type));
    uci_foreach_element(&section->options, element_option) { /* for each option */
      struct uci_option* option;
      option = uci_to_option(element_option);
      json_object_object_add(json_section, 
                             option->e.name, 
                             json_object_new_string(option_to_string(option)));

    } /* end for each option */
    json_object_object_add(json_package, 
                           (cur_section_ref ? cur_section_ref : section->e.name), 
                           json_section);

  } /* end for each section */
  json_object_object_add(result, package, json_package);

  uci_reset_typelist();
  uci_unload(UCI_CONTEXT, ptr.p);

  return num_sections;
}


/**
 *
 */
bool uci_show_package(const char* package)
{
  int num_sections = 0;
  struct uci_ptr ptr;
  if (uci_lookup_ptr(UCI_CONTEXT, &ptr, package, true) != UCI_OK) {
    log_message("uci_show: could not lookup package\n");
    uci_perror(UCI_CONTEXT, "village-bus-uci");
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
      printf("  %s\t: { ", section->package->e.name);
      first_section = false;
    } else {
      printf(",\n\t\t    ");
    }
    printf("%s\t: { _sectiontype\t: \"%s\"", (cur_section_ref ? cur_section_ref : section->e.name), section->type);
    struct uci_element* element_option;
    uci_foreach_element(&section->options, element_option) { /* for each option */
      struct uci_option* option;
      option = uci_to_option(element_option);
      printf(",\n");
      printf("\t\t\t\t    %s\t: ", option->e.name);
      printf("\"%s\"", option_to_string(option));
    } /* end for each option */
    printf(" }"); /* end options */
  } /* end for each section */
  uci_reset_typelist();
  if (num_sections > 0) printf("\n\t\t  }"); /* end sections */

  uci_unload(UCI_CONTEXT, ptr.p);

  return num_sections;
}



/**
 * Set a uci configuration value
 */
bool uci_set_config(const char* config, const char* section, const char* option, const char* value)
{
  char* msg;
  char buf[1024];
  sprintf(buf, "%s.%s.%s=%s", config, section, option, value);

  struct uci_ptr ptr;
  if (uci_lookup_ptr(UCI_CONTEXT, &ptr, buf, true) != UCI_OK) {
    uci_get_errorstr(UCI_CONTEXT, &msg, "uci_set_config");
    log_message("Could not lookup config %s - %s\n", buf, msg);
    free(msg);
    return false;
  }

  if (uci_set(UCI_CONTEXT, &ptr) != UCI_OK) {
    uci_get_errorstr(UCI_CONTEXT, &msg, "uci_set_config");
    log_message("Could not set config %s - %s\n", buf, msg);
    free(msg);
    return false;
  }

  if (uci_commit(UCI_CONTEXT, &ptr.p, false) != UCI_OK) {
    uci_get_errorstr(UCI_CONTEXT, &msg, "uci_set_config");
    log_message("Could not commit config %s - %s\n", buf, msg);
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
static char* uci_lookup_section_ref(struct uci_section* s)
{
  struct uci_type_list* ti = type_list;
  int maxlen;
  if (!s->anonymous) {
    printf("ANONYMOUS\n");
    return s->e.name;
  }
  /* look up in section type list */
  while (ti) {
    if (strcmp(ti->name, s->type) == 0)
      break;
    ti = ti->next;
  }
  if (!ti) {
    ti = malloc(sizeof(struct uci_type_list));
    memset(ti, 0, sizeof(struct uci_type_list));
    ti->next = type_list;
    type_list = ti;
    ti->name = s->type;
  }
  maxlen = strlen(s->type) + 1 + 2 + 10;
  if (!typestr) {
    typestr = malloc(maxlen);
  } else {
    typestr = realloc(typestr, maxlen);
  }
  sprintf(typestr, "@%s[%d]", ti->name, ti->idx);
  ti->idx++;
  return typestr;
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
