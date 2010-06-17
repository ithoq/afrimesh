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
void httpd_out(const char* message, ...)
{
  printl("<- ");
  va_list args;
  va_start(args, message);
  vprintf(message, args);
  vprintl(message, args);
  va_end(args);
}


/**
 *
 */
void httpd_error(const char* message, ...)
{
  va_list args;
  printf("json_error_handler([ { \"error\" : \"");
  printl("ERROR: error([ { \"error\" : \"");
  va_start(args, message);
  vprintf(message, args);
  vprintl(message, args);
  va_end(args);  
  printf("\" } ])\n");
  printl("\" } ])\n");
}


/**
 * 
 */
void whttpd_out(const wchar_t* message, ...)
{
  wprintl(L"<- ");
  va_list args;
  va_start(args, message);
  vwprintf(message, args);
  vwprintl(message, args);
  va_end(args);
}


/**
 *
 */
void whttpd_error(const wchar_t* message, ...)
{
  va_list args;
  wprintf(L"json_error_handler([ { \"error\" : \"");
  wprintl(L"ERROR: error([ { \"error\" : \"");
  va_start(args, message);
  vwprintf(message, args);
  vwprintl(message, args);
  va_end(args);  
  wprintf(L"\" } ])\n");
  wprintl(L"\" } ])\n");
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
  //printl("DECODED REQUEST: %s\n\n", buffer);
  return buffer;
}


/**
 * Converts path such as /status/self to a JSON formatted string 
 * e.g. [ "/", "status", "self" ]
 */
const wchar_t* path_to_json(const wchar_t* request, size_t length)
{
  if (request == NULL) {
    return NULL;
  }

  size_t buffer_size = length * 2 * sizeof(wchar_t);
  cgi_parse_buffer = (wchar_t*)malloc(buffer_size);
  memset(cgi_parse_buffer, 0, buffer_size);
  wchar_t* buffer = cgi_parse_buffer;
  // TODO

  return NULL;
}



/**
 * Converts a URL search string such as foo=bar&plink=plonk to a JSON formatted string
 * e.g. { "foo" : "bar", "plink" : "plonk" }
 */
const wchar_t* search_to_json(const wchar_t* search, size_t length) 
{
  if (search == NULL) {
    return NULL;
  }

  wchar_t c;
  size_t index = 0;
  size_t buffer_size = length * 2 * sizeof(wchar_t);
  int first = 0;
  int code = 0;
  
  if (cgi_parse_buffer != NULL) {
    free(cgi_parse_buffer);
    cgi_parse_buffer = NULL;
  }
  
  cgi_parse_buffer = malloc(buffer_size);
  memset(cgi_parse_buffer, 0, buffer_size);
  wchar_t * iter = cgi_parse_buffer;

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
      if(swscanf((search + index + 1), L"%2x", &code) != 1) code = '?';
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

  //printl("CONVERTED SEARCH TO: %s\n\n", cgi_parse_buffer);
  return cgi_parse_buffer;
}


/**
 * Transparently return the CGI request whether it was sent via the web or the command line
 */
const Request* cgi_request(int argc, char** argv)
{
  static Request* request = NULL;
  if (request != NULL) {
    free(request);
    request = NULL;
  } 
  request = (Request*)malloc(sizeof(Request));
  memset(request, 0, sizeof(Request));

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
  if (request_uri)     printl("    REQUEST_URI: %s\n", request_uri);
  if (script_name)     printl("    SCRIPT_NAME: %s\n", script_name);
  if (script_filename) printl("SCRIPT_FILENAME: %s\n", script_filename);
  if (path_info)       printl("      PATH_INFO: %s\n", path_info);
  if (path_translated) printl("PATH_TRANSLATED: %s\n", path_translated);
  if (request_method)  printl(" REQUEST_METHOD: %s\n", request_method);
  if (content_type)    printl("   CONTENT_TYPE: %s\n", content_type);
  if (content_length)  printl(" CONTENT_LENGTH: %s\n", content_length);
  if (query_string)    printl("   QUERY_STRING: %s\n", query_string); 

  /* parse request - Method */
  if (request_method == NULL && argc >= 1 && argv[1]) { // command line
    request_method = argv[1];
  } 
  request->method = string_to_symbol(request_method, Methods);
  if (request->method == SYMBOL_UNKNOWN) {
    printl("METHOD NOT FOUND!\n");
    // TODO - fail nicely
  }

  /* parse request - HREF */
  if (cgi_href_buffer != NULL) {
    free(cgi_href_buffer);
    cgi_href_buffer = NULL;
  }
  if (path_info) {                                      // web
    size_t buffer_size = (strlen(path_info)+1) * sizeof(wchar_t);
    if (query_string) {
      buffer_size += strlen(query_string);
    }
    cgi_href_buffer = (wchar_t*)malloc(buffer_size);
    memset(cgi_href_buffer, 0, buffer_size);
    swprintf(cgi_href_buffer, buffer_size, L"%s%s%s", path_info, (query_string ? "?" : ""), (query_string ? query_string : ""));
    request->href     = cgi_href_buffer;
    request->pathname = wcsdupchar(path_info);
    request->search   = wcsdupchar(query_string);
  } else if (argc >= 2 && argv[2]) {                     // command line
    request->href     = wcsdupchar(argv[2]);
    size_t buffer_size = (strlen(argv[2])+1) * sizeof(char);
    char* buffer = (char*)malloc(buffer_size);
    memset(buffer, 0, buffer_size);
    snprintf(buffer, buffer_size, "%s", argv[2]);
    char* state;
    request->pathname = wcsdupchar(strtok_r(buffer, "?", &state));
    request->search   = wcsdupchar(strtok_r(NULL, "?", &state));
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
    cgi_post_buffer = malloc((read_expected+1) * sizeof(char));
    while (read_actual < read_expected) {
      size_t read = fread(cgi_post_buffer, 1, read_expected, stdin);
      read_actual += read;
      cgi_post_buffer[read_actual] = 0;
      //printl("GOT: %d %s\n", read_actual, cgi_post_buffer);
      if (read == 0) {
        break;
      }
    }    
    request->data = cgi_post_buffer;
  }

  return request;
}



/**
 *
 */
void cgi_release()
{
  if (cgi_post_buffer != NULL) {
    free(cgi_post_buffer);
    cgi_post_buffer = NULL;
  }
  if (cgi_parse_buffer != NULL) {
    free(cgi_parse_buffer);
    cgi_parse_buffer = NULL;
  }
  if (cgi_href_buffer != NULL) {
    /* TODO - Utterly bizarre gcc bug giving: 
         villagebus(96815) malloc: *** error for object 0x100140: pointer being freed was not allocated
         *** set a breakpoint in malloc_error_break to debug  
       Early investigations say it may be only latest XCode release that sufferes from this */
    //free(cgi_href_buffer);  
  }
}



