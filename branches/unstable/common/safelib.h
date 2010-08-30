/**
 * Adapted from Chapter 1 of the O'Reilly Secure Programming Cookbook
 * http://www.secureprogramming.com
 */

#ifndef SAFELIB_H
#define SAFELIB_H

#include <stdio.h>
#include <sys/types.h>

pid_t safe_fork(int *, size_t);

FILE *safe_popen(char* command, char** argv, const char *mode);
void safe_pclose(FILE *);

#define popen safe_popen
#define pclose safe_pclose

#endif
