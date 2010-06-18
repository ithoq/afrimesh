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

#include "util.h"



/**
 * Logging
 */
void printl(const char* message, ...)
{
  if (log_level == 0) return;
  va_list args;
  va_start(args, message);
  vprintl(message, args);
  va_end(args);
}

void vprintl(const char* message, va_list ap)
{
  if (log_level == 0) return;
  /* Initialize log if necessary */
  if (log_file == NULL) {
    log_file = fopen(log_filename, "a");
    //log_file = stdout;
    if (log_file == NULL) {
      perror("Could not open log file");
      return;
    }
  }
  vfprintf(log_file, message, ap);
  fflush(log_file);
}

void wprintl(const wchar_t* message, ...)
{
  if (log_level == 0) return;
  va_list args;
  va_start(args, message);
  vwprintl(message, args);
  va_end(args);
}

void vwprintl(const wchar_t* message, va_list ap)
{
  if (log_level == 0) return;
  /* Initialize log if necessary */
  if (log_file == NULL) {
    log_file = fopen(log_filename, "a");
    //log_file = stdout;
    if (log_file == NULL) {
      perror("Could not open log file");
      return;
    }
  }
  vfwprintf(log_file, message, ap);
  fflush(log_file);
}


void log_release() 
{
  printl("============================================================\n\n");
  if (log_file != NULL) {
    fclose(log_file);
  }
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

char* substring(char *s, size_t start, size_t count)
{
  if (start + count > strlen(s)) {
    return NULL;
  }
  char* ret = malloc((sizeof(char) * count) + 1);
  strncpy(ret, s + start, count);
  return ret;
}

wchar_t* wcsdupchar(const char* string)
{
  if (string == NULL) return NULL;
  size_t length = strlen(string);
  size_t buffer_size = sizeof(wchar_t) * (length + 1);
  wchar_t* buffer = malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  if (mbstowcs(buffer, string, length) != length) {
    return NULL; 
  }
  buffer[length] = L'\0';
  return buffer;
}

int vaswprintf(wchar_t** result, const wchar_t* format, va_list args)
{
  va_list ap;
  va_copy(ap, args);
  FILE* devnull = fopen("/dev/null", "w");
  int length = vfwprintf(devnull, format, ap);  // TODO - this is mildly evil
  fclose(devnull);
  va_end(ap);
  if (length == -1) {
    return -1;
  }

  wchar_t *buffer = (wchar_t*)malloc((length + 1) * sizeof(wchar_t));
  if (buffer == NULL) {
    return -1;
  }
  
  va_list aq;
  va_copy(aq, args);
  length = vswprintf(buffer, length + 1, format, aq);
  va_end (aq);
  if (length == -1) {
    return -1;
  }
  *result = buffer;

  return length;
}


/**
 * Parsing utilities
 */
int string_to_symbol(const char* string, const SymbolTable symbols)
{
  if (string == NULL) {
    return SYMBOL_UNKNOWN;
  }
  size_t t = 0;
  for (t = 0; symbols[t].symbol != SYMBOL_UNKNOWN; t++) {
    if (strncmp(string, symbols[t].string, symbols[t].length) == 0) {
      return symbols[t].symbol;
    }
  }
  return SYMBOL_UNKNOWN;
}

