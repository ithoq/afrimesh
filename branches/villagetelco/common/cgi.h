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

#include <util.h>


typedef enum { OPTIONS, GET, HEAD, POST, PUT, DELETE,  PATCH, CONNECT, TRACE } Method;
typedef struct {
  Method method;        // POST
  wchar_t* href;        // /status/self?foo=bar&plink=plonk
  wchar_t* pathname;    // /status/self
  wchar_t* search;      // foo=bar&plink=plonk
  unsigned char* data;  // { some string }
} Request;

static wchar_t* cgi_href_buffer  = NULL;
static wchar_t* cgi_parse_buffer = NULL;
static char*    cgi_post_buffer  = NULL;

void httpd_out(const char* message, ...);
void httpd_error(const char* message, ...);
void whttpd_out(const wchar_t* message, ...);
void whttpd_error(const wchar_t* message, ...);


const Request* cgi_request(int argc, char** argv);
const char* cgi_decode(const char* request, size_t length);
void cgi_release();

const wchar_t* search_to_json(const wchar_t* search, size_t length);

#endif /* CGI */
