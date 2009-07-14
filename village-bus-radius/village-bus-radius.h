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

#ifndef VILLAGE_BUS_RADIUS_H
#define VILLAGE_BUS_RADIUS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <uci.h> 

/* TODO - make configurable */
/* TODO - also support querying from mysql */
static char* radutmp_filename = "/var/log/freeradius/radutmp";
static char* radius_mysql_server   = "localhost";
static char* radius_database_type   = "1";
static char* radius_mysql_database = "radius";
static char* radius_mysql_username = "radius";
static char* radius_mysql_password = "radpass";

/* radius commands */
void radius_who();
void radius_list_mysql();
void radius_new_mysql(const char* username, const char* type, int seconds);
void radius_delete_mysql(const char* username);
void radius_modify_mysql(const char* username, const char* new_username, const char* new_password, const char* new_type);

/* wrapper for executing mysql queries */
#include <mysql.h>
int mysql(MYSQL** connection, const char* query, ...);

/* originally defined in freeradius/src/include/radutmp.h */
#ifndef P_UNKNOWN
  #define P_UNKNOWN       0
  #define P_LOCAL         'L'
  #define P_RLOGIN        'R'
  #define P_SLIP          'S'
  #define P_CSLIP         'C'
  #define P_PPP           'P'
  #define P_AUTOPPP       'A'
  #define P_TELNET        'E'
  #define P_TCPCLEAR      'T'
  #define P_TCPLOGIN      'U'
  #define P_CONSOLE       '!'
  #define P_SHELL         'X'
#endif

#define P_IDLE          0
#define P_LOGIN         1

struct radutmp {
  char login[32];               /* Loginname */
                                /* FIXME: extend to 48 or 64 bytes */
  unsigned int nas_port;        /* Port on the terminal server (32 bits). */
  char session_id[8];           /* Radius session ID (first 8 bytes at least)*/
                                /* FIXME: extend to 16 or 32 bytes */
  unsigned int nas_address;     /* IP of portmaster. */
  unsigned int framed_address;  /* SLIP/PPP address or login-host. */
  int proto;                    /* Protocol. */
  time_t time;                  /* Time entry was last updated. */
  time_t delay;                 /* Delay time of request */
  int type;                     /* Type of entry (login/logout) */
  char porttype;                /* Porttype (I=ISDN A=Async T=Async-ISDN */
  char res1,res2,res3;          /* Fills up to one int */
  char caller_id[16];           /* Calling-Station-ID */
  char reserved[12];            /* 3 ints reserved */
};

struct uci_context* UCI_CONTEXT;
#endif /* VILLAGE_BUS_RADIUS_H */

