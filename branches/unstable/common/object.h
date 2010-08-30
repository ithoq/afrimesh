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


#ifndef OBJ_H
#define OBJ_H

#define ICACHE 1	/* nonzero to enable point-of-send inline cache */
#define MCACHE 1	/* nonzero to enable global method cache        */

struct _vtable;
struct _object;
struct _closure;
struct _symbol;

typedef struct _object *(*imp_t)(struct _closure *closure, struct _object *receiver, ...);

struct _vtable
{
  struct _vtable  *_vt[0];
  int              size;
  int              tally;
  struct _object **keys;
  struct _object **values;
  struct _vtable  *parent;
};

struct _object {
  struct _vtable *_vt[0];
};

struct _closure
{
  struct _vtable *_vt[0];
  imp_t	 method;
  struct _object *data;
};

struct _symbol
{
  struct _vtable *_vt[0];
  wchar_t       *string;
};

extern struct _vtable *_vtable_vt;
extern struct _vtable *object_vt;
extern struct _vtable *symbol_vt;
extern struct _vtable *closure_vt;

/* - global selectors --------------------------------------------------- */
extern struct _object *s_addMethod;
extern struct _object *s_allocate;
extern struct _object *s_delegated;
extern struct _object *s_lookup;


/* - implementation ----------------------------------------------------- */
void *alloc(size_t size);
struct _object *symbol_new(const wchar_t *string);
struct _object *closure_new(imp_t method, struct _object *data);
struct _object *vtable_lookup(struct _closure *closure, struct _vtable *self, struct _object *key);
struct _closure *_bind(struct _object *rcv, struct _object *msg);  /* _bind conflicts with socket.h */
struct _vtable *vtable_delegated(struct _closure *closure, struct _vtable *self);
struct _object *vtable_allocate(struct _closure *closure, struct _vtable *self, int payloadSize);
struct _object *vtable_lookup(struct _closure *closure, struct _vtable *self, struct _object *key);
struct _object *symbol_intern(struct _closure *closure, struct _object *self, const wchar_t *string);
struct _object* symbol_lookup(struct _closure* closure, struct _object* self, const wchar_t *string);
void obj_init(void);


/* - cache macros ------------------------------------------------------ */
struct _closure *_bind(struct _object *rcv, struct _object *msg);
#if ICACHE
# define send(RCV, MSG, ARGS...) ({                       \
      struct        _object   *r = (struct _object *)(RCV);	\
      static struct _vtable   *prevVT  = 0;                \
      static struct _closure   *closure = 0;                \
      register struct _vtable *thisVT  = r->_vt[-1];       \
      thisVT == prevVT                                    \
        ?  closure                                        \
        : (prevVT  = thisVT,                              \
           closure = _bind(r, (MSG)));                     \
      closure->method(closure, r, ##ARGS);                \
    })
#else
# define send(RCV, MSG, ARGS...) ({                 \
      struct _object  *r = (struct _object *)(RCV);		\
      struct _closure *c = _bind(r, (MSG));           \
      c->method(c, r, ##ARGS);                      \
    })
#endif

#if MCACHE
struct entry {
  struct _vtable  *vtable;
  struct _object *selector;
  struct _closure *closure;
} MethodCache[8192];
#endif


#endif /* OBJ_H */
