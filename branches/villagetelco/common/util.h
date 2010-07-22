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

#ifndef UTIL
#define UTIL

/* logging */
static const int log_level = 1;
static const char* log_filename = "/tmp/village-bus.log";
static FILE* log_file = NULL;
void printl(const char* message, ...);
void vprintl(const char* message, va_list ap);
void wprintl(const wchar_t* message, ...);
void vwprintl(const wchar_t* message, va_list ap);
void log_release();

/* string utilities */
char* triml(char* s);
char* trimr(char* s);
char* trim(char *s);
char* substring(char *s, size_t start, size_t count);

/* wide string utilities */
wchar_t* wcsdupchar(const char* string);
//wchar_t* wcsdup(const wchar_t* string);
int vaswprintf(wchar_t** result, const wchar_t* format, va_list args);

/* file utilities */
char* path_exists(const char* name);

/* parse utilities */
struct Symbol {
  int    symbol;
  size_t length;
  const  char* string;
  char* (*lambda) ();
};
typedef struct Symbol SymbolTable [];
static const int SYMBOL_UNKNOWN = -1;
int string_to_symbol(const char* string, const SymbolTable symbols);

char* cut_field(const char* input, size_t start, size_t end);
const char* parse_field(const char* input, size_t length, char* tokens, char** pfield);

/* ip address utilities */


#endif /* UTIL */
