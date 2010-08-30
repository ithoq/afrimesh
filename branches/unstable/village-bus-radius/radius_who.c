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


#include <sys/socket.h>

#include "village-bus-radius.h"


/**
 * reads the radius radutmp file and dumps the content to stdout as json 
 */
void radius_who()
{
  FILE* radutmp_file;
  radutmp_file = fopen(radutmp_filename, "r");
  if (radutmp_file == NULL) {
    printf("\t{\n\t\t\"error\" : \"%s\"\n\t}\n", strerror(errno));
    return;
  }

  struct radutmp rt;
  char buf[256];
  int first_time = 1;
  while (fread(&rt, sizeof(rt), 1, radutmp_file) == 1) {
    if (!first_time) {
      printf(",");
    }
    first_time = 0;
    printf("\n\t{\n");
    switch (rt.type) {
    case P_IDLE: 
      printf("\t\t\"type\"              : \"idle\",\n");
      break;
    case P_LOGIN: 
      printf("\t\t\"type\"              : \"login\",\n");
      break;
    default: 
      printf("\t\t\"type\"              : \"unknown\",\n");
    }
    printf("\t\t\"username\"          : \"%s\",\n", rt.login);
    strncpy(buf, rt.session_id, 8); buf[8] = '\0';
    printf("\t\t\"session_id\"        : \"%s\",\n", buf);
    printf("\t\t\"nas_address\"       : \"%s\",\n", inet_ntop(AF_INET, &rt.nas_address, buf, sizeof(buf)));
    printf("\t\t\"nas_port\"          : %d,\n", rt.nas_port);
    switch (rt.proto) {
    case 'S': 
      printf("\t\t\"service_type\"      : \"Framed-User\",\n");
      printf("\t\t\"framed_protocol\"   : \"SLIP\",\n");
      break;
    case 'P': 
      printf("\t\t\"service_type\"      : \"Framed-User\",\n");
      printf("\t\t\"framed_protocol\"   : \"PPP\",\n");
      break;
    default: 
      printf("\t\t\"service_type\"      : \"Login-User\",\n");
      break;
    }
    printf("\t\t\"framed_address\" : \"%s\",\n", inet_ntop(AF_INET, &rt.framed_address, buf, sizeof(buf)));
    time_t now = 0;
    now = time(NULL);
    printf("\t\t\"session_time\" : %d,\n", (unsigned int)(now - rt.time));
    strncpy(buf, rt.caller_id, 16); buf[16] = '\0';
    printf("\t\t\"caller_id\"   : \"%s\"\n", buf);

    printf("\t}");
  }

  if (radutmp_file != NULL) {
    fclose(radutmp_file);
  }
}

