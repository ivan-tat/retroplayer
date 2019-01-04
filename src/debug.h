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

#include "cc/stdio.h"

#define DBGLOG_MSG  0
#define DBGLOG_INFO 1
#define DBGLOG_WARN 2
#define DBGLOG_ERR  3

void __far _DEBUG_LOG(const int type, const char *file, int line, const char *method, const char *format, ...);
void __far _DEBUG_BEGIN(const char *file, int line, const char *method);
void __far _DEBUG_END(const char *file, int line, const char *method);
void __far _DEBUG_FAIL(const char *file, int line, const char *method, const char *msg);
void __far _DEBUG_SUCCESS(const char *file, int line, const char *method);
void __far _DEBUG_REG(const char *file, const char *method, const char *name);
void __far _DEBUG_UNREG(const char *file, const char *method, const char *name);
void __far _DEBUG_get_xnum(uint32_t value, char len, char *dest);
void __far _DEBUG_get_xline(void *buf, uint8_t size, uint8_t max, char *dest);
void __far _DEBUG_dump_mem(void *buf, unsigned size, const char *padstr);

#ifdef DEBUG
 #define DEBUG_LOG(type, method, text)         _DEBUG_LOG(type, __FILE__, __LINE__, method, text)
 #define DEBUG_LOG_(type, method, format, ...) _DEBUG_LOG(type, __FILE__, __LINE__, method, format, __VA_ARGS__)
 #define DEBUG_MSG(method, text)               _DEBUG_LOG(DBGLOG_MSG, __FILE__, __LINE__, method, text)
 #define DEBUG_MSG_(method, format, ...)       _DEBUG_LOG(DBGLOG_MSG, __FILE__, __LINE__, method, format, __VA_ARGS__)
 #define DEBUG_INFO(method, text)              _DEBUG_LOG(DBGLOG_INFO, __FILE__, __LINE__, method, text)
 #define DEBUG_INFO_(method, format, ...)      _DEBUG_LOG(DBGLOG_INFO, __FILE__, __LINE__, method, format, __VA_ARGS__)
 #define DEBUG_WARN(method, text)              _DEBUG_LOG(DBGLOG_WARN, __FILE__, __LINE__, method, text)
 #define DEBUG_WARN_(method, format, ...)      _DEBUG_LOG(DBGLOG_WARN, __FILE__, __LINE__, method, format, __VA_ARGS__)
 #define DEBUG_ERR(method, text)               _DEBUG_LOG(DBGLOG_ERR, __FILE__, __LINE__, method, text)
 #define DEBUG_ERR_(method, format, ...)       _DEBUG_LOG(DBGLOG_ERR, __FILE__, __LINE__, method, format, __VA_ARGS__)
 #define DEBUG_BEGIN(method)                   _DEBUG_BEGIN(__FILE__, __LINE__, method);
 #define DEBUG_END(method)                     _DEBUG_END(__FILE__, __LINE__, method);
 #define DEBUG_FAIL(method, text)              _DEBUG_FAIL(__FILE__, __LINE__, method, text);
 #define DEBUG_SUCCESS(method)                 _DEBUG_SUCCESS(__FILE__, __LINE__, method);
 #define DEBUG_REG(method, name)               _DEBUG_REG(__FILE__, method, name)
 #define DEBUG_UNREG(method, name)             _DEBUG_UNREG(__FILE__, method, name)
 #define DEBUG_get_xnum(value, len, dest)      _DEBUG_get_xnum(value, len, dest)
 #define DEBUG_get_xline(buf, len, max, dest)  _DEBUG_get_xline(buf, len, max, dest)
 #define DEBUG_dump_mem(buf, size, padstr)     _DEBUG_dump_mem(buf, size, padstr)
#else
 #define DEBUG_LOG(type, method, text)
 #define DEBUG_LOG_(type, method, format, ...)
 #define DEBUG_MSG(method, text)
 #define DEBUG_MSG_(method, format, ...)
 #define DEBUG_INFO(method, text)
 #define DEBUG_INFO_(method, format, ...)
 #define DEBUG_WARN(method, text)
 #define DEBUG_WARN_(method, format, ...)
 #define DEBUG_ERR(method, text)
 #define DEBUG_ERR_(method, format, ...)
 #define DEBUG_BEGIN(method)
 #define DEBUG_END(method)
 #define DEBUG_FAIL(method, text)
 #define DEBUG_SUCCESS(method)
 #define DEBUG_REG(method, name)
 #define DEBUG_UNREG(method, name)
 #define DEBUG_get_xnum(value, len, dest)
 #define DEBUG_get_xline(buf, len, max, dest)
 #define DEBUG_dump_mem(buf, size, padstr)
#endif

void __far __pascal Debug_Msg (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Info (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Warn (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Err (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Begin (const char *file, const char *method);
void __far __pascal Debug_End (const char *file, const char *method);
void __far __pascal Debug_Fail (const char *file, const char *method, const char *msg);
void __far __pascal Debug_Success (const char *file, const char *method);

/*** Variables ***/

extern FILE *debuglogfile;

/*** Initialization ***/

DECLARE_REGISTRATION (debug)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _DEBUG_LOG "*";
#pragma aux _DEBUG_BEGIN "*";
#pragma aux _DEBUG_END "*";
#pragma aux _DEBUG_FAIL "*";
#pragma aux _DEBUG_SUCCESS "*";
#pragma aux _DEBUG_REG "*";
#pragma aux _DEBUG_UNREG "*";
#pragma aux _DEBUG_get_xnum "*";
#pragma aux _DEBUG_get_xline "*";
#pragma aux _DEBUG_dump_mem "*";

#pragma aux debuglogfile "*";

#pragma aux register_debug "*";
#pragma aux unregister_debug "*";

#endif  /* __WATCOMC__ */

#endif  /* DEBUG_H */
