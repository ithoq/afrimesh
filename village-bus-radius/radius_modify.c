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

void radius_modify_mysql(const char* username, const char* new_username, const char* new_password, const char* new_type)
{
  MYSQL* connection = NULL;
  unsigned long count;

  /* update user information */
  if (mysql(&connection, "UPDATE radcheck SET username='%s' WHERE username='%s'", new_username, username) != 0) {
    return;
  }
  if (mysql(&connection, "UPDATE radcheck SET value='%s' WHERE username='%s' AND attribute='ClearText-Password'", new_password, new_username) != 0) {
    return;
  }
  count = mysql_affected_rows(connection);

  if (strncasecmp("prepaid", new_type, 7)) {
  } else if (strncasecmp("flatrate", new_type, 8)) {
  } else if (strncasecmp("disabled", new_type, 8)) { 
  } else if (strncasecmp("metered", new_type, 7)) { // TODO - implement
  } else {
    // TODO - handle 
  }

  /* output result */
  printf("\t{ count : %d }\n", count); 

  /* close connection */
  if (connection != NULL) {
    mysql_close(connection);
  }

}
