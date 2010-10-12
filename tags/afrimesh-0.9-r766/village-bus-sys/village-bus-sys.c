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


#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>

#include <common/safelib.h>

#include "village-bus-sys.h"


/**
 * Check to see if a file called 'name' exists as specified or in the system path
 * 
 * NULL if not, else fully qualified binary name
 */
char* path_exists(const char* name)
{
  const char *path, *cursor;
  size_t count;
  size_t namelen;
  static char* buf = NULL;
  if (buf == NULL) {
    free(buf);
    buf = malloc(PATH_MAX * sizeof(char));
  }

  if (name == NULL || name[0] == '\0') {
    return NULL;
  }

  /* if it is an absolute or relative path, just check it  */
  if (strchr(name, '/') && access(path, F_OK) == 0) {
    return name;
  }

  /* Search the system path for the file */
  namelen = strlen(name);
  //path = getenv("PATH"); {  TODO - we need a better solution for crappy httpd paths
  path = "/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/opt/local/bin";

  cursor = path;
  do {
    for (path = cursor; *cursor != 0 && *cursor != ':'; cursor++) continue;
    if (cursor == path) {
      path = ".";
      count = 1;
    } else {
      count = cursor - path;
    }
    memcpy(buf, path, count);
    buf[count] = '/';
    memcpy(buf + count + 1, name, namelen);
    buf[count + namelen + 1] = '\0';
    if (access(buf, F_OK) == 0) {
      return buf;
    }
  } while (*cursor++ == ':');

  return NULL;
}


struct json_object* die_gracefully(FILE* file, struct json_object* error)
{
  fclose(file);
  return error;
}


// TODO - not threadsafe
static char* cut_field_buf = NULL;
char* cut_field(const char* input, size_t start, size_t end)
{
  if (cut_field_buf != NULL) {
    free(cut_field_buf);
  }
  size_t length = end - start;
  cut_field_buf = malloc((length + 1) * sizeof(char));
  strncpy(cut_field_buf, input + start, length);
  cut_field_buf[length] = '\0';
  return cut_field_buf;
}


char* parse_field(const char* input, size_t length, char* tokens, char** pfield)
{
  char* field;
  char* cursor;
  int n = 0;
  for (cursor = input; tokens[n] != -1   &&
                       *cursor   != '\0' &&
                       cursor    != NULL && 
                       cursor    != (input + length); cursor++) {
    char c = *cursor;
    if (tokens[n] == c) {
      n++;
    } else if (tokens[n] == '_')  { /* eat whitespace */
      if (c == ' ' || c == '\t') {
        continue;
      } 
      n++;
    }
    if (tokens[n] == -1) {
      size_t fieldlen = cursor - input;
      field = malloc(sizeof(char) * fieldlen+1);
      strncpy(field, input, fieldlen);
      field[fieldlen] = '\0';
      *pfield = field;
    }
  }

  if (tokens[n] != -1) {
    *pfield = "<unknown.err>";
    cursor = input;
  }

  return cursor;
}


/**
 * Jan  2 18:12:08 <daemon.err> 192.168.20.2 batmand[567]: Error - got packet from unknown client: 10.0.0.3 (tunnelled sender ip 169.254.0.3)  
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
  char* cursor = input;
  cursor = parse_field(cursor, length, (char[]){' ',':',':',' ',-1}, &entry.timestamp);
  cursor = parse_field(cursor, length, (char[]){'<','.','>',' ',-1}, &entry.level);
  cursor = parse_field(cursor, length, (char[]){' ',-1},             &entry.address);
  cursor = parse_field(cursor, length, (char[]){':',' ',-1},         &entry.process);
  entry.message = cursor;
  return entry;
}



/**
 * Return the last 'n' lines of the system log
 */
struct json_object* sys_syslog(int n)
{
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
    struct json_object* response = json_object_new_object();
    json_object_object_add(response, "result", entries);
    return response;
  }

  /* open logfile */
  FILE* logfile = fopen(name, "r");
  if (!logfile) {
    return jsonrpc_error("Failed to open system log: %s", strerror(errno));
  }

  /* find eof */
  if (fseek(logfile, 0, SEEK_END) == -1) {
    return die_gracefully(logfile, jsonrpc_error("Initial seek failed - %s", strerror(errno)));
  }
  long eof = ftell(logfile);
  if (eof == -1) {
    return die_gracefully(logfile, jsonrpc_error("Could not locate end of logfile - %s", strerror(errno)));
  }
  
  long eol = eof;
  size_t lines = 0;
  for (lines = 0; lines < n && eol > 0; lines++) {
    if (readsize > eol) { 
      readsize = eol; 
    }
    //printf("lines: %d  eol:%d  readsize:%d\n", lines, eol, readsize);

    /* go to last known line end & read previous 'readsize' bytes */
    if (fseek(logfile, eol - readsize, SEEK_SET) == -1) {
      return die_gracefully(logfile, jsonrpc_error("Seek failed - %s", strerror(errno)));
    }
    size_t bytesread = fread(buffer, 1, readsize, logfile);
    if (bytesread == -1) {
      return die_gracefully(logfile, jsonrpc_error("Read failed - %s", strerror(errno)));
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
    return jsonrpc_error("Error closing system log: %s", strerror(errno));
  }

  struct json_object* response = json_object_new_object();
  json_object_object_add(response, "result", entries);
  return response;
}


struct json_object* sys_version()
{
  struct json_object* response = json_object_new_object();
#ifdef AFRIMESH_VERSION
  json_object_object_add(response, "result", json_object_new_string(AFRIMESH_VERSION));
#else
  json_object_object_add(response, "result", json_object_new_string("r?-unknown"));
#endif
  return response;
}


/**
 * execute the given command and return standard output as an array of lines
 */
struct json_object* sys_exec(char* command, char** arguments)
{
  return sys_exec_parsed(command, arguments, (void*)NULL);
}


/**
 * execute the given command and return the result as an array of
 * objects parsed according to the given parser function
 */
struct json_object* sys_exec_parsed(char* command, char** arguments, 
                                    struct json_object* (*parser)(const char*, size_t))
{
  size_t max   = 1024; /* Hard limit on number of output lines - TODO last 'max' lines rather than first ? */
  size_t count = 0;
  struct json_object* lines;
  FILE* output;

  if ((command = path_exists(command)) == NULL) {
    return jsonrpc_error("%s: command not found", command);
  }
  
  lines = json_object_new_array();
  output = safe_popen(command, arguments, "r");
  while (1 && count < max) { 
    size_t length;
    char* line = fgetln(output, &length);
    if (line == NULL) {
      if (feof(output) != 0) {          /* eof */
        break;
      } else if (ferror(output) != 0) { /* error */
        return die_gracefully(output, jsonrpc_error("Error reading command output - %s", strerror(errno)));
      } 
      return die_gracefully(output, jsonrpc_error("Unexpected end of command output - %s", strerror(errno)));
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

  struct json_object* response = json_object_new_object();
  json_object_object_add(response, "result", lines);

  return response;
}


