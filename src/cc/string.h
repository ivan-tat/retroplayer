/* string.h -- declarations for custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_STRING_H_INCLUDED
#define _CC_STRING_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include "pascal.h"

#ifndef NULL
#define NULL ((void *)0)
#endif  /* !NULL */

int    cc_memcmp(const void *s1, const void *s2, size_t n);
void  *cc_memcpy(void *dest, const void *src, size_t n);
void  *cc_memset(void *s, int c, size_t n);
char  *cc_strchr(const char *s, int c);
int    cc_strcmp(const char *s1, const char *s2);
int    cc_stricmp(const char *s1, const char *s2);
size_t cc_strlen(const char *s);
int    cc_strncmp(const char *s1, const char *s2, size_t n);
char  *cc_strncpy(char *dest, const char *src, size_t n);
int    cc_strnicmp(const char *s1, const char *s2, size_t n);

/*** Aliases ***/

#define memcmp   cc_memcmp
#define memcpy   cc_memcpy
#define memset   cc_memset
#define strchr   cc_strchr
#define strcmp   cc_strcmp
#define stricmp  cc_stricmp
#define strlen   cc_strlen
#define strncmp  cc_strncmp
#define strncpy  cc_strncpy
#define strnicmp cc_strnicmp

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux cc_memcmp "*";
#pragma aux cc_memcpy "*";
#pragma aux cc_memset "*";
#pragma aux cc_strchr "*";
#pragma aux cc_strcmp "*";
#pragma aux cc_stricmp "*";
#pragma aux cc_strlen "*";
#pragma aux cc_strncmp "*";
#pragma aux cc_strncpy "*";
#pragma aux cc_strnicmp "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_STRING_H_INCLUDED */
