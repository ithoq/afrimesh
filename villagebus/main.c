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


#include "villagebus.h"


/**
 * Lifecycle state management
 */
int exit_failure(Request* request, const wchar_t* error) 
{
  request->out(request, error);
  cgi_release(request);
  log_release();
  return EXIT_FAILURE;
}

int exit_success(Request* request) 
{
  cgi_release(request);
  log_release();
  return EXIT_SUCCESS;
}


/**
 *
 */
int main(int argc, char** argv)
{

  /** - parse request --------------------------------------------------- */
  cgi_init();
  Request* request = cgi_request(argc, argv);
  wprintl(L"Parsed request : %p\n", request);
  if (request == NULL) {
    return exit_failure(request, L"{ \"error\" : \"Could not parse request\" }");
  } else if (request->method == SYMBOL_UNKNOWN) {
    return exit_failure(request, L"{ \"error\" : \"usage: villagebus [GET|PUT|POST] name\" }");
  } else if (request->pathname == NULL) {
    return exit_failure(request, L"{ \"error\" : \"name not found\" }");
  }
  wprintl(L"\tmethod : %d\n", request->method);
  wprintl(L"\thref   : %S\n", request->href);
  //wprintl(L"request->pathname : %S\n", request->pathname);
  //wprintl(L"request->search   : %S\n", request->search);
  //if (request->search) {
  //  wprintl(L"request->search   : %S\n", search_to_json(request->search, wcslen(request->search)));
  //}
  //wprintl(L"request->data     : %s\n", request->data);

  /** - bootstrap interpreter ------------------------------------------- */
  wprintl(L"Bootstrapping object\n");
  obj_init();
  wprintl(L"Bootstrapping fexpression\n");
  fexp_init();

  /** - bootstrap modules ----------------------------------------------- */
  wprintl(L"Bootstrapping modules: ");
  // TODO - generate .so's and load dynamically
  villagebus_init();
  accounting_init();
  config_init();
  package_init();
#ifdef ENABLE_MOD_PROVISION
  provision_init();
#endif
#ifdef ENABLE_MOD_REDIS
  db_init();
#endif
#ifdef ENABLE_MOD_SNMP
  snmp_init();
#endif
  sys_init();
  http_init();
  telephony_init();
  string* modules = (string*)send(VillageBus->modules, s_tojson, false);
  wprintl(L"Loaded modules %s\n", (char*)send(modules, s_string_tochar));

  /** - compile request ------------------------------------------------- */
  const fexp* message = (fexp*)send(VillageBus, s_villagebus_compile, request);
  // TODO debug output for compiled request

  /** - evaluate message ------------------------------------------------ */
  message = (fexp*)send(VillageBus, s_villagebus_evaluate, message);

  /** - print eval result (if any) -------------------------------------- */
  if (message == fexp_nil) {  // do nothing & allow handlers to manage their own output
    if (request->state == OUT_HEADER) {  // we printed nothing out, so this is a true nil
      request->out(request, L"undefined"); 
    }
    // TODO - clean all of this dire evil up somewhat, thing are getting 
    //        completely out of hand w/ the output handling!!!
    // We only need these three options:
    // 1. Output module response as JSON output
    // 2. Output module error as JSON error
    // 3. Let the module do its own output handling

  } else if ((vtable*)send(message, s_type) == string_vt) {     // strings are printed verbatim - WTF?!?!
    request->out(request, ((string*)message)->buffer);          // not best solution. should be json.

  } else if (send(message, s_fexp_car) == s_villagebus_error) { // TODO - handle error messages in jQuery 
    message = (fexp*)send(message, s_fexp_cdr);
    message = (fexp*)send(message, s_fexp_car);
    string* error = (string*)send(message, s_tojson, false);
    request->out(request, L"{ 'error' : %S }", error->buffer);

  } else if (send(message, s_fexp_car) == s_villagebus_json) { // encapsulated json - usually from mod_db 
    message = (fexp*)send(message, s_fexp_cdr);
    string* json = (string*)send(message, s_tojson, true);
    request->out(request, json->buffer);

  } else {                                                     // everything else treated as plain json
    string* json = (string*)send(message, s_tojson, false);
    request->out(request, json->buffer);
  }

  /** - release resources & exit ---------------------------------------- */
  return exit_success(request);
}
