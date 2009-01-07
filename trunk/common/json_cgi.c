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

  va_list args;
  va_start(args, message);
  vfprintf(json_cgi_log_file, message, args);
  va_end(args);
  fflush(json_cgi_log_file);
}



/**
 *
 */
void json_cgi_release()
{
  if (json_cgi_log_file != NULL && json_cgi_log_file != stdout) {
    fprintf(json_cgi_log_file, "Finished logging.\n\n");
  }
  if (json_cgi_log_file != NULL) {
    fclose(json_cgi_log_file);
  }

  if (json_cgi_post_buffer != NULL) {
    free(json_cgi_post_buffer);
  }
}



/**
 *
 */
char* json_cgi_request()
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
    if (json_cgi_post_buffer != NULL) {
      free(json_cgi_post_buffer);
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
