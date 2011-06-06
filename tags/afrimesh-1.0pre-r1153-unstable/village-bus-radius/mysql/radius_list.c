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

#include <json_cgi.h> /* for log_message */
#include "village-bus-radius.h"


void print_customer(const char* username, const char* cleartext_password, const char* max_prepaid_session)
{
  printf("{ ");
  printf("\"username\" : \"%s\", ", username);
  if (max_prepaid_session) {
    printf("\"type\" : \"prepaid\", ");
    printf("\"seconds\" : %s", max_prepaid_session);
  } else {
    printf("\"type\" : \"flatrate\"");
  }
  printf(" }");
}


void radius_list_mysql()
{
  MYSQL* connection = NULL;
  MYSQL_RES* result  = NULL;
  MYSQL_ROW  row    = NULL;
   
  /* query user attributes */
  if (mysql(&connection, "SELECT username, attribute, op, value FROM radcheck WHERE username != 'chillispot' ORDER BY username") != 0) {
    return;
  }
  result = mysql_use_result(connection);

  char* username    = NULL;
  char* cleartext_password = NULL;
  char* max_prepaid_session = NULL;
  while ((row = mysql_fetch_row(result)) != NULL) {
    if (username && strcasecmp(username, row[0]) != 0) {
      print_customer(username, cleartext_password, max_prepaid_session);
      printf(", ");
      cleartext_password = NULL;
      max_prepaid_session = NULL;
    }
    username = strdup(row[0]);
    if (strcasecmp("ClearText-Password", row[1]) == 0) {
      cleartext_password = strdup(row[3]);
    } else if (strcasecmp("Max-Prepaid-Session", row[1]) == 0) {
      max_prepaid_session = strdup(row[3]);
    } else {
      log_message("Unknown attribute: %s %s %s %s\n", row[0], row[1], row[2], row[3]);
    }
  }
  if (username) { /* don't forget the last one */
    print_customer(username, cleartext_password, max_prepaid_session);
  }


  /* release memory used to store results and close connection */
  if (result != NULL) {
    mysql_free_result(result);
  }
  if (connection != NULL) {
    mysql_close(connection);
  }
}

