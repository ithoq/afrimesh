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
object *s_type   = 0;
object *s_new    = 0;
object *s_length = 0;
object *s_print  = 0;
object *s_tojson = 0;


/* - library initialization --------------------------------------------- */
void fexp_init()
{
  // global
  s_type   = symbol_intern(0, 0, L"type");
  s_new    = symbol_intern(0, 0, L"new");
  s_length = symbol_intern(0, 0, L"length");
  s_print  = symbol_intern(0, 0, L"print");
  s_tojson = symbol_intern(0, 0, L"tojson");

  // extend base object model
  send(object_vt, s_addMethod, s_type,   object_type);
  send(symbol_vt, s_addMethod, s_print,  symbol_print);
  send(symbol_vt, s_addMethod, s_tojson, symbol_tojson);

  // string
  s_string_fromwchar = symbol_intern(0, 0, L"fromwchar");
  s_string_fromchar  = symbol_intern(0, 0, L"fromchar");
  s_string_tochar    = symbol_intern(0, 0, L"tochar");
  s_string_add       = symbol_intern(0, 0, L"add");
  s_string_split     = symbol_intern(0, 0, L"split");
  string_vt = (vtable*)send(object_vt, s_delegated);
  send(string_vt, s_addMethod, s_new,    string_new);
  send(string_vt, s_addMethod, s_length, string_length);
  send(string_vt, s_addMethod, s_print,  string_print);
  send(string_vt, s_addMethod, s_tojson, string_tojson);
  send(string_vt, s_addMethod, s_string_fromwchar, string_fromwchar);
  send(string_vt, s_addMethod, s_string_fromchar,  string_fromchar);
  send(string_vt, s_addMethod, s_string_tochar,    string_tochar);
  send(string_vt, s_addMethod, s_string_add,       string_add);
  send(string_vt, s_addMethod, s_string_split,     string_split);
  String = send(string_vt, s_allocate, 0);

  // fexp
  s_fexp_car    = symbol_intern(0, 0, L"car");
  s_fexp_cdr    = symbol_intern(0, 0, L"cdr");
  s_fexp_cons   = symbol_intern(0, 0, L"cons");
  s_fexp_last   = symbol_intern(0, 0, L"last");
  s_fexp_nth    = symbol_intern(0, 0, L"nth");
  s_fexp_nthcdr = symbol_intern(0, 0, L"nthcdr");
  s_fexp_join   = symbol_intern(0, 0, L"join");
  fexp_vt = (vtable*)send(object_vt, s_delegated);
  send(fexp_vt, s_addMethod, s_new,    fexp_new);
  send(fexp_vt, s_addMethod, s_length, fexp_length);
  send(fexp_vt, s_addMethod, s_print,  fexp_print);
  send(fexp_vt, s_addMethod, s_tojson, fexp_tojson);
  send(fexp_vt, s_addMethod, s_fexp_car,    fexp_car);
  send(fexp_vt, s_addMethod, s_fexp_cdr,    fexp_cdr);
  send(fexp_vt, s_addMethod, s_fexp_cons,   fexp_cons);
  send(fexp_vt, s_addMethod, s_fexp_last,   fexp_last);
  send(fexp_vt, s_addMethod, s_fexp_nth,    fexp_nth);
  send(fexp_vt, s_addMethod, s_fexp_nthcdr, fexp_nthcdr);
  send(fexp_vt, s_addMethod, s_fexp_join,   fexp_join);
  Fexp = send(fexp_vt, s_allocate, 0);
  //string* nil = (string*)send(String, s_new, L"NIL", 3)
  object* nil = send(String, s_new, L"NIL", 3);
  fexp_nil = (fexp*)send(Fexp, s_new, nil, fexp_nil);
  fexp_nil->cdr = (object*)fexp_nil; // need to set manually because fexp_nil is undefined at construction time
  //fexp_nil = (fexp*)send(Fexp, s_fexp_cons, nil, fexp_nil);
  //fexp_environment = (fexp*)send(fexp_nil, s_fexp_cons, fexp_nil); // TODO ???

  // tconc
  s_tconc_tconc  = symbol_intern(0, 0, L"tconc");
  s_tconc_append = symbol_intern(0, 0, L"append");
  tconc_vt = (vtable*)send(object_vt, s_delegated);
  send(tconc_vt, s_addMethod, s_new,    tconc_new);
  send(tconc_vt, s_addMethod, s_length, tconc_length);
  send(tconc_vt, s_addMethod, s_print,  tconc_print);
  send(tconc_vt, s_addMethod, s_tconc_tconc,  tconc_tconc);
  send(tconc_vt, s_addMethod, s_tconc_append, tconc_append);
  Tconc = send(tconc_vt, s_allocate, 0);
}


/* - obj extensions ----------------------------------------------------- */
vtable* object_type(closure* c, object* self)
{
  return self->_vt[-1];
}


symbol* symbol_print(closure* c, symbol* self)
{
  wprintf(L"#%ls", self->string);
  return self;
}

string* symbol_tojson(closure* c, symbol* self, bool quoted)
{
  /*if (quoted) {
    return self;
  } */
  return (string*)send(String, s_string_fromwchar, L"\"#%S\"", self->string);
}


/* - string ------------------------------------------------------------- */
vtable* string_vt = 0;
object* String = 0;
object *s_string_fromwchar = 0;
object *s_string_fromchar  = 0;
object *s_string_tochar    = 0;
object *s_string_add       = 0;
object *s_string_split     = 0;

object* string_new(closure* c, string* self, const wchar_t* s, size_t length) {
  if (length > wcslen(s)) return (object*)fexp_nil; 
  string* clone = (string*)send(self->_vt[-1], s_allocate, sizeof(string));
  clone->length = length;
  if (length != 0) {
    size_t buffer_size = sizeof(wchar_t) * (length + 1);
    clone->buffer = (wchar_t*)malloc(buffer_size);
    memset(clone->buffer, 0, buffer_size);
    clone->buffer = wcsncpy(clone->buffer, s, length);
    clone->buffer[length] = L'\0';
  } else {
    clone->buffer = NULL;
  }
  return (object*)clone;
}

size_t string_length(closure* c, string *self)
{
  return self->length;
}

string* string_print(closure* c, string *self)
{
  wprintf(L"\"%ls\"", self->buffer ? self->buffer : L"null.string");
  return self;
}

string* string_tojson(closure* c, string* self, bool quoted)
{
  if (quoted) {
    return self;
  } 
  return (string*)send(String, s_string_fromwchar, L"\"%S\"", self->buffer);
}

object* string_fromwchar(closure* c, string* self, const wchar_t* format, ...)
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
object* string_fromchar(closure* c, string *self, const char* s, size_t length) {
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

char* string_tochar(closure* c, string* self)
{
  size_t buffer_size = sizeof(char) + (self->length + 1);
  char* buffer = (char*)malloc(buffer_size);
  memset(buffer, 0, buffer_size);
  if (wcstombs(buffer, self->buffer, self->length) != self->length) {
    wprintf(L"conversion failed: %S\n", self->buffer);
  }
  buffer[self->length] = '\0';
  return buffer;
}

string* string_add(closure* c, string *self, const string *s)
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

fexp* string_split(closure* c, string* self, const string* delimiter)
{
  tconc* ret = (tconc*)send(Tconc, s_new, fexp_nil);
  wchar_t* iter = self->buffer;
  wchar_t* next;
  while (true) {
    next = wcsstr(iter, delimiter->buffer);
    if (!next || next == iter) {
      object* s = send(String, s_new, iter, wcslen(iter));
      ret = (tconc*)send(ret, s_tconc_append, s);
      break;
    } 
    object* s = send(String, s_new, iter, next - iter);
    ret = (tconc*)send(ret, s_tconc_append, s);
    iter = next + 1;
  }
  return (fexp*)send(ret, s_tconc_tconc);
}


/* - fexp --------------------------------------------------------------- */
vtable* fexp_vt = 0;
object* Fexp = 0;
object* s_fexp_car       = 0;
object* s_fexp_cdr       = 0;
object* s_fexp_cons      = 0;
object* s_fexp_last      = 0;
object* s_fexp_nth       = 0;
object* s_fexp_nthcdr    = 0;
object* s_fexp_join      = 0;
fexp*   fexp_environment = 0;
fexp*   fexp_nil         = 0;
object* fexp_t           = 0;

object* fexp_new(closure* c, fexp* self, object* car, object* cdr)
{
  fexp *clone = (fexp*)send(self->_vt[-1], s_allocate, sizeof(fexp));
  clone->car = car;
  clone->cdr = cdr;
  return (object*)clone;
}

size_t fexp_length(closure* c, fexp *self)
{
  return ((fexp*)self->cdr != fexp_nil) ? 1 + (size_t)send(self->cdr, s_length) : 1;
}

fexp* fexp_print(closure* c, fexp* self)
{
  wprintf(L"(");
  object* iter; 
  for (iter = (object*)self; iter != (object*)fexp_nil; iter = send(iter, s_fexp_cdr)) {
    wprintf(L" ");
    if ((vtable*)send(iter, s_type) != fexp_vt) { // it's a cons pair
      wprintf(L". ");
      send(iter, s_print);
      break;
    }
    object* car = send(iter, s_fexp_car);
    send(car, s_print);
  }
  wprintf(L" )");
  return self;
}

string* fexp_tojson(closure* c, fexp* self, bool quoted)
{
  string* json = (string*)send(String, s_string_fromwchar, L"[ ");
  object* iter;
  bool first = true;
  for (iter = (object*)self; iter != (object*)fexp_nil; iter = send(iter, s_fexp_cdr)) {
    if ((vtable*)send(iter, s_type) != fexp_vt) { // it's a cons pair
      // TODO - need an elegant way that does not require that everyone implement a s_tojson
      //        best would probably be to change s_print to s_tostring and stick s_print 
      //        into object
      break;
    } 
    object* car = send(iter, s_fexp_car);
    string* json_car = (string*)send(car, s_tojson, quoted);
    if (!first) {
      json_car = (string*)send(String, s_string_fromwchar, L", %S", json_car->buffer);
    }
    json = (string*)send(json, s_string_add, json_car);
    first = false;
  }  
  json = (string*)send(json, s_string_fromwchar, L"%S ]", json->buffer);
  return json;
}

object* fexp_car(closure* c, fexp* self)
{
  return self->car;
}

object* fexp_cdr(closure* c, fexp* self)
{
  return self->cdr;
}

fexp* fexp_cons(closure* c, fexp* self, object* car)
{
  fexp *x = (fexp*)send(Fexp, s_new, car, self);
  return x;
}

fexp* fexp_last(closure* c, fexp* self)
{
  return ((fexp*)self->cdr == fexp_nil) ? self : (fexp*)send(self->cdr, s_fexp_last);
}

object* fexp_nth (closure* c, fexp* self, int n)
{
  if (self == fexp_nil) {
    return (object*)fexp_nil;
  }
  if (n == 0) {
    return self->car;
  }
  return send(self->cdr, s_fexp_nth, n - 1);
}

fexp* fexp_nthcdr(closure* c, fexp* self, int n)
{
  if (self == fexp_nil) {
    return fexp_nil;
  }
  if (n == 0) {
    return self;
  }
  return (fexp*)send(self->cdr, s_fexp_nthcdr, n - 1);
}

string* fexp_join(closure* c, fexp* self, const string* delimiter)
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
vtable* tconc_vt = 0;
object* Tconc = 0;
object *s_tconc_tconc  = 0;
object *s_tconc_append = 0;

tconc* tconc_new(closure* c, tconc* self, fexp* list)
{
  tconc* clone = (tconc*)send(self->_vt[-1], s_allocate, sizeof(tconc));
  clone->head = list;
  clone->tail = (fexp*)send(list, s_fexp_last);
  return clone;
}

size_t tconc_length(closure* c, tconc* self)
{
  fexp* list = self->head;
  return ((fexp*)list->cdr != fexp_nil) ? 1 + (size_t)send(list->cdr, s_length) : 0;
}

tconc* tconc_print(closure* c, tconc* self)
{
  fexp* list = self->head;
  send(list, s_print);
  return self;
}

fexp* tconc_tconc(closure* c, tconc* self)
{
  return self->head;
}

tconc* tconc_append(closure* c, tconc* self, object* item)
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

