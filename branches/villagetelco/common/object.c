/* Copyright (c) 2007 by Ian Piumarta.
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * The Software is provided "as is".  Use entirely at your own risk.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"


struct _vtable *SymbolList= 0;

struct _vtable *vtable_vt;
struct _vtable *object_vt;
struct _vtable *symbol_vt;
struct _vtable *closure_vt;

struct _object *s_addMethod = 0;
struct _object *s_allocate  = 0;
struct _object *s_delegated = 0;
struct _object *s_lookup    = 0;


/*extern*/ inline void *alloc(size_t size)
{
  struct _vtable **ppvt= (struct _vtable **)calloc(1, sizeof(struct _vtable *) + size);
  return (void *)(ppvt + 1);
}

struct _object *symbol_new(const wchar_t *string)
{
  struct symbol *symbol = (struct symbol *)alloc(sizeof(struct symbol));
  symbol->_vt[-1] = symbol_vt;
  size_t length = wcslen(string);
  size_t buffer_size = sizeof(wchar_t) * (length + 1);
  symbol->string = (wchar_t*)malloc(buffer_size);
  memset(symbol->string, 0, buffer_size);
  /*symbol->string =*/ wcsncpy(symbol->string, string, length); // TODO - pointer from int without cast WTF!?
  return (struct _object *)symbol;
}

struct _object *closure_new(imp_t method, struct _object *data)
{
  struct closure *closure = (struct closure *)alloc(sizeof(struct closure));
  closure->_vt[-1] = closure_vt;
  closure->method  = method;
  closure->data    = data;
  return (struct _object *)closure;
}

//struct _object *vtable_lookup(struct closure *closure, struct _vtable *self, struct _object *key);
/*
#if ICACHE
# define send(RCV, MSG, ARGS...) ({				\
      struct        object   *r = (struct _object *)(RCV);	\
      static struct _vtable   *prevVT  = 0;			\
      static struct closure  *closure = 0;			\
      register struct _vtable *thisVT  = r->_vt[-1];		\
      thisVT == prevVT						\
	?  closure						\
	: (prevVT  = thisVT,					\
	   closure = bind(r, (MSG)));				\
      closure->method(closure, r, ##ARGS);			\
    })
#else
# define send(RCV, MSG, ARGS...) ({				\
      struct _object  *r = (struct _object *)(RCV);		\
      struct closure *c = bind(r, (MSG));			\
      c->method(c, r, ##ARGS);					\
    })
#endif
*/

#if MCACHE
struct entry {
  struct _vtable  *vtable;
  struct _object *selector;
  struct closure *closure;
} MethodCache[8192];
#endif


struct closure *bind(struct _object *rcv, struct _object *msg)
{
  struct closure *c;
  struct _vtable  *vt = rcv->_vt[-1];
#if MCACHE
  struct entry   *cl = MethodCache + ((((unsigned)vt << 2) ^ ((unsigned)msg >> 3)) & ((sizeof(MethodCache) / sizeof(struct entry)) - 1));
  if (cl->vtable == vt && cl->selector == msg)
    return cl->closure;
#endif
  c = ((msg == s_lookup) && (rcv == (struct _object *)vtable_vt))
    ? (struct closure *)vtable_lookup(0, vt, msg)
    : (struct closure *)send(vt, s_lookup, msg);
#if MCACHE
  cl->vtable   = vt;
  cl->selector = msg;
  cl->closure  = c;
#endif
  return c;
}

struct _vtable *vtable_delegated(struct closure *closure, struct _vtable *self)
{
  struct _vtable *child= (struct _vtable *)alloc(sizeof(struct _vtable));
  child->_vt[-1] = self ? self->_vt[-1] : 0;
  child->size    = 2;
  child->tally   = 0;
  child->keys    = (struct _object **)calloc(child->size, sizeof(struct _object *));
  child->values  = (struct _object **)calloc(child->size, sizeof(struct _object *));
  child->parent  = self;
  return child;
}

struct _object *vtable_allocate(struct closure *closure, struct _vtable *self, int payloadSize)
{
  struct _object *obj = (struct _object *)alloc(payloadSize);
  obj->_vt[-1] = self;
  return obj;
}

imp_t vtable_addMethod(struct closure *closure, struct _vtable *self, struct _object *key, imp_t method)
{
  int i;
  for (i = 0;  i < self->tally;  ++i)
    if (key == self->keys[i])
      return ((struct closure *)self->values[i])->method = method;
  if (self->tally == self->size) {
    self->size  *= 2;
    self->keys   = (struct _object **)realloc(self->keys,   sizeof(struct _object *) * self->size);
    self->values = (struct _object **)realloc(self->values, sizeof(struct _object *) * self->size);
  }
  self->keys  [self->tally  ] = key;
  self->values[self->tally++] = closure_new(method, 0);
  return method;
}

struct _object *vtable_lookup(struct closure *closure, struct _vtable *self, struct _object *key)
{
  int i;
  for (i = 0;  i < self->tally;  ++i)
    if (key == self->keys[i])
      return self->values[i];
  if (self->parent)
    return send(self->parent, s_lookup, key);
  fwprintf(stderr, L"lookup failed %p %S\n", self, ((struct symbol *)key)->string);
  return 0;
}


struct _object* symbol_lookup(struct closure* closure, struct _object* self, const wchar_t *string)
{
  struct _object* symbol;
  struct _vtable* vt = self ? self->_vt[-1] : SymbolList;
  int i;
  for (i = 0;  i < vt->tally;  ++i) {
    symbol = vt->keys[i];
    /*wprintf(L"COMPARE: '%S'(%d) ? '%S'(%d)  =  %d\n", 
            string, 
            wcslen(string),
            ((struct symbol*)symbol)->string, 
            wcslen(((struct symbol*)symbol)->string),
            wcscmp(string, ((struct symbol*)symbol)->string));*/
    if (wcscmp(string, ((struct symbol*)symbol)->string) == 0) {
      //printf("FOUND IT!\n");
      return symbol;
    }
  }
  return 0;
}


struct _object *symbol_intern(struct closure *closure, struct _object *self, const wchar_t *string)
{
  struct _object *symbol;
  symbol = symbol_lookup(closure, self, string);
  if (!symbol) {
    struct _vtable* vt = self ? self->_vt[-1] : SymbolList;
    symbol = symbol_new(string);
    vtable_addMethod(0, vt, symbol, 0);
  }
  return symbol;
}


void obj_init(void)
{
  vtable_vt = vtable_delegated(0, 0);
  vtable_vt->_vt[-1] = vtable_vt;

  object_vt = vtable_delegated(0, 0);
  object_vt->_vt[-1] = vtable_vt;
  vtable_vt->parent = object_vt;

  symbol_vt  = vtable_delegated(0, object_vt);
  closure_vt = vtable_delegated(0, object_vt);

  SymbolList = vtable_delegated(0, 0);

  s_lookup    = symbol_intern(0, 0, L"lookup");
  s_addMethod = symbol_intern(0, 0, L"addMethod");
  s_allocate  = symbol_intern(0, 0, L"allocate");
  s_delegated = symbol_intern(0, 0, L"delegated");

  vtable_addMethod(0, vtable_vt, s_lookup,    (imp_t)vtable_lookup);
  vtable_addMethod(0, vtable_vt, s_addMethod, (imp_t)vtable_addMethod);

  send(vtable_vt, s_addMethod, s_allocate,    vtable_allocate);
  send(vtable_vt, s_addMethod, s_delegated,   vtable_delegated);
}