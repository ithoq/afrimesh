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


int check_username(const char* username)
{
  MYSQL* connection = NULL;
  unsigned long count;

  /* check that username is unique */
  MYSQL_RES* result = NULL;
  MYSQL_ROW  row;
  if (mysql(&connection, "SELECT count(*) FROM radcheck WHERE username='%s'", username) != 0) {
    return -1;
  }
  result = mysql_use_result(connection);
  if ((row = mysql_fetch_row(result)) == NULL) {
    printf("\t{\n\t\t\"error\" : \"Could not perform username check\"\n\t}\n");
    return -1;
  }
  count = atoi(row[0]);
  if (result != NULL) {
    mysql_free_result(result);
  }
  if (count != 0) {
    printf("\t{\n\t\t\"error\" : \"The username '%s' already exists\"\n\t}\n", username);
    return -1;
  }

  /* close connection */
  if (connection != NULL) {
    mysql_close(connection);
  }

  return 0;
}


int generate_username(char** ret, size_t n, const char* base)
{
  MYSQL* connection = NULL;

  /* get base count */
  MYSQL_RES* result = NULL;
  MYSQL_ROW  row;
  unsigned long base_count;
  if (mysql(&connection, "SELECT count(*) FROM radcheck WHERE username like '%s%%'", base) != 0) { // TODO - idiotic. fix.
    return -1;
  }
  result = mysql_use_result(connection);
  if ((row = mysql_fetch_row(result)) == NULL) {
    printf("\t{\n\t\t\"error\" : \"Could not perform basename check\"\n\t}\n");
    return - 1;
  }
  base_count = atoi(row[0]);
  snprintf(ret, n, "%s%s", base, row[0]);
  if (result != NULL) {
    mysql_free_result(result);
  }

  /* close connection */
  if (connection != NULL) {
    mysql_close(connection);
  }

  return 0;
}


#include <sys/time.h>
int generate_password(char* ret, size_t n) 
{
  struct timeval t;
  gettimeofday(&t, NULL);
  srand(t.tv_sec);

  for (size_t t = 0; t < n; t++) {
    char c = ((float)(rand() / (float)RAND_MAX) * 26.0) + 101;
    ret[t] = c;
  }

  ret[n-1] = '\0';

  return 0;
}


/* TODO - sanitize sql inputs! */
void radius_new_mysql(const char* username, const char* type, int seconds)
{
  MYSQL* connection = NULL;

  /* if type is prepaid - generate a username */
  if (strcmp(type, "prepaid") == 0) {
    char buf[256];
    if (generate_username(buf, sizeof(buf), username) != 0) {
      return;
    }
    username = buf;
  }

  /* check that username is unique */
  if (check_username(username) != 0) {
    return;
  }

  /* generate a password */
  char password[8];
  generate_password(password, 8);

  /* create user */
  unsigned long count = 0;
  if (mysql(&connection, "INSERT INTO radcheck (username, attribute, op, value) "
                         "VALUES ('%s', 'ClearText-Password', ':=', '%s')", username, password) != 0) {
    return;
  }
  count = mysql_affected_rows(connection);
  if (strncasecmp("prepaid", type, 7) == 0) {
    if (mysql(&connection, "INSERT INTO radcheck (username, attribute, op, value) "
	      "VALUES ('%s', 'Max-Prepaid-Session', ':=', '%d')", username, seconds) != 0) {
      return;
    }
  } else if (strncasecmp("flatrate", type, 8) == 0) {
  } else if (strncasecmp("metered", type, 7) == 0) { // TODO - implement
  } else {
    // TODO - handle 
  }


  /* output result */
  printf("\t{\n");
  printf("\t\t\"username\" : \"%s\",\n", username);
  printf("\t\t\"password\" : \"%s\"\n", password);
  printf("\t}\n");

  /* close connection */
  if (connection != NULL) {
    mysql_close(connection);
  }
}

