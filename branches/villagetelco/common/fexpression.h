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
typedef struct _object object;


/* - initialization ----------------------------------------------------- */
void fexp_init();


/* - global selectors --------------------------------------------------- */
// extern object* s_type;           TODO  a symbol representing the 'type'
// extern object* s_selectors;      TODO  list of all methods that can operate on this 'type'
extern object* s_new;
extern object* s_length;
extern object* s_print;
extern object* s_tojson;

/* - string ------------------------------------------------------------- */
typedef struct _string {
  struct vtable* _vt[0];
  size_t   length;
  wchar_t* buffer;
} string;
extern struct vtable* string_vt;
extern object* String;
extern object* s_string_fromwchar;
extern object* s_string_fromchar;
extern object* s_string_tochar;
extern object* s_string_add;

object* string_fromwchar(struct closure* closure, string* self, const wchar_t* format, ...);
object* string_fromchar (struct closure* closure, string* self, const char* s, size_t length);
char*   string_tochar   (struct closure* closure, string* self);
string* string_add      (struct closure* closure, string* self, const string* s);

object* string_new   (struct closure* closure, string* self, const wchar_t* s, size_t length);
size_t  string_length(struct closure* closure, string* self);
string* string_print (struct closure* closure, string* self);
string* string_tojson(struct closure* closure, string* self, bool quoted);


/* - number ------------------------------------------------------------- */
// TODO - implementation - surprising how far one can get without numbers these days :)
typedef struct _number {
  struct vtable* _vt[0];
  int integer;
} number;
extern struct vtable* number_vt;
extern object* Number;

object* number_new   (struct closure* closure, number* self, const wchar_t* s, size_t length);
number* number_print (struct closure* closure, number* self);
string* number_tojson(struct closure* closure, number* self, bool quoted);


/* - fexp --------------------------------------------------------------- */
typedef struct _fexp {
  struct vtable* _vt[0];
  object* car;
  object* cdr; 
} fexp;
extern struct vtable* fexp_vt;
extern object* Fexp;
extern object* s_fexp_car;
extern object* s_fexp_cdr;
extern object* s_fexp_cons;
extern object* s_fexp_last;
extern object* s_fexp_join;
extern fexp*   fexp_environment;
extern fexp*   fexp_nil;
extern object* fexp_t;

object* fexp_car (struct closure* closure, fexp* self);
object* fexp_cdr (struct closure* closure, fexp* self);
fexp*   fexp_cons(struct closure* closure, fexp* self, object* car);
fexp*   fexp_last(struct closure* closure, fexp* self);
string* fexp_join(struct closure* closure, fexp* self, const string* delimiter);

object* fexp_new   (struct closure* closure, fexp* self, object* car, object* cdr);
size_t  fexp_length(struct closure* closure, fexp* self);
fexp*   fexp_print (struct closure* closure, fexp* self);
string* fexp_tojson(struct closure* closure, fexp* self, bool quoted);


/* - tconc -------------------------------------------------------------- */
typedef struct _tconc {
  struct vtable* _vt[0];
  fexp* head;
  fexp* tail;
} tconc;
extern struct vtable* tconc_vt;
extern object* Tconc;
extern object* s_tconc_tconc;
extern object* s_tconc_append;
tconc* tconc_new   (struct closure* closure, tconc* self, fexp* list);
size_t tconc_length(struct closure* closure, tconc* self);
tconc* tconc_print (struct closure* closure, tconc* self);

fexp*  tconc_tconc (struct closure* closure, tconc* self);
tconc* tconc_append(struct closure* closure, tconc* self, object* item);


/* - obj extensions ----------------------------------------------------- */
struct symbol* symbol_print (struct closure* closure, struct symbol* self);


#endif /* FEXP_H */
