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
 * 
 */
int exit_failure() 
{
  cgi_release();
  log_release();
  return EXIT_FAILURE;
}

int exit_success() 
{
  cgi_release();
  log_release();
  return EXIT_SUCCESS;
}



/**
 *
 */
int main(int argc, char** argv)
{
  /** - output HTTP header ------------------------------------------------ */
  //httpd_out("Content-type: application/json\n\n");
  whttpd_out(L"Content-type: text/plain\n\n");

  /** - parse request ----------------------------------------------------- */
  const Request* request = cgi_request(argc, argv);
  /*whttpd_out(L"request->method   : %d\n", request->method);
  whttpd_out(L"request->href     : %S\n", request->href);
  whttpd_out(L"request->pathname : %S\n", request->pathname);
  whttpd_out(L"request->search   : %S\n", request->search);
  if (request->search) {
    whttpd_out(L"request->search   : %S\n", search_to_json(request->search, wcslen(request->search)));
  }
  whttpd_out(L"request->data     : %s\n", request->data);*/

  /** - bootstrap interpreter --------------------------------------------- */
  obj_init();
  fexp_init();

  /** - bootstrap modules ------------------------------------------------- */
  villagebus_init();
  db_init();
  config_init();
  /*printf("villagebus->modules: ");
  send(((villagebus*)VillageBus)->modules, s_print);
  printf("\n");*/

  /** - compile request --------------------------------------------------- */
  const fexp* message = (fexp*)send(VillageBus, s_villagebus_compile, request);

  /** - evaluate message -------------------------------------------------- */
  message = (fexp*)send(VillageBus, s_villagebus_evaluate, message);

  /** - print eval result (if any) ---------------------------------------- */
  if (message == fexp_nil) {
    //whttpd_out(L"jsonp(null, {})\n");
  } else if (send(message, s_fexp_car) == s_villagebus_error) {
      whttpd_out(L"jsonp(");
      send(message, s_print);
      whttpd_out(L", null)\n");
  } else {
    whttpd_out(L"jsonp(null, ");
    send(message, s_print);
    whttpd_out(L")\n");
  }

  
  /** - release resources ------------------------------------------------- */
  cgi_release();

  //printf("-----------------------------------------------------\n");

  return exit_success();
}



