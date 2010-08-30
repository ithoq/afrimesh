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


/* utility functions */
void radius_set_type(MYSQL* connection, const char* username, const char* new_type);



/**
 * Modify radius user
 */
void radius_modify_mysql(const char* username, const char* new_username, const char* new_password, const char* new_type)
{
  MYSQL* connection = NULL;
  unsigned long count = 0;

  /* update user information */
  if (new_username) {
    if (mysql(&connection, "UPDATE radcheck SET username='%s' WHERE username='%s'", new_username, username) != 0) {
      printf("{ \"error\" : \"Failed to update username\" }");
      return;
    }
    username = new_username;
  }
  if (new_password) {
    if (mysql(&connection, "UPDATE radcheck SET value='%s' WHERE username='%s' AND attribute='ClearText-Password'", new_password, username) != 0) {
      printf("{ \"error\" : \"Failed to update password\" }");
      return;
    }
  }
  if (new_type) {
    radius_set_type(connection, username, new_type);
  }

  /* output result */
  if (connection) {
    count = mysql_affected_rows(connection);
  }
  printf("\t{ \"count\" : %d }\n", count); 

  /* close connection */
  if (connection) {
    mysql_close(connection);
  }
}



/**
 * Set account type for radius user
 */
void radius_set_type(MYSQL* connection, const char* username, const char* new_type)
{
  char* query = NULL;
  
  /* clear all existing rules for username */
  if (mysql(&connection, "DELETE FROM radcheck WHERE username='%s' AND attribute='Max-Prepaid-Session'", username) != 0) {
    return;
  }

  /* create rules for new type */
  if (strncasecmp("prepaid", new_type, 7) == 0) {
    if (mysql(&connection, "INSERT INTO radcheck (UserName, Attribute, op, Value) VALUES ('%s', 'Max-Prepaid-Session', ':=', '3600')", username) != 0) {
      log_message("Could not set new type '%s' for '%s'\n", new_type, username);
      return;
    }
  } else if (strncasecmp("flatrate", new_type, 8) == 0) {
    // flatrate has no attributes
  } else if (strncasecmp("disabled", new_type, 8) == 0) { // TODO - implement
  } else if (strncasecmp("metered", new_type, 7) == 0)  { // TODO - implement
  }

}
