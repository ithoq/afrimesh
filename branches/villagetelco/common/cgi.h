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

#ifndef CGI
#define CGI

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

#include <json/json.h> // TODO - deprecate in favor of fexpressions

#include <util.h>


typedef enum { OUT_HEADER = 1, OUT_BODY = 2, OUT_RAW = 3 } OutState;
typedef enum { OPTIONS, GET, HEAD, POST, PUT, DELETE,  PATCH, CONNECT, TRACE } Method;
typedef struct _Request {
  Method method;            // POST
  wchar_t* href;            // /status/self?foo=bar&plink=plonk
  wchar_t* pathname;        // /status/self
  wchar_t* search;          // foo=bar&plink=plonk
  struct json_object* json; // { 'foo' : 'bar', 'plink' : 'plonk' }
  wchar_t* callback;        // jsonp1277450195457
  unsigned char* data;      // { some raw data }
  OutState state;
  void (*out) (struct _Request*, const wchar_t*, ...);
} Request;

static wchar_t* cgi_href_buffer  = NULL; // TODO - embed global static buffer in Request object 
static char*    cgi_post_buffer  = NULL; // TODO - embed global static buffer in Request object 

void     cgi_init();
Request* cgi_request(int argc, char** argv);
void     cgi_release(Request* self);
void     cgi_out(Request* self, const wchar_t* message, ...);

const char* cgi_decode(const char* request, size_t length);

struct json_object* search_to_json(const char* search, size_t length);
char*    search_to_json_string(const char* search, size_t length);
wchar_t* wsearch_to_json_string(const wchar_t* search, size_t length);

#endif /* CGI */
