/**
 * fexpression.c: Tiny Functional Expression Library built from P&W Objects
 * Copyright (C) 2010 antoine@7degrees.co.za
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the 'Software'),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that both the
 * above copyright notice(s) and this permission notice appear in supporting
 * documentation.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS'.  USE ENTIRELY AT YOUR OWN RISK.
 */

#ifndef FEXP_H
#define FEXP_H

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>

#include <object.h>
typedef struct _vtable  vtable;
typedef struct _object  object;
typedef struct _closure closure;
typedef struct _symbol  symbol;

typedef unsigned char byte;

/* - initialization ----------------------------------------------------- */
void fexp_init();


/* - global selectors --------------------------------------------------- */
extern object* s_type;           
// extern object* s_selectors;      TODO  list of all methods that can operate on this 'type'
extern object* s_new;
extern object* s_length;
extern object* s_print;
extern object* s_tojson;

/* - string ------------------------------------------------------------- */
typedef struct _string {
  vtable* _vt[0];
  size_t   length;
  wchar_t* buffer;
} string;
extern vtable* string_vt;
extern object* String;
extern object* s_string_fromwchar;
extern object* s_string_fromchar;
extern object* s_string_tochar;
extern object* s_string_add;
extern object* s_string_split;

object* string_new   (closure* c, string* self, const wchar_t* s, size_t length);
size_t  string_length(closure* c, string* self);
string* string_print (closure* c, string* self);
string* string_tojson(closure* c, string* self, bool quoted);

object* string_fromwchar(closure* c, string* self, const wchar_t* format, ...);
object* string_fromchar (closure* c, string* self, const char* s, size_t length);
char*   string_tochar   (closure* c, string* self);
string* string_add      (closure* c, string* self, const string* s);
struct _fexp* string_split(closure* c, string* self, const string* delimiter);

/* - number ------------------------------------------------------------- */
// TODO - implementation - surprising how far one can get without numbers these days :)
typedef struct _number {
  vtable* _vt[0];
  int integer;
} number;
extern vtable* number_vt;
extern object* Number;

object* number_new   (closure* c, number* self, const wchar_t* s, size_t length);
number* number_print (closure* c, number* self);
string* number_tojson(closure* c, number* self, bool quoted);


/* - fexp --------------------------------------------------------------- */
typedef struct _fexp {
  vtable* _vt[0];
  object* car;
  object* cdr; 
} fexp;
extern vtable* fexp_vt;
extern object* Fexp;
extern object* s_fexp_car;
extern object* s_fexp_cdr;
extern object* s_fexp_cons;
extern object* s_fexp_last;
extern object* s_fexp_nth;
extern object* s_fexp_nthcdr;
extern object* s_fexp_join;
extern fexp*   fexp_environment;
extern fexp*   fexp_nil;
extern object* fexp_t;

object* fexp_new   (closure* c, fexp* self, object* car, object* cdr);
size_t  fexp_length(closure* c, fexp* self);
fexp*   fexp_print (closure* c, fexp* self);
string* fexp_tojson(closure* c, fexp* self, bool quoted);

object* fexp_car   (closure* c, fexp* self);
object* fexp_cdr   (closure* c, fexp* self);
fexp*   fexp_cons  (closure* c, fexp* self, object* car);
fexp*   fexp_last  (closure* c, fexp* self);
object* fexp_nth   (closure* c, fexp* self, int n);
fexp*   fexp_nthcdr(closure* c, fexp* self, int n);
string* fexp_join  (closure* c, fexp* self, const string* delimiter);



/* - tconc -------------------------------------------------------------- */
typedef struct _tconc {
  vtable* _vt[0];
  fexp* head;
  fexp* tail;
} tconc;
extern vtable* tconc_vt;
extern object* Tconc;
extern object* s_tconc_tconc;
extern object* s_tconc_append;

tconc* tconc_new   (closure* c, tconc* self, fexp* list);
size_t tconc_length(closure* c, tconc* self);
tconc* tconc_print (closure* c, tconc* self);

fexp*  tconc_tconc (closure* c, tconc* self);
tconc* tconc_append(closure* c, tconc* self, object* item);


/* - obj extensions ----------------------------------------------------- */
vtable* object_type (closure* c,  object* self);
symbol* symbol_print(closure* c,  symbol* self);
string* symbol_tojson(closure* c, symbol* self, bool quoted);

#endif /* FEXP_H */
