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

#include "village-bus-radius.h"


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

  /* parse user attributes and output results */
  int first_time = 1;
  char* last_username = NULL;
  char is_prepaid = 0;
  while ((row = mysql_fetch_row(result)) != NULL) {

    /* username & type */
    if ((last_username == NULL) || strcasecmp(last_username, row[0]) != 0) { 
      if (!first_time) {
	printf("\t\ttype : \"%s\"\n", (is_prepaid ? "prepaid" : "flatrate"));
        printf("\t},");
      }
      first_time = 0;
      printf("\n\t{\n");
      printf("\t\tusername : \"%s\",\n", row[0]);
    }

    /* parse attributes */
    if (strcasecmp("ClearText-Password", row[1]) == 0) {
      printf("\t\tpassword : \"%s\",\n", row[3]);
    } else if (strcasecmp("Max-Prepaid-Session", row[1]) == 0) {
      is_prepaid = 1;
      printf("\t\tseconds : \"%s\",\n", row[3]);
    }

    if (last_username != NULL) free(last_username);
    last_username = strdup(row[0]);
  }
  printf("\t\ttype : \"%s\"\n", (is_prepaid ? "prepaid" : "flatrate"));
  printf("\t}");

  /* release memory used to store results and close connection */
  if (result != NULL) {
    mysql_free_result(result);
  }
  if (connection != NULL) {
    mysql_close(connection);
  }
}

