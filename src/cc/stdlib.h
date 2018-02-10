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
#include "cc/errno.h"

extern uint16_t _cc_psp;

int  cc_atoi(const char *src);
long cc_atol(const char *src);

long cc_strtol(const char *nptr, char **endptr, int base);

void *cc_malloc(size_t size);
void  cc_free(void *ptr);

void cc_exit(int status);

char *custom_getenv(char *dest, const char *name, size_t maxlen);

/* Aliases */

#define _psp _cc_psp

#define atoi cc_atoi
#define atol cc_atol

#define strtol cc_strtol

#define malloc cc_malloc
#define free   cc_free

#define exit cc_exit

#ifdef __WATCOMC__
#pragma aux _cc_psp "*";
#pragma aux cc_atoi "*";
#pragma aux cc_atol "*";
#pragma aux cc_strtol "*";
#pragma aux cc_malloc "*";
#pragma aux cc_free "*";
#pragma aux cc_exit "*";
#pragma aux custom_getenv "*";
#endif

#endif  /* CC_STDLIB_H */
