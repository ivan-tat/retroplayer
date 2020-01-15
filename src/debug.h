/* debug.h -- declarations for debug.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DEBUG_H
#define DEBUG_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdarg.h>
#include "commdbg.h"
#include "cc/stdio.h"

void __far _DEBUG_LOG(const int type, const char *file, int line, const char *method, const char *format, ...);
void __far _DEBUG_BEGIN(const char *file, int line, const char *method);
void __far _DEBUG_END(const char *file, int line, const char *method);
void __far _DEBUG_SUCCESS(const char *file, int line, const char *method);
void __far _DEBUG_REG(const char *file, const char *method, const char *name);
void __far _DEBUG_UNREG(const char *file, const char *method, const char *name);
void __far _DEBUG_dump_mem(void *buf, unsigned size, const char *padstr);

#if DEBUG == 1
 #define DEBUG_LOG(type, text)             _DEBUG_LOG (type, __FILE__, __LINE__, __func__, "%s", text)
 #define DEBUG_LOG_(type, format, ...)     _DEBUG_LOG (type, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define DEBUG_MSG(text)                   _DEBUG_LOG (DBGLOG_MSG, __FILE__, __LINE__, __func__, "%s", text)
 #define DEBUG_MSG_(format, ...)           _DEBUG_LOG (DBGLOG_MSG, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define DEBUG_INFO(text)                  _DEBUG_LOG (DBGLOG_INFO, __FILE__, __LINE__, __func__, "%s", text)
 #define DEBUG_INFO_(format, ...)          _DEBUG_LOG (DBGLOG_INFO, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define DEBUG_WARN(text)                  _DEBUG_LOG (DBGLOG_WARN, __FILE__, __LINE__, __func__, "%s", text)
 #define DEBUG_WARN_(format, ...)          _DEBUG_LOG (DBGLOG_WARN, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define DEBUG_BEGIN()                     _DEBUG_BEGIN (__FILE__, __LINE__, __func__)
 #define DEBUG_END()                       _DEBUG_END (__FILE__, __LINE__, __func__)
 #define DEBUG_SUCCESS()                   _DEBUG_SUCCESS (__FILE__, __LINE__, __func__)
 #define DEBUG_ERR(text)                   _DEBUG_LOG (DBGLOG_ERR, __FILE__, __LINE__, __func__, "%s", text)
 #define DEBUG_ERR_(format, ...)           _DEBUG_LOG (DBGLOG_ERR, __FILE__, __LINE__, __func__, format, __VA_ARGS__)
 #define DEBUG_REG(method, name)           _DEBUG_REG (__FILE__, method, name)
 #define DEBUG_UNREG(method, name)         _DEBUG_UNREG (__FILE__, method, name)
 #define DEBUG_dump_mem(buf, size, padstr) _DEBUG_dump_mem (buf, size, padstr)
#else
 #define DEBUG_LOG(type, text)
 #define DEBUG_LOG_(type, format, ...)
 #define DEBUG_MSG(text)
 #define DEBUG_MSG_(format, ...)
 #define DEBUG_INFO(text)
 #define DEBUG_INFO_(format, ...)
 #define DEBUG_WARN(text)
 #define DEBUG_WARN_(format, ...)
 #define DEBUG_BEGIN()
 #define DEBUG_END()
 #define DEBUG_SUCCESS()
 #define DEBUG_ERR(text)
 #define DEBUG_ERR_(format, ...)
 #define DEBUG_REG(method, name)
 #define DEBUG_UNREG(method, name)
 #define DEBUG_dump_mem(buf, size, padstr)
#endif

void __far __pascal Debug_Msg (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Info (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Warn (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Begin (const char *file, const char *method);
void __far __pascal Debug_End (const char *file, const char *method);
void __far __pascal Debug_Success (const char *file, const char *method);
void __far __pascal Debug_Err (const char *file, const char *method, const char *msg);

/*** Variables ***/

#if DEBUG_WRITE_LOG == 1

extern FILE *debuglogfile;

#endif  /* DEBUG_WRITE_LOG */

/*** Initialization ***/

DECLARE_REGISTRATION (debug)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _DEBUG_LOG "*";
#pragma aux _DEBUG_BEGIN "*";
#pragma aux _DEBUG_END "*";
#pragma aux _DEBUG_SUCCESS "*";
#pragma aux _DEBUG_REG "*";
#pragma aux _DEBUG_UNREG "*";
#pragma aux _DEBUG_dump_mem "*";

#if DEBUG_WRITE_LOG == 1

#pragma aux debuglogfile "*";

#endif  /* DEBUG_WRITE_LOG */

#pragma aux register_debug "*";
#pragma aux unregister_debug "*";

#endif  /* __WATCOMC__ */

#endif  /* DEBUG_H */
