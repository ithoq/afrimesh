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

#include "json_cgi.h"


/**
 *
 */
void log_message(const char* message, ...)
{
#ifdef DISABLE_LOG
  return;
#endif
  va_list args;
  va_start(args, message);
  vlog_message(message, args);
  va_end(args);
}

void vlog_message(const char* message, va_list ap)
{
#ifdef DISABLE_LOG
  return;
#endif

  /* Initialize log */
  if (json_cgi_log_file == NULL) {
    json_cgi_log_file = fopen(json_cgi_log_filename, "a");
    //json_cgi_log_file = stdout;
    if (json_cgi_log_file == NULL) {
      perror("Could not open log file");
      return;
    }
    fprintf(json_cgi_log_file, "Started logging...\n");
  }

  vfprintf(json_cgi_log_file, message, ap);
  fflush(json_cgi_log_file);
}




/**
 *
 */
void json_cgi_release()
{
  if (json_cgi_log_file != NULL && json_cgi_log_file != stdout) {
    fprintf(json_cgi_log_file, "Finished logging.\n");
    fprintf(json_cgi_log_file, "============================================================\n\n");
  }
  if (json_cgi_log_file != NULL) {
    fclose(json_cgi_log_file);
  }

  if (json_cgi_post_buffer != NULL) {
    free(json_cgi_post_buffer);
    json_cgi_post_buffer = NULL;
  }
  if (json_cgi_parse_buffer != NULL) {
    free(json_cgi_parse_buffer);
    json_cgi_parse_buffer = NULL;
  }
}


char* cgi_decode(const char* request, size_t length)
{
  char c;
  size_t index = 0;
  size_t buffer_size = length * 2 * sizeof(char);
  int code = 0;
  if (json_cgi_parse_buffer != NULL) {
    free(json_cgi_parse_buffer);
    json_cgi_parse_buffer = NULL;
  }
  json_cgi_parse_buffer = malloc(buffer_size);
  memset(json_cgi_parse_buffer, 0, buffer_size);
  char* buffer = json_cgi_parse_buffer;
  for (index = 0; request[index] != '\0' && index < length; index++) {
    c = request[index];
    switch (c) {
    case '%':
      if(sscanf((request + index + 1), "%2x", &code) != 1) code = '?';
      index += 2;
      buffer += snprintf(buffer, buffer_size, "%c", code);
      break;
    default:
      buffer += snprintf(buffer, buffer_size, "%c", c);
      break;
    }
  }
  //log_message("DECODED REQUEST: %s\n\n", json_cgi_parse_buffer);
  return json_cgi_parse_buffer;
}


char* request_to_json(const char* request, size_t length) 
{
  char c;
  size_t index = 0;
  size_t buffer_size = length * 2 * sizeof(char);
  int first = 0;
  int code = 0;

  if (json_cgi_parse_buffer != NULL) {
    free(json_cgi_parse_buffer);
    json_cgi_parse_buffer = NULL;
  }
  
  json_cgi_parse_buffer = malloc(buffer_size);
  memset(json_cgi_parse_buffer, 0, buffer_size);
  char * buffer = json_cgi_parse_buffer;

  buffer += snprintf(buffer, buffer_size, "{ '");

  for (index = 0; request[index] != '\0' && index < length; index++) {
    c = request[index];
    switch (c) {
    case '=':
      buffer += snprintf(buffer, buffer_size, "' : '");
      break;
    case '+':
      buffer += snprintf(buffer, buffer_size, " ");
      break;
    case '%':
      if(sscanf((request + index + 1), "%2x", &code) != 1) code = '?';
      index += 2;
      buffer += snprintf(buffer, buffer_size, "%c", code);
      break;
    case '?':
      break;
    case '&':
      buffer += snprintf(buffer, buffer_size, "', '");
      break;
    default:
      buffer += snprintf(buffer, buffer_size, "%c", c);
      break;
    }
  }

  buffer += snprintf(buffer, buffer_size, "' }");

  //log_message("CONVERTED REQUEST TO: %s\n\n", json_cgi_parse_buffer);
  return json_cgi_parse_buffer;
}


/**
 * Transparently return the CGI request whether it was sent via GET, POST or the command line
 */
const char* cgi_request(int argc, char** argv)
{
  char* request_method;
  char* content_type;
  char* content_length;
  char* query_string;
  char* request_uri;
  char* script_name;   
  char* script_filename;
  char* path_info;      
  char* path_translated;
  char* request;

  request_method  = getenv("REQUEST_METHOD");
  query_string    = getenv("QUERY_STRING");
  content_type    = getenv("CONTENT_TYPE");
  content_length  = getenv("CONTENT_LENGTH");
  request_uri     = getenv("REQUEST_URI");
  script_name     = getenv("SCRIPT_NAME");
  script_filename = getenv("SCRIPT_FILENAME");
  path_info       = getenv("PATH_INFO");
  path_translated = getenv("PATH_TRANSLATED");
  /*if (request_uri)     log_message("    REQUEST_URI: %s\n", request_uri);
  if (script_name)     log_message("    SCRIPT_NAME: %s\n", script_name);
  if (script_filename) log_message("SCRIPT_FILENAME: %s\n", script_filename);
  if (path_info)       log_message("      PATH_INFO: %s\n", path_info);
  if (path_translated) log_message("PATH_TRANSLATED: %s\n", path_translated);
  if (request_method)  log_message(" REQUEST_METHOD: %s\n", request_method);
  if (content_type)    log_message("   CONTENT_TYPE: %s\n", content_type);
  if (content_length)  log_message(" CONTENT_LENGTH: %s\n", content_length);
  if (query_string)    log_message("   QUERY_STRING: %s\n", query_string); */

  if (query_string && request_method && strncmp(request_method, "GET", 3) == 0) {
    request = query_string;

  } else if (request_method && strncmp(request_method, "POST", 4) == 0) {
    if (json_cgi_post_buffer != NULL) {
      free(json_cgi_post_buffer);
      json_cgi_post_buffer = NULL;
    }
    size_t read_actual = 0;
    size_t read_expected = atoi(content_length);
    json_cgi_post_buffer = malloc(read_expected + 1 * sizeof(char));
    //log_message("POST_CONTENT: ");
    while (read_actual < read_expected) {
      read_actual += fread(json_cgi_post_buffer, 1, read_expected, stdin);
      json_cgi_post_buffer[read_actual] = 0;
      //log_message("%s", json_cgi_post_buffer);
    }
    //log_message("\n");
    request = json_cgi_post_buffer;

  } else if (argc == 2 && argv && argv[1]) {
    request = argv[1];

  } else if (argc == 3 && argv && argv[2]) {  /* request path is argv[1] */
    request = argv[2];

  } else {
    log_message("Unknown request: %s\n", request_method);
    request = NULL;
  }

  return request;
}


/**
 *
 */
const char* json_cgi_request()
{
  char* request_method;
  char* content_type;
  char* content_length;
  char* query_string;

  request_method = getenv("REQUEST_METHOD");
  query_string   = getenv("QUERY_STRING");
  content_type   = getenv("CONTENT_TYPE");
  content_length = getenv("CONTENT_LENGTH");

  if (request_method) log_message("REQUEST_METHOD: %s\n", request_method);
  if (content_type)   log_message("CONTENT_TYPE: %s\n", content_type);
  if (content_length) log_message("CONTENT_LENGTH: %s\n", content_length);

  if (query_string && request_method && strncmp(request_method, "GET", 3) == 0) {
    log_message("QUERY_STRING: %s\n", query_string);
    return query_string;

  } else if (request_method && strncmp(request_method, "POST", 4) == 0) {
    /* TODO - w/ JSONP & POST the 'jsonp=callbackname' parameter gets passed in the query string */
    if (json_cgi_post_buffer != NULL) {
      free(json_cgi_post_buffer);
      json_cgi_post_buffer = NULL;
    }
    size_t read_actual = 0;
    size_t read_expected = atoi(content_length);
    json_cgi_post_buffer = malloc(read_expected + 1 * sizeof(char));
    log_message("POST_CONTENT: ");
    while (read_actual < read_expected) {
      read_actual += fread(json_cgi_post_buffer, 1, read_expected, stdin);
      json_cgi_post_buffer[read_actual] = 0;
      log_message("%s", json_cgi_post_buffer);
    }
    log_message("\n");
    return json_cgi_post_buffer;

  } 

  log_message("Unknown request: %s\n", request_method);
  return "";
}



/**
 * String utilities
 */
char* triml(char* s)
{
  while (isspace(*s)) s++;
  return s;
}

char* trimr(char* s)
{
  char* _s = s + strlen(s);
  while (isspace(*--_s));
  *(_s + 1) = '\0';
  return s;
}

char* trim(char *s)
{
  return trimr(triml(s)); 
}
