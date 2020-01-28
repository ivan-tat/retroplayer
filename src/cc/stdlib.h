/* stdlib.h -- declarations for custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_STDLIB_H_INCLUDED
#define _CC_STDLIB_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include <stdarg.h>
#include "pascal.h"
#include "startup.h"
#include "cc/errno.h"

/* _cc_psp is defined in startup.h */

/* atexit() */
#define CC_ATEXIT_MAX _CC_ATEXIT_MAX

/* Heap */
extern void __far *cc_heap_org;
extern void __far *cc_heap_end;
extern void __far *cc_heap_ptr;
extern void __far *cc_heap_free_list;
extern uint16_t (* __far cc_heap_error) (void);

/* Environment variables */
extern char **cc_environ;

int cc_atexit(void (*__far function)());

int  cc_atoi(const char *src);
long cc_atol(const char *src);

long cc_strtol(const char *nptr, char **endptr, int base);

/* Heap */
uint32_t cc_memavail (void);
uint32_t cc_maxavail (void);
void *cc_malloc(size_t size);
void  cc_free(void *ptr);

void cc_exit(int status);

/* Environment variables */
char   *cc_getenv(const char *name);
int16_t cc_unsetenv(const char *name);
int16_t cc_setenv(const char *name, const char *value, int16_t overwrite);

/*** Initialization ***/

bool cc_heap_init (uint16_t size_min_paras, uint16_t size_max_paras);
bool environ_init(void);

/*** Aliases ***/

#define ATEXIT_MAX CC_ATEXIT_MAX

#define heap_org    cc_heap_org
#define heap_end    cc_heap_end
#define heap_ptr    cc_heap_ptr
#define heap_error  cc_heap_error
#define heap_free_list cc_heap_free_list

#define environ cc_environ

#define atexit cc_atexit

#define atoi cc_atoi
#define atol cc_atol

#define strtol cc_strtol

#define memavail cc_memavail
#define maxavail cc_maxavail
#define malloc cc_malloc
#define free   cc_free

#define exit cc_exit

#define getenv cc_getenv
#define unsetenv cc_unsetenv
#define setenv cc_setenv

#define heap_init cc_heap_init

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux cc_heap_org "*";
#pragma aux cc_heap_end "*";
#pragma aux cc_heap_ptr "*";
#pragma aux cc_heap_error "*";
#pragma aux cc_heap_free_list "*";
#pragma aux cc_environ "*";
#pragma aux cc_atexit "*";
#pragma aux cc_atoi "*";
#pragma aux cc_atol "*";
#pragma aux cc_strtol "*";
#pragma aux cc_memavail "*";
#pragma aux cc_maxavail "*";
#pragma aux cc_malloc "*";
#pragma aux cc_free "*";
#pragma aux cc_exit "*";
#pragma aux cc_getenv "*";
#pragma aux cc_unsetenv "*";
#pragma aux cc_setenv "*";
#pragma aux cc_heap_init "*";
#pragma aux environ_init "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_STDLIB_H_INCLUDED */
