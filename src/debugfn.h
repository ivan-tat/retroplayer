/* debugfn.h -- declarations for debugfn.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DEBUGFN_H
#define DEBUGFN_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

extern void PUBLIC_CODE _DEBUG_print(const char *str);

#ifdef __WATCOMC__
#pragma aux _DEBUG_print modify [ax bx cx dx si di es];
#endif

void __far _DEBUG_get_xnum(uint32_t value, char len, char *dest);
void __far _DEBUG_get_xline(void *buf, uint8_t size, uint8_t max, char *dest);
void __far _DEBUG_dump_mem(void *buf, unsigned size, const char *padstr);

#ifdef DEBUG
 #define DEBUG_print(str)                     _DEBUG_print(str)
 #define DEBUG_get_xnum(value, len, dest)     _DEBUG_get_xnum(value, len, dest)
 #define DEBUG_get_xline(buf, len, max, dest) _DEBUG_get_xline(buf, len, max, dest)
 #define DEBUG_dump_mem(buf, size, padstr)    _DEBUG_dump_mem(buf, size, padstr)
#else
 #define DEBUG_print(str)
 #define DEBUG_get_xnum(value, len, dest)
 #define DEBUG_get_xline(buf, len, max, dest)
 #define DEBUG_dump_mem(buf, size, padstr)
#endif

/* Linking */

#ifdef __WATCOMC__
#pragma aux _DEBUG_get_xnum "*";
#pragma aux _DEBUG_get_xline "*";
#pragma aux _DEBUG_dump_mem "*";
#endif

#endif  /* DEBUGFN_H */
