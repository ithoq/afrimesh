
#include <stdio.h>
#include <stdlib.h>
#include <fexpression.h>

int main(int argc, char** argv)
{
  /* initialization */
  obj_init();
  fexp_init();

  /* create some objects */
  object* s1 = send(String, s_string_fromwchar, L"Some String");
  send(s1, s_print);
  wprintf(L"\n");
  object* s2 = send(String, s_string_fromwchar, L"Some Other String");
  send(s2, s_print);
  wprintf(L"\n");
  object* f = (object*)fexp_nil;
  f = send(f, s_fexp_cons, s1);
  f = send(f, s_fexp_cons, s2);
  send(f, s_print);
  wprintf(L"\n");

  /* check their types */
  wprintf(L"s1(%p) string_vt(%p) String(%p)\n", send(s1, s_type), string_vt, send(String, s_type));
  wprintf(L"s2(%p) string_vt(%p) String(%p)\n", send(s2, s_type), string_vt, send(String, s_type));
  wprintf(L"f(%p) fexp_vt(%p) Fexp(%p)\n", send(f, s_type), fexp_vt, send(Fexp, s_type));

  return EXIT_SUCCESS;
}

