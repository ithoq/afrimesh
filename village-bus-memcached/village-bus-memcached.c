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


#include "village-bus-memcached.h"





char* get(memcached_st* memcached, const char* key)
{
  size_t value_length;
  uint32_t flags;
  memcached_return ret;

  char* value = memcached_get(memcached, key, strlen(key), &value_length, &flags, &ret);
  if (ret == MEMCACHED_SUCCESS) {
    //printf("key: %s\nflags: %x\nlength: %zu\nvalue: %s\n", key, flags, (int)value_length, value);
  } else if (ret == MEMCACHED_NOTFOUND) {
    value = strdup("not found");
  } else { 
    value = malloc(512);
    snprintf(value, 512, "\"'%s' - %s\"", 
             key, 
             memcached_strerror(memcached, ret), 
             ((memcached->cached_errno) ? strerror(memcached->cached_errno) : ""));
  }
  return value;
}


/**
 * @return value on success
 */
char* set(memcached_st* memcached, const char* key, const char* value)
{
  memcached_return ret;
  char* msg = (char*)value;

  ret = memcached_set(memcached, key, strlen(key), value, strlen(value), 0, 0);

  if (ret != MEMCACHED_SUCCESS) {
    msg = malloc(512);
    snprintf(msg, 512, "\"'%s' - %s\"", 
             key, 
             memcached_strerror(memcached, ret), 
             ((memcached->cached_errno) ? strerror(memcached->cached_errno) : ""));
  }

  return msg;
}
