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

#include <errno.h>
#include <unistd.h>

#include <uci.h> 
#include <common/safelib.h>

#include "mod_sys.h"


/* - sys ----------------------------------------------------------------- */
vtable* sys_vt = 0;
object* _Sys = 0;
sys*    Sys = 0;
symbol* s_sys = 0;
symbol* s_sys_service = 0;
symbol* s_sys_syslog  = 0;
symbol* s_sys_uname   = 0;
symbol* s_sys_version = 0;

void sys_init() 
{
  sys_vt = (vtable*)send(object_vt, s_delegated); // TODO - inherit from VillageBus ?
  send(sys_vt, s_addMethod, s_print, sys_print);
  send(sys_vt, s_addMethod, s_villagebus_evaluate, sys_evaluate);
  _Sys = send(sys_vt, s_allocate, 0);

  // register local symbols
  s_sys_service = (symbol*)symbol_intern(0, _Sys, L"service"); 
  s_sys_syslog  = (symbol*)symbol_intern(0, _Sys, L"syslog"); 
  s_sys_uname   = (symbol*)symbol_intern(0, _Sys, L"uname"); 
  s_sys_version = (symbol*)symbol_intern(0, _Sys, L"version"); 
  send(sys_vt, s_addMethod, s_sys_service, sys_service);
  send(sys_vt, s_addMethod, s_sys_syslog,  sys_syslog);
  send(sys_vt, s_addMethod, s_sys_uname,   sys_uname);
  send(sys_vt, s_addMethod, s_sys_version, sys_version);

  // global module instance vars
  Sys = (sys*)send(_Sys->_vt[-1], s_allocate, sizeof(sys));

  // register module with VillageBus
  s_sys = (symbol*)symbol_intern(0, 0, L"sys");
  fexp* module = (fexp*)send(Fexp, s_new, s_sys, Sys);
  VillageBus->modules = (fexp*)send(VillageBus->modules, s_fexp_cons, module);
}



const fexp* sys_evaluate(closure* c, sys* self, const fexp* expression)
{
  // TODO - VillageBus->request context should be coming in via the closure

  // search for name in local context
  string* name    = (string*)send(expression, s_fexp_car);
  fexp*   message = (fexp*)send(expression, s_fexp_cdr);
  object* channel = symbol_lookup(0, _Sys, name->buffer);
  if (channel) {
    return (fexp*)send(self, channel, message);
  } 

  return (fexp*)send(VillageBus, 
                     s_villagebus_error, 
                     L"mod_sys has no registered handler for requested name: %S", 
                     name->buffer);  
}



sys* sys_print(closure* c, sys* self)
{
  wprintf(L"#<SYS.%p>", self);
  return self;
}



/* - names -------------------------------------------------------------- */

/**
 * /sys/service/<name>[?command=start|stop|reload|status]
 * /sys/service?name=<name>[?command=start|stop|reload|status]
 *
 * Default command is 'status'
 *
 * TODO - status is GET
 * TODO - start|stop|reload are PUT
 *
 * Search params always take priority over path spec so for e.g
 *   /sys/service/network?name=ntpclient?command=status
 * Will be router to ntpclient NOT network
 */
const fexp* sys_service(closure* c, sys* self, const fexp* message)
{
  Request* request = VillageBus->request; 

  // get parameters
  char* name    = (char*)send(send(message, s_fexp_car), s_string_tochar);
  char* command = "status";
  if (request->json) {
    const char* param = json_object_get_string(json_object_object_get(request->json, "name"));
    name    = param ? (char*)param : name;
    param = json_object_get_string(json_object_object_get(request->json, "command"));
    command = param ? (char*)param : command;
  }

  char path[64];
  snprintf(path, 64, "/etc/init.d/%s", name);
  char* argv[3];
  argv[0] = path; 
  argv[1] = command;
  argv[2] = 0;

  // TODO - only support status&reload until we have some level of security in the system
  if ((strncasecmp(command, "reload", 6) == 0) || 
      (strncasecmp(command, "status", 6) == 0)) {
    return exec(argv[0], argv);
  } 

  return (fexp*)send(VillageBus, s_villagebus_error, L"Unknown command '%s' for service: %s", command, name);
}



/**
 * Jan  2 18:12:08 <daemon.err> 192.168.20.2 batmand[567]: Error - got packet from unknown client: 10.0.0.3 (tunnelled sender ip 169.254.
0.3)  
*/
typedef struct {
  char* timestamp;
  char* level;
  char* address;
  char* process;
  char* message;
} LogEntry;
LogEntry parse_entry(const char* input, size_t length)
{
  LogEntry entry;
  const char* cursor = input;
  cursor = parse_field(cursor, length, (char[]){' ',':',':',' ',-1}, &entry.timestamp);
  cursor = parse_field(cursor, length, (char[]){'<','.','>',' ',-1}, &entry.level);
  cursor = parse_field(cursor, length, (char[]){' ',-1},             &entry.address);
  cursor = parse_field(cursor, length, (char[]){':',' ',-1},         &entry.process);
  entry.message = strdup(cursor);
  return entry;
}



/**
 * TODO - add support for POSTing to syslog
 */
const fexp* sys_syslog(closure* c, sys* self, const fexp* message)
{
  Request* request = VillageBus->request; 
  object* reply = (object*)fexp_nil;

  // get parameters
  int count = 10;
  if (request->json) {
    count = json_object_get_int(json_object_object_get(request->json, "count"));
  }

  size_t readsize = 256;
  char   buffer[readsize];
  struct json_object* entries = json_object_new_array();

  /* track down logfile in common locations */
  const char* name_linux   = "/var/log/messages";
  const char* name_freebsd = "/var/log/messages";
  const char* name_osx     = "/var/log/system.log";
  const char* name;
  if (access(name_linux, F_OK) == 0) {
    name = name_linux;
  } else if (access(name_freebsd, F_OK) == 0) {
    name = name_freebsd;
  } else if (access(name_osx, F_OK) == 0) {
    name = name_osx;
  } else {
    struct json_object* entry = json_object_new_object();
    json_object_object_add(entry, "timestamp", json_object_new_string("-"));
    json_object_object_add(entry, "level",     json_object_new_string("<daemon.err>"));
    json_object_object_add(entry, "address",   json_object_new_string("localhost"));
    json_object_object_add(entry, "process",   json_object_new_string("village-bus-syslog"));
    json_object_object_add(entry, "message",   json_object_new_string("Could not locate system logfile"));
    json_object_array_add(entries, entry);  
    request->out(request, L"%s", json_object_get_string(entries));
    return fexp_nil;
  }

  /* open logfile */
  FILE* logfile = fopen(name, "r");
  if (!logfile) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"Failed to open system log: %s", strerror(errno));
  }

  /* find eof */
  if (fseek(logfile, 0, SEEK_END) == -1) {
    fclose(logfile);
    return (fexp*)send(VillageBus, s_villagebus_error, L"Initial seek failed: %s", strerror(errno));
  }
  long eof = ftell(logfile);
  if (eof == -1) {
    fclose(logfile);
    return (fexp*)send(VillageBus, s_villagebus_error, L"Could not locate end of logfile: %s", strerror(errno));
  }
  
  long eol = eof;
  size_t lines = 0;
  for (lines = 0; lines < count && eol > 0; lines++) {
    if (readsize > eol) { 
      readsize = eol; 
    }
    //printf("lines: %d  eol:%d  readsize:%d\n", lines, eol, readsize);

    /* go to last known line end & read previous 'readsize' bytes */
    if (fseek(logfile, eol - readsize, SEEK_SET) == -1) {
      fclose(logfile);
      return (fexp*)send(VillageBus, s_villagebus_error, L"Seek failed: %s", strerror(errno));
    }
    size_t bytesread = fread(buffer, 1, readsize, logfile);
    if (bytesread == -1) {
      fclose(logfile);
      return (fexp*)send(VillageBus, s_villagebus_error, L"Read failed: %s", strerror(errno));
    }
    buffer[bytesread] = 0;

    /* read buffer backward until there's a newline */
    char* start;
    for (start = buffer + bytesread - 2; start != buffer; start--) {
      if (*start == '\n') {
        break;
      }
    }

    /* everything between here and end of buffer is the line */
    size_t line_length = (buffer + bytesread) - start;
    start[line_length] = 0;
    /* TODO - parse line */
    if (start[0] == '\n') { start++; }
    //printf("-> %d : |%s|\n", lines, start);
    LogEntry logentry = parse_entry(start, strlen(start));
    //printf("-> %d : |%s|\n", lines, start + 1);
    //LogEntry logentry = parse_entry(start + 1, strlen(start + 1));
    struct json_object* entry = json_object_new_object();
    json_object_object_add(entry, "timestamp", json_object_new_string(logentry.timestamp));
    json_object_object_add(entry, "level",     json_object_new_string(logentry.level));
    json_object_object_add(entry, "address",   json_object_new_string(logentry.address));
    json_object_object_add(entry, "process",   json_object_new_string(logentry.process));
    json_object_object_add(entry, "message",   json_object_new_string(logentry.message));
    json_object_array_add(entries, entry);
    
    /* move last known eol backward */
    eol = eol - line_length;
  }

  /* cleanup */
  if (fclose(logfile) != 0) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"Error closing system log: %s", strerror(errno));
  }

  /* output log */
  request->out(request, L"%s", json_object_get_string(entries));

  return fexp_nil;
}



/**
 *
 */
const fexp* sys_uname(closure* c, sys* self, const fexp* message)
{
  char* argv[3];
  argv[0] = "uname";
  argv[1] = "-srm";
  argv[2] = 0;
  return exec(argv[0], argv); /* TODO - add flags for arguments ? */
}



/**
 *
 */
const string* sys_version(closure* c, sys* self, const fexp* message)
{
  object* reply = (object*)fexp_nil;
  wprintl(L"GET /sys/version\n");
#ifdef AFRIMESH_VERSION
  reply = send(String, s_string_fromchar, AFRIMESH_VERSION, strlen(AFRIMESH_VERSION));
#else
  reply = send(String, s_new, L"r?-unknown", wcslen(L"r?-unknown"));
#endif
  reply = send(reply, s_tojson, false);
  return (string*)reply;
}




/* - utilities ---------------------------------------------------------- */

/**
 * execute the given command and return standard output as an array of lines
 */
const fexp* exec(char* command, char** arguments)
{
  return exec_parsed(command, arguments, (void*)NULL);
}


/**
 * execute the given command & print the output directly to stdout
 */
const fexp* exec_out(char* command, char** arguments)
{
  char* tmp;
  if ((tmp = path_exists(command)) == NULL) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"Command not found: %s", command);
  }
  command = tmp;

  FILE* output;
  output = safe_popen(command, arguments, "r");
  while (1) {
    size_t length;
    char* line = fgetln(output, &length);
    if (line == NULL) {
      if (feof(output) != 0) {          /* eof */
        break;
      } else if (ferror(output) != 0) { /* error */
        fclose(output);
        return (fexp*)send(VillageBus, s_villagebus_error, L"Error reading command output: %s", strerror(errno));
      } 
      fclose(output);
      return (fexp*)send(VillageBus, s_villagebus_error, L"Unexpected end of command output: %s", strerror(errno));
    }
    fprintf(stdout, "%s", line);
  }
  fclose(output);
  return fexp_nil;
}


/**
 * execute the given command & return the output as a string
 */
const fexp* exec_string(char* command, char** arguments)
{
  char* tmp;
  if ((tmp = path_exists(command)) == NULL) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"Command not found: %s", command);
  }
  command = tmp;

  size_t max   = 1024; /* Hard limit on number of output lines */
  size_t count = 0;
  string* ret = (string*)send(String, s_new, L"", 0);
  FILE* output;
  output = safe_popen(command, arguments, "r");
  while (1 && count < max) { 
    size_t length;
    char* line = fgetln(output, &length);
    if (line == NULL) {
      if (feof(output) != 0) {          /* eof */
        break;
      } else if (ferror(output) != 0) { /* error */
        fclose(output);
        return (fexp*)send(VillageBus, s_villagebus_error, L"Error reading command output: %s", strerror(errno));
      } 
      fclose(output);
      return (fexp*)send(VillageBus, s_villagebus_error, L"Unexpected end of command output: %s", strerror(errno));
    }
    count++;
    string* sline = (string*)send(String, s_string_fromchar, line, length);
    ret = (string*)send(ret, s_string_add, sline);
  }
  fclose(output);
  return (fexp*)ret;
}



/**
 * execute the given command and output the result as an array of
 * objects parsed according to the given parser function
 */
const fexp* exec_parsed(char* command, char** arguments, 
                        struct json_object* (*parser)(const char*, size_t))
{
  Request* request = VillageBus->request; 

  size_t max   = 1024; /* Hard limit on number of output lines - TODO last 'max' lines rather than first ? */
  size_t count = 0;
  struct json_object* lines;
  FILE* output;

  char* tmp;
  if ((tmp = path_exists(command)) == NULL) {
    return (fexp*)send(VillageBus, s_villagebus_error, L"Command not found: %s", command);
  }
  command = tmp;
  
  lines = json_object_new_array();
  output = safe_popen(command, arguments, "r");
  while (1 && count < max) { 
    size_t length;
    char* line = fgetln(output, &length);
    if (line == NULL) {
      if (feof(output) != 0) {          /* eof */
        break;
      } else if (ferror(output) != 0) { /* error */
        fclose(output);
        return (fexp*)send(VillageBus, s_villagebus_error, L"Error reading command output: %s", strerror(errno));
      } 
      fclose(output);
      return (fexp*)send(VillageBus, s_villagebus_error, L"Unexpected end of command output: %s", strerror(errno));
    }
    if (parser == NULL) {
      json_object_array_add(lines, json_object_new_string_len(line, length - 1));
    } else {
      struct json_object* parsed = parser(line, length);
      if (parsed != NULL) {
        json_object_array_add(lines, parsed);
      }
    }
    count++;
  }
  fclose(output);

  /* output command output */
  request->out(request, L"%s", json_object_get_string(lines));

  return fexp_nil;
}

