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

#include "cgi.h"


/**
 * Symbols 
 */
static const SymbolTable Methods = {
  { OPTIONS, 7, "OPTIONS", NULL },
  { GET,     3, "GET",     NULL },
  { HEAD,    4, "HEAD",    NULL },
  { POST,    4, "POST",    NULL },
  { PUT,     3, "PUT",     NULL },
  { DELETE,  6, "DELETE",  NULL },
  { PATCH,   5, "PATCH",   NULL },
  { CONNECT, 7, "CONNECT", NULL },
  { TRACE,   5, "TRACE",   NULL },
  { -1, 0, NULL, NULL }
};


/**
 * 
 */
void cgi_out(Request* self, const wchar_t* message, ...)
{
  // print HTTP headers if needed
  if (self && self->state == OUT_HEADER) {
    //wprintf(L"Content-type: text/plain\n\n");
    //wprintf(L"Content-type: application/json\n\n");  
    wprintf(L"Content-type: application/x-javascript\n\n"); // Grrf
    wprintf(L"%S(", self->callback);
    self->state = OUT_BODY;
  }

  // print output
  va_list args;
  va_start(args, message);
  vwprintf(message, args);
  vwprintl(message, args);
  va_end(args);
}


/**
 *
 */
const char* cgi_decode(const char* request, size_t length)
{
  static char* buffer = NULL;
  if (buffer != NULL) {
    free(buffer);
    buffer = NULL;
  }

  char c;
  size_t index = 0;
  size_t buffer_size = length * 2 * sizeof(char);
  int code = 0;
  buffer = malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  char* iter = buffer;
  for (index = 0; request[index] != '\0' && index < length; index++) {
    c = request[index];
    switch (c) {
    case '%':
      if(sscanf((request + index + 1), "%2x", &code) != 1) code = '?';
      index += 2;
      iter += snprintf(iter, buffer_size, "%c", code);
      break;
    default:
      iter += snprintf(iter, buffer_size, "%c", c);
      break;
    }
  }
  //wprintl(L"DECODED REQUEST: %s\n\n", buffer);
  return buffer;
}


/**
 * Converts path such as /status/self to a JSON formatted string 
 * e.g. [ "/", "status", "self" ]
 */
wchar_t* path_to_json(const wchar_t* request, size_t length)
{
  if (request == NULL) {
    return NULL;
  }

  size_t buffer_size = sizeof(wchar_t) * (length + 1) * 2;
  wchar_t* buffer = (wchar_t*)malloc(buffer_size);
  memset(buffer, 0, buffer_size);

  // TODO - implementation

  return buffer;
}



struct json_object* search_to_json(const char* search, size_t length)
{
  if (search == NULL) {
    return NULL;
  }
  char* s = search_to_json_string(search, length);
  struct json_object* json = json_tokener_parse(s);
  free(s);
  if (!json) {
    return NULL;
  }
  return json;
}


/**
 * Converts a URL search string such as foo=bar&plink=plonk to a JSON formatted string
 * e.g. { "foo" : "bar", "plink" : "plonk" }
 */
char* search_to_json_string(const char* search, size_t length) 
{
  if (search == NULL) {
    return NULL;
  }
  char c;
  size_t index = 0;
  size_t buffer_size = sizeof(char) * (length + 1) * 3;
  char* buffer = (char*)malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  int first = 0;
  int code = 0;
  char* iter = buffer;
  iter += snprintf(iter, buffer_size, "{ '");
  for (index = 0; search[index] != '\0' && index < length; index++) {
    c = search[index];
    switch (c) {
    case '=':
      iter += snprintf(iter, buffer_size, "' : '");
      break;
    case '+':
      iter += snprintf(iter, buffer_size, " ");
      break;
    case '%':
      if(sscanf((search + index + 1), "%2x", &code) != 1) code = '?';
      index += 2;
      iter += snprintf(iter, buffer_size, "%c", code);
      break;
    case '?':
      break;
    case '&':
      iter += snprintf(iter, buffer_size, "', '");
      break;
    default:
      iter += snprintf(iter, buffer_size, "%c", c);
      break;
    }
  }
  iter += snprintf(iter, buffer_size, "' }");

  return buffer;
}


/**
 * Converts a URL search string such as foo=bar&plink=plonk to a JSON formatted string
 * e.g. { "foo" : "bar", "plink" : "plonk" }
 */
wchar_t* wsearch_to_json_string(const wchar_t* search, size_t length) 
{
  if (search == NULL) {
    return NULL;
  }
  wchar_t c;
  size_t index = 0;
  size_t buffer_size = sizeof(wchar_t) * (length + 1) * 3;
  wchar_t* buffer = (wchar_t*)malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  int first = 0;
  int code = 0;
  wchar_t * iter = buffer;
  iter += swprintf(iter, buffer_size, L"{ '");
  for (index = 0; search[index] != L'\0' && index < length; index++) {
    c = search[index];
    switch (c) {
    case L'=':
      iter += swprintf(iter, buffer_size, L"' : '");
      break;
    case L'+':
      iter += swprintf(iter, buffer_size, L" ");
      break;
    case L'%':
      if(swscanf((search + index + 1), L"%2x", &code) != 1) code = L'?';
      index += 2;
      iter += swprintf(iter, buffer_size, L"%c", code);
      break;
    case L'?':
      break;
    case L'&':
      iter += swprintf(iter, buffer_size, L"', '");
      break;
    default:
      iter += swprintf(iter, buffer_size, L"%c", c);
      break;
    }
  }
  iter += swprintf(iter, buffer_size, L"' }");

  return buffer;
}


/**
 * Transparently return the CGI request whether it was sent via the web or the command line
 */
Request* cgi_request(int argc, char** argv)
{
  static Request* request = NULL;
  if (request != NULL) {
    free(request);
    request = NULL;
  } 
  request = (Request*)malloc(sizeof(Request));
  memset(request, 0, sizeof(Request));
  request->callback = L"jsonp"; // TODO - make optional ?
  request->out      = cgi_out;
  request->state    = OUT_RAW;

  char* request_method;
  char* content_type;
  char* content_length;
  char* query_string;
  char* request_uri;
  char* script_name;   
  char* script_filename;
  char* path_info;      
  char* path_translated;

  request_method  = getenv("REQUEST_METHOD");
  query_string    = getenv("QUERY_STRING");
  query_string    = (query_string && strlen(query_string) ? query_string : NULL);
  content_type    = getenv("CONTENT_TYPE");
  content_length  = getenv("CONTENT_LENGTH");
  request_uri     = getenv("REQUEST_URI");
  script_name     = getenv("SCRIPT_NAME");
  script_filename = getenv("SCRIPT_FILENAME");
  path_info       = getenv("PATH_INFO");
  path_translated = getenv("PATH_TRANSLATED");
  if (request_uri)     wprintl(L"    REQUEST_URI: %s\n", request_uri);
  if (script_name)     wprintl(L"    SCRIPT_NAME: %s\n", script_name);
  if (script_filename) wprintl(L"SCRIPT_FILENAME: %s\n", script_filename);
  if (path_info)       wprintl(L"      PATH_INFO: %s\n", path_info);
  if (path_translated) wprintl(L"PATH_TRANSLATED: %s\n", path_translated);
  if (request_method)  wprintl(L" REQUEST_METHOD: %s\n", request_method);
  if (content_type)    wprintl(L"   CONTENT_TYPE: %s\n", content_type);
  if (content_length)  wprintl(L" CONTENT_LENGTH: %s\n", content_length);
  if (query_string)    wprintl(L"   QUERY_STRING: %s\n", query_string); 

  /* parse request - Method */
  if (request_method == NULL && argc >= 1 && argv[1]) {    // command line
    request_method = argv[1];
  } else if (request_method == NULL) {                     // command line but no params
    request->method = SYMBOL_UNKNOWN;
    return request;
  } else if (strncmp(request_method, "CONSOLE", 7) == 0) { // command line from cgi @ugly!
    request_method = argv[1];
  } else {                                                 // web
    request->state = OUT_HEADER;
  }
  request->method = string_to_symbol(request_method, Methods);
  if (request->method == SYMBOL_UNKNOWN) {
    wprintl(L"METHOD NOT FOUND: %s\n", request_method);
    return request;
  }

  /* parse request - HREF */
  if (cgi_href_buffer != NULL) {
    free(cgi_href_buffer);
    cgi_href_buffer = NULL;
  }

  if (path_info) {                                      // web
    size_t buffer_size = strlen(path_info) + 1;
    if (query_string) {
      buffer_size += strlen(query_string) + 1;
    }
    buffer_size = (buffer_size) * sizeof(wchar_t);
    cgi_href_buffer = (wchar_t*)malloc(buffer_size);
    memset(cgi_href_buffer, 0, buffer_size);
    swprintf(cgi_href_buffer, buffer_size, L"%s%s%s", path_info, (query_string ? "?" : ""), (query_string ? query_string : ""));
    request->href     = cgi_href_buffer;
    request->pathname = wcsdupchar(path_info);
    request->search   = query_string ? wcsdupchar(query_string) : NULL;
    request->json     = query_string ? search_to_json(query_string, strlen(query_string)) : NULL;

  } else if (argc >= 2 && argv[2]) {                     // command line
    request->href     = wcsdupchar(argv[2]);
    size_t length = strlen(argv[2]);
    size_t buffer_size = sizeof(char) * (length + 1);
    char* buffer = (char*)malloc(buffer_size);
    memset(buffer, 0, buffer_size);
    snprintf(buffer, buffer_size, "%s", argv[2]);
    buffer[length] = '\0';
    char* state;
    path_info         = strtok_r(buffer, "?", &state);
    query_string      = strtok_r(NULL, "?", &state);
    request->pathname = wcsdupchar(path_info);
    request->search   = query_string ? wcsdupchar(query_string) : NULL;
    request->json     = query_string ? search_to_json(query_string, strlen(query_string)) : NULL;
  }  

  /* parse request - jsonp Callback */
  if (request->json) {
    const char* callback = json_object_get_string(json_object_object_get(request->json, "callback"));
    request->callback = callback ? wcsdupchar(callback) : request->callback;
    json_object_object_del(request->json, "callback"); // remove reference to callback
    // set request->json to NULL if there are no other parameter
    size_t count = 0;
    struct lh_entry* iter;
    for (iter = json_object_get_object(request->json)->head; iter ; iter = iter->next) {
      count++;
    }
    if (count == 0) {
      request->json = NULL;
    }
  }

  /* parse request - Data */
  if (cgi_post_buffer != NULL) {
    free(cgi_post_buffer);
    cgi_post_buffer = NULL;
  }
  if (request->method == POST ||
      request->method == PUT  ||
      request->method == PATCH) {
    size_t read_actual = 0;
    size_t read_expected = 2048;
    if (content_length) {
      read_expected = atoi(content_length);
    }
    cgi_post_buffer = malloc((read_expected + 1) * sizeof(char));
    while (read_actual < read_expected) {
      size_t read = fread(cgi_post_buffer, 1, read_expected, stdin);
      read_actual += read;
      cgi_post_buffer[read_actual] = 0;
      //wprintl(L"GOT: %d %s\n", read_actual, cgi_post_buffer);
      if (read == 0) {
        //wprintl(L"fin\n");
        break;
      }
    }    
    request->data = cgi_post_buffer;
    //if (request->data) wprintl(L"   CONTENT_DATA: %s\n", request->data);
  }

  return request;
}


/**
 *
 */
void cgi_init() 
{
  cgi_href_buffer = NULL;
  cgi_post_buffer = NULL;
}



/**
 *
 */
void cgi_release(Request* self)
{
  if (self && self->state == OUT_BODY) {
    //wprintf(L"Content-type: text/plain\n\n");
    //wprintf(L"Content-type: application/json\n\n");  
    wprintf(L")\n");
    self->state == OUT_RAW;
  }
  // TODO - lose static global buffers
  if (cgi_post_buffer != NULL) {
    free(cgi_post_buffer);
    cgi_post_buffer = NULL;
  }
  if (cgi_href_buffer != NULL) {
    free(cgi_href_buffer);  
    cgi_href_buffer = NULL;
  }
}



