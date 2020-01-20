/* commdbg.h -- declarations for commdbg.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _COMMDBG_H_INCLUDED
#define _COMMDBG_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdint.h>

#define DBGLOG_MSG  0
#define DBGLOG_INFO 1
#define DBGLOG_WARN 2
#define DBGLOG_ERR  3

void __far _DEBUG_get_xnum (uint32_t value, char len, char *dest);
void __far _DEBUG_get_xline (void *buf, uint8_t size, uint8_t max, char *dest);

#if DEBUG == 1
 #define DEBUG_get_xnum(value, len, dest)     _DEBUG_get_xnum (value, len, dest)
 #define DEBUG_get_xline(buf, len, max, dest) _DEBUG_get_xline (buf, len, max, dest)
#else   /* DEBUG != 1 */
 #define DEBUG_get_xnum(value, len, dest)
 #define DEBUG_get_xline(buf, len, max, dest)
#endif  /* DEBUG != 1 */

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _DEBUG_get_xnum "*";
#pragma aux _DEBUG_get_xline "*";

#endif  /* __WATCOMC__ */

#endif  /* !_COMMDBG_H_INCLUDED */
