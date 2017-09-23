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
#endif

void PUBLIC_CODE Debug_Msg(const char *file, const char *method, const char *msg);
void PUBLIC_CODE Debug_Info(const char *file, const char *method, const char *msg);
void PUBLIC_CODE Debug_Warn(const char *file, const char *method, const char *msg);
void PUBLIC_CODE Debug_Err(const char *file, const char *method, const char *msg);
void PUBLIC_CODE Debug_Begin(const char *file, const char *method);
void PUBLIC_CODE Debug_End(const char *file, const char *method);
void PUBLIC_CODE Debug_Fail(const char *file, const char *method, const char *msg);
void PUBLIC_CODE Debug_Success(const char *file, const char *method);

/*** Initialization ***/

DECLARE_REGISTRATION(debug)

#endif  /* DEBUG_H */
