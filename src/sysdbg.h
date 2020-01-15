/* sysdbg.h -- declarations for sysdbg.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef SYSDBG_H
#define SYSDBG_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdarg.h>
#include "commdbg.h"

void __far _SYSDEBUG_LOG (const int type, const char *file, int line, const char *method, const char *format, ...);
void __far _SYSDEBUG_BEGIN (const char *file, int line, const char *method);
void __far _SYSDEBUG_END (const char *file, int line, const char *method);
void __far _SYSDEBUG_SUCCESS (const char *file, int line, const char *method);
void __far _SYSDEBUG_dump_mem (void *buf, unsigned size, const char *padstr);

#if SYSDEBUG == 1
 #define SYSDEBUG_LOG(type, text)             _SYSDEBUG_LOG (type, __FILE__, __LINE__, __func__, "%s", text)
 #define SYSDEBUG_LOG_(type, format, ...)     _SYSDEBUG_LOG (type, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define SYSDEBUG_MSG(text)                   _SYSDEBUG_LOG (DBGLOG_MSG, __FILE__, __LINE__, __func__, "%s", text)
 #define SYSDEBUG_MSG_(format, ...)           _SYSDEBUG_LOG (DBGLOG_MSG, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define SYSDEBUG_MSG(text)                   _SYSDEBUG_LOG (DBGLOG_MSG, __FILE__, __LINE__, __func__, "%s", text)
 #define SYSDEBUG_MSG_(format, ...)           _SYSDEBUG_LOG (DBGLOG_MSG, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define SYSDEBUG_INFO(text)                  _SYSDEBUG_LOG (DBGLOG_INFO, __FILE__, __LINE__, __func__, "%s", text)
 #define SYSDEBUG_INFO_(format, ...)          _SYSDEBUG_LOG (DBGLOG_INFO, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define SYSDEBUG_WARN(text)                  _SYSDEBUG_LOG (DBGLOG_WARN, __FILE__, __LINE__, __func__, "%s", text)
 #define SYSDEBUG_WARN_(format, ...)          _SYSDEBUG_LOG (DBGLOG_WARN, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define SYSDEBUG_BEGIN()                     _SYSDEBUG_BEGIN (__FILE__, __LINE__, __func__)
 #define SYSDEBUG_END()                       _SYSDEBUG_END (__FILE__, __LINE__, __func__)
 #define SYSDEBUG_SUCCESS()                   _SYSDEBUG_SUCCESS (__FILE__, __LINE__, __func__)
 #define SYSDEBUG_ERR(text)                   _SYSDEBUG_LOG (DBGLOG_ERR, __FILE__, __LINE__, __func__, "%s", text)
 #define SYSDEBUG_ERR_(format, ...)           _SYSDEBUG_LOG (DBGLOG_ERR, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define SYSDEBUG_dump_mem(buf, size, padstr) _SYSDEBUG_dump_mem (buf, size, padstr)
#else
 #define SYSDEBUG_LOG(type, text)
 #define SYSDEBUG_LOG_(type, format, ...)
 #define SYSDEBUG_MSG(text)
 #define SYSDEBUG_MSG_(format, ...)
 #define SYSDEBUG_MSG(text)
 #define SYSDEBUG_MSG_(format, ...)
 #define SYSDEBUG_INFO(text)
 #define SYSDEBUG_INFO_(format, ...)
 #define SYSDEBUG_WARN(text)
 #define SYSDEBUG_WARN_(format, ...)
 #define SYSDEBUG_BEGIN()
 #define SYSDEBUG_END()
 #define SYSDEBUG_SUCCESS()
 #define SYSDEBUG_ERR(text)
 #define SYSDEBUG_ERR_(format, ...)
 #define SYSDEBUG_dump_mem(buf, size, padstr)
#endif

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _SYSDEBUG_LOG "*";
#pragma aux _SYSDEBUG_BEGIN "*";
#pragma aux _SYSDEBUG_END "*";
#pragma aux _SYSDEBUG_SUCCESS "*";
#pragma aux _SYSDEBUG_dump_mem "*";

#endif  /* __WATCOMC__ */

#endif  /* SYSDBG_H */
