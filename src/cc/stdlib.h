/* stdlib.h -- declarations for custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDLIB_H
#define CC_STDLIB_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdarg.h>
#include <stddef.h>

#include "pascal/pascal.h"
#include "cc/errno.h"

int  cc_atoi(const char *src);
long cc_atol(const char *src);

long cc_strtol(const char *nptr, char **endptr, int base);

void *cc_malloc(size_t size);
void  cc_free(void *ptr);

void cc_exit(int status);

/* Aliases */

#define atoi cc_atoi
#define atol cc_atol

#define strtol cc_strtol

#define malloc cc_malloc
#define free   cc_free

#define exit cc_exit

#endif  /* CC_STDLIB_H */
