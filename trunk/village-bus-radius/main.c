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
#include <json_cgi.h>
#include <uci_util.h>
#include <village-bus-radius.h>

int main(int argv, char** argc)
{
  char *test = malloc (1024);
  if (uci_get(test, "afrimesh", "radius", "radutmp")) {
    radutmp_filename = malloc(1024);
    sprintf (radutmp_filename, "%s", test);
  }
  if (uci_get(test, "afrimesh", "radius", "server")) {
    radius_mysql_server = malloc(1024);
    sprintf (radius_mysql_server, test);
  }
  if (uci_get(test, "afrimesh", "radius", "radtype")) {
    radius_database_type = malloc(1024);
    sprintf (radius_database_type, test);
  }
  if (uci_get(test, "afrimesh", "radius", "database")) {
    radius_mysql_database = malloc(1024);
    sprintf (radius_mysql_database, test);
  }
  if (uci_get(test, "afrimesh", "radius", "username")) {
    radius_mysql_username = malloc(1024);
    sprintf (radius_mysql_username, test);
  }
  if (uci_get(test, "afrimesh", "radius", "password")) {
    radius_mysql_password = malloc(1024);
    sprintf (radius_mysql_password, test);
  }
  free(test);

  //printf("Content-type: text/plain\n\n");
  printf("Content-type: application/json\n\n");

  /* parse request */
  char* request = json_cgi_request(); 
  if (request == NULL || strcasecmp(request, "") == 0) {  /* TODO - return NULL on json_cgi_request always */
    log_message("NULL request\n");if (UCI_CONTEXT == NULL) {
    return NULL;
  }
    printf("[\n\t{ \"error\" : \"NULL request\" }\n]\n");
    json_cgi_release();
    return EXIT_FAILURE;
  } 

  struct json_object* request_object = json_tokener_parse(request);
  if (request_object == NULL) {if (UCI_CONTEXT == NULL) {
    return NULL;
  }
    log_message("Could not parse request: %s\n", request);
    printf("[\n\t{ \"error\" : \"Could not parse request: %s\" }\n]\n", request);
    json_cgi_release();
    return EXIT_FAILURE;
  }
  char* command = json_object_get_string(json_object_object_get(request_object, "command"));

  /* execute request command */
  printf("[");
  if (strncasecmp("who", command, 3) == 0) {
    radius_who(); 

  } else if (strncasecmp("list", command, 4) == 0) {
      if(strcmp("1", radius_database_type) == 0) {
        radius_list_mysql();
      } else {
        radius_list_memcachedb();
      }

  } else if (strncasecmp("new", command, 3) == 0) {
    char* username = json_object_get_string(json_object_object_get(request_object, "username"));
    char* type     = json_object_get_string(json_object_object_get(request_object, "type"));
    int   seconds  = json_object_get_int   (json_object_object_get(request_object, "seconds"));
    if(strcmp("1", radius_database_type) == 0) {
      radius_new_mysql(username, type, seconds);
    } else {
        radius_new_memcachedb(username, type, seconds);
    }

  } else if (strncasecmp("delete", command, 6) == 0) {
    char* username = json_object_get_string(json_object_object_get(request_object, "username"));    
    if(strcmp("1", radius_database_type) == 0) {
      radius_delete_mysql(username);
    } else {
        radius_delete_memcachedb(username);
    }

  } else if (strncasecmp("modify", command, 6) == 0) {
    char* username = json_object_get_string(json_object_object_get(request_object, "username"));
    char* new_username = json_object_get_string(json_object_object_get(request_object, "new_username"));
    char* new_password = json_object_get_string(json_object_object_get(request_object, "new_password"));
    char* new_type     = json_object_get_string(json_object_object_get(request_object, "new_type"));
    if(strcmp("1", radius_database_type) == 0) {
      radius_modify_mysql(username, new_username, new_password, new_type);
    } else {
        radius_modify_memcachedb(username, new_username, new_password, new_type);
    }

  } else {
    printf("{ \"error\" : \"unknown command\" }");
  }
  printf("\n]\n");

  /* release resources */
  json_object_put(request_object);
  json_cgi_release();

  return EXIT_SUCCESS;
}


/**
 * Executes a sql query.
 *
 * If connection is NULL a new connection will be created and returned by reference.
 *
 * Remember to call mysql_free_result on the result (if any) and mysql_close on
 * the connection when you are done.
 *
 * Returns 0 on success.
 */
int mysql(MYSQL** connection, const char* query, ...)
{
  if (query == NULL) {
    printf("\t{\n\t\t\"error\" : \"Cannot perform null query\"\n\t}\n");
    return -1;
  }

  /* connect to database if needed */
  if (*connection == NULL) {
    *connection = mysql_init(NULL);
    if (!mysql_real_connect(*connection,
			    radius_mysql_server,
			    radius_mysql_username,
			    radius_mysql_password,
			    radius_mysql_database, 0 , NULL, 0)) {
      char* message = mysql_error(*connection);
      log_message("Failed to connect to mysql server: %s\n", radius_mysql_server);
      log_message("Message was: %s\n", message);
      printf("\t{\n\t\t\"error\" : \"%s\"\n\t}\n", message);
      return -1;
    }
  }

  /* assemble query */
  va_list args;
  va_start(args, query);
  char* final_query;
  if ((vasprintf(&final_query, query, args) == -1) || final_query == NULL) {
      printf("\t{\n\t\t\"error\" : \"Could not build query '%s'\"\n\t}\n", query);
      return -1;    
  }
  va_end(args);

  /* execute query */
  if (mysql_query(*connection, final_query) != 0) {
    char* message = mysql_error(*connection);
    log_message("Error executing mysql query: %s\n", final_query);
    log_message("Message was: %s\n", message);
    printf("\t{\n\t\t\"error\" : \"%s\"\n\t}\n", message);
    return -1;
  } 
  free(final_query);

  return 0;
}
