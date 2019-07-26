/* stdlib.h -- declarations for custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDLIB_H
#define CC_STDLIB_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stddef.h>
#include <stdarg.h>
#include "pascal.h"
#include "startup.h"
#include "cc/errno.h"

extern char **cc_environ;

#define CC_ATEXIT_MAX _CC_ATEXIT_MAX

int cc_atexit(void (*__far function)());

int  cc_atoi(const char *src);
long cc_atol(const char *src);

long cc_strtol(const char *nptr, char **endptr, int base);

void *cc_malloc(size_t size);
void  cc_free(void *ptr);

void cc_exit(int status);

char   *cc_getenv(const char *name);
int16_t cc_unsetenv(const char *name);
int16_t cc_setenv(const char *name, const char *value, int16_t overwrite);

/*** Initialization ***/

bool environ_init(void);

/*** Aliases ***/

/* cc_PrefixSeg is defined in startup.h */
#define _psp cc_PrefixSeg

#define ATEXIT_MAX CC_ATEXIT_MAX

#define environ cc_environ

#define atexit cc_atexit

#define atoi cc_atoi
#define atol cc_atol

#define strtol cc_strtol

#define malloc cc_malloc
#define free   cc_free

#define exit cc_exit

#define getenv cc_getenv
#define unsetenv cc_unsetenv
#define setenv cc_setenv

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux cc_environ "*";
#pragma aux cc_atexit "*";
#pragma aux cc_atoi "*";
#pragma aux cc_atol "*";
#pragma aux cc_strtol "*";
#pragma aux cc_malloc "*";
#pragma aux cc_free "*";
#pragma aux cc_exit "*";
#pragma aux cc_getenv "*";
#pragma aux cc_unsetenv "*";
#pragma aux cc_setenv "*";
#pragma aux environ_init "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_STDLIB_H */
