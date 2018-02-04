// Common defines

#ifndef DEFINES_H
#define DEFINES_H 1

#ifdef __WATCOMC__
#pragma once
#endif

#define DEBUG 1
#define DEBUG_FILE_S3M_LOAD 1
#define DEBUG_WRITE_OUTPUT_STREAM 0
//#define DEFINE_LOCAL_DATA 1
//#define USE_INTRINSICS 1

#define CRLF "\r\n"
//#define CRLF "\n"

#define PUBLIC_DATA __pascal
#define PUBLIC_CODE __far __pascal
#define EXTERN_LINK extern

#endif  /* DEFINES_H */
