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


#include <fexpression.h>

/* - global selectors --------------------------------------------------- */
object *s_new    = 0;
object *s_length = 0;
object *s_print  = 0;


/* - library initialization --------------------------------------------- */
void fexp_init()
{
  // global
  s_new    = symbol_intern(0, 0, L"new");
  s_length = symbol_intern(0, 0, L"length");
  s_print  = symbol_intern(0, 0, L"print");

  // extend base object model
  send(symbol_vt, s_addMethod, s_print, symbol_print);

  // string
  s_string_fromwchar = symbol_intern(0, 0, L"fromwchar");
  s_string_fromchar  = symbol_intern(0, 0, L"fromchar");
  s_string_tochar    = symbol_intern(0, 0, L"tochar");
  s_string_add       = symbol_intern(0, 0, L"add");
  string_vt = (struct vtable*)send(object_vt, s_delegated);
  send(string_vt, s_addMethod, s_new,    string_new);
  send(string_vt, s_addMethod, s_length, string_length);
  send(string_vt, s_addMethod, s_print,  string_print);
  send(string_vt, s_addMethod, s_string_fromwchar, string_fromwchar);
  send(string_vt, s_addMethod, s_string_fromchar,  string_fromchar);
  send(string_vt, s_addMethod, s_string_tochar,    string_tochar);
  send(string_vt, s_addMethod, s_string_add,       string_add);
  String = send(string_vt, s_allocate, 0);

  // fexp
  s_fexp_car  = symbol_intern(0, 0, L"car");
  s_fexp_cdr  = symbol_intern(0, 0, L"cdr");
  s_fexp_cons = symbol_intern(0, 0, L"cons");
  s_fexp_last = symbol_intern(0, 0, L"last");
  s_fexp_join = symbol_intern(0, 0, L"join");
  fexp_vt = (struct vtable*)send(object_vt, s_delegated);
  send(fexp_vt, s_addMethod, s_new,    fexp_new);
  send(fexp_vt, s_addMethod, s_length, fexp_length);
  send(fexp_vt, s_addMethod, s_print,  fexp_print);
  send(fexp_vt, s_addMethod, s_fexp_car,  fexp_car);
  send(fexp_vt, s_addMethod, s_fexp_cdr,  fexp_cdr);
  send(fexp_vt, s_addMethod, s_fexp_cons, fexp_cons);
  send(fexp_vt, s_addMethod, s_fexp_last, fexp_last);
  send(fexp_vt, s_addMethod, s_fexp_join, fexp_join);
  Fexp = send(fexp_vt, s_allocate, 0);
  //string* nil = (string*)send(String, s_new, L"NIL", 3)
  object* nil = send(String, s_new, L"NIL", 3);
  fexp_nil = (fexp*)send(Fexp, s_new, nil, fexp_nil);
  fexp_nil->cdr = (object*)fexp_nil; // need to set manually because fexp_nil is undefined at construction time
  //fexp_nil = (fexp*)send(Fexp, s_fexp_cons, nil, fexp_nil);
  fexp_environment = (fexp*)send(fexp_nil, s_fexp_cons, fexp_nil); // TODO ???

  // tconc
  s_tconc_tconc  = symbol_intern(0, 0, L"tconc");
  s_tconc_append = symbol_intern(0, 0, L"append");
  tconc_vt = (struct vtable*)send(object_vt, s_delegated);
  send(tconc_vt, s_addMethod, s_new,    tconc_new);
  send(tconc_vt, s_addMethod, s_length, tconc_length);
  send(tconc_vt, s_addMethod, s_print,  tconc_print);
  send(tconc_vt, s_addMethod, s_tconc_tconc,  tconc_tconc);
  send(tconc_vt, s_addMethod, s_tconc_append, tconc_append);
  Tconc = send(tconc_vt, s_allocate, 0);
}


/* - obj extensions ----------------------------------------------------- */
struct symbol* symbol_print(struct closure* closure, struct symbol* self)
{
  wprintf(L"#%ls", self->string);
  return self;
}


/* - string ------------------------------------------------------------- */
struct vtable* string_vt = 0;
object* String = 0;
object *s_string_fromwchar = 0;
object *s_string_fromchar  = 0;
object *s_string_tochar    = 0;
object *s_string_add       = 0;

object* string_new(struct closure* closure, string* self, const wchar_t* s, size_t length) {
  if (length > wcslen(s)) return (object*)fexp_nil; 
  string* clone = (string*)send(self->_vt[-1], s_allocate, sizeof(string));
  clone->length = length;
  if (length != 0) {
    size_t buffer_size = sizeof(wchar_t) * (length+1);
    clone->buffer = (wchar_t*)malloc(buffer_size);
    memset(clone->buffer, 0, buffer_size);
    clone->buffer = wcsncpy(clone->buffer, s, length);
    clone->buffer[length] = L'\0';
  } else {
    clone->buffer = NULL;
  }
  return (object*)clone;
}


object* string_fromwchar(struct closure* closure, string* self, const wchar_t* format, ...)
{
  wchar_t* buffer;
  int length;
  va_list args;
  va_start(args, format);
  if ((length = vaswprintf(&buffer, format, args)) == -1 || buffer == NULL) {
    return (object*)fexp_nil;
  }
  va_end(args);
  string *clone = (string*)send(self->_vt[-1], s_allocate, sizeof(string));
  clone->length = length;
  clone->buffer = buffer;
  return (object*)clone;
}

// TODO - make it take a format string as above and onstruct using vasprintf
object* string_fromchar(struct closure *closure, string *self, const char* s, size_t length) {
  /*  va_list args;
  va_start(args, query);
  char* final_query;
  if ((vasprintf(&final_query, query, args) == -1) || final_query == NULL) {
      printf("\t{\n\t\terror : \"Could not build query '%s'\"\n\t}\n", query);
      return -1;    
  }
  va_end(args);  */
  if (length > strlen(s)) return (object*)fexp_nil; 
  string *clone = (string*)send(self->_vt[-1], s_allocate, sizeof(string));
  clone->length = length;
  size_t buffer_size = sizeof(wchar_t) * (length+1);
  clone->buffer = (wchar_t*)malloc(buffer_size);
  memset(clone->buffer, 0, buffer_size);
  if (mbstowcs(clone->buffer, s, length) != length) {
    return (object*)fexp_nil; 
  }
  clone->buffer[length] = L'\0';
  return (object*)clone;
}



char* string_tochar(struct closure* closure, string* self)
{
  size_t buffer_size = sizeof(char) + (self->length+1);
  char* buffer = (char*)malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  if (wcstombs(buffer, self->buffer, self->length) != self->length) {
    printf("conversion failed: %s\n", buffer);
  }
  buffer[self->length] = '\0';
  return buffer;
}

string* string_add(struct closure *closure, string *self, const string *s)
{
  size_t   length = self->length + s->length;
  size_t   buffer_size = sizeof(wchar_t) * (length+1);
  wchar_t* buffer = (wchar_t*)malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  if (self->length == 0) {
    wcscpy(buffer, s->buffer);
  } else {
    wcscpy(buffer, self->buffer);
    wcscat(buffer, s->buffer);
  }
  buffer[length] = L'\0';
  if (self->buffer) {
    free(self->buffer);
  }
  self->buffer = buffer;
  self->length = length;
  return self;
}

size_t string_length(struct closure *closure, string *self)
{
  return self->length;
}

string* string_print(struct closure *closure, string *self)
{
  wprintf(L"\"%ls\"", self->buffer ? self->buffer : L"null.string");
  return self;
}


/* - fexp --------------------------------------------------------------- */
struct vtable* fexp_vt = 0;
object* Fexp = 0;
object* s_fexp_car    = 0;
object* s_fexp_cdr    = 0;
object* s_fexp_cons   = 0;
object* s_fexp_last   = 0;
object* s_fexp_join   = 0;
fexp*   fexp_environment = 0;
fexp*   fexp_nil         = 0;
object* fexp_t           = 0;

object* fexp_new(struct closure* closure, fexp* self, object* car, object* cdr)
{
  fexp *clone = (fexp*)send(self->_vt[-1], s_allocate, sizeof(fexp));
  clone->car = car;
  clone->cdr = cdr;
  return (object*)clone;
}

size_t fexp_length(struct closure *closure, fexp *self)
{
  return ((fexp*)self->cdr != fexp_nil) ? 1 + (size_t)send(self->cdr, s_length) : 0;
}

fexp* fexp_print(struct closure* closure, fexp* self)
{
  printf("(");
  object* iter; 
  for (iter = (object*)self; iter != (object*)fexp_nil; iter = send(iter, s_fexp_cdr)) {
    printf(" ");
    if (iter->_vt[-1] != Fexp->_vt[-1]) { // Is not a Fexp - TODO is this the best way?
      printf(". ");
      send(iter, s_print);
      break;
    }
    object* car = send(iter, s_fexp_car);
    send(car, s_print);
  }
  printf(" )");
  return self;
}

object* fexp_car(struct closure *closure, fexp *self)
{
  return self->car;
}

object* fexp_cdr(struct closure *closure, fexp* self)
{
  return self->cdr;
}

fexp* fexp_cons(struct closure *closure, fexp *self, object* car)
{
  fexp *x = (fexp*)send(Fexp, s_new, car, self);
  return x;
}

fexp* fexp_last(struct closure *closure, fexp *self)
{
  //return (self->cdr == fexp_nil) ? self->car : fexp_last(closure, self->cdr);
  //printf("nil(%p) - self(%p) - self->cdr(%p)\n", fexp_nil, self, self->cdr);
  return ((fexp*)self->cdr == fexp_nil) ? self : (fexp*)send(self->cdr, s_fexp_last);
}

string* fexp_join(struct closure* closure, fexp* self, const string* delimiter)
{
  size_t length = (size_t)send(self, s_length);
  string* ret = (string*)send(String, s_new, L"", 0);
  fexp* iter;
  for (iter = self; iter != fexp_nil; iter = (fexp*)send(iter, s_fexp_cdr)) {
    if (ret->length) {
      ret = (string*)send(ret, s_string_add, delimiter);    
    } 
    string* car = (string*)send(iter, s_fexp_car);
    ret = (string*)send(ret, s_string_add, car);
  }
  return ret;
}


/* - tconc -------------------------------------------------------------- */
struct vtable* tconc_vt = 0;
object* Tconc = 0;
object *s_tconc_tconc  = 0;
object *s_tconc_append = 0;

tconc* tconc_new(struct closure* closure, tconc* self, fexp* list)
{
  tconc* clone = (tconc*)send(self->_vt[-1], s_allocate, sizeof(tconc));
  clone->head = list;
  clone->tail = (fexp*)send(list, s_fexp_last);
  return clone;
}

size_t tconc_length(struct closure* closure, tconc* self)
{
  fexp* list = self->head;
  return ((fexp*)list->cdr != fexp_nil) ? 1 + (size_t)send(list->cdr, s_length) : 0;
}

tconc* tconc_print(struct closure* closure, tconc* self)
{
  fexp* list = self->head;
  send(list, s_print);
  return self;
}

fexp* tconc_tconc(struct closure* closure, tconc* self)
{
  return self->head;
}

tconc* tconc_append(struct closure* closure, tconc* self, object* item)
{
  fexp* tail = (fexp*)send(fexp_nil, s_fexp_cons, item);
  if (self->head == fexp_nil) { /* we're the TCONC of the empty list */
    self->head = tail;
    self->tail = tail;
  } else {
    self->tail->cdr = (object*)tail;
    self->tail = tail;
  }
  return self;
}
