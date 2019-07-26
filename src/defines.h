/* Common defines */

#ifndef DEFINES_H
#define DEFINES_H 1

#ifdef __WATCOMC__
#pragma once
#define __attribute__(x)
#endif

#undef DEBUG_WRITE_LOG
#undef DEBUG_FILE_S3M_LOAD
#undef DEBUG_WRITE_OUTPUT_STREAM

#if DEBUG == 1
# define DEBUG_WRITE_LOG 1
# define DEBUG_FILE_S3M_LOAD 1
# define DEBUG_WRITE_OUTPUT_STREAM 0
#else
# undef  DEBUG
# define DEBUG 0
# define DEBUG_WRITE_LOG 0
# define DEBUG_FILE_S3M_LOAD 0
# define DEBUG_WRITE_OUTPUT_STREAM 0
#endif

#if LINKER_TPC != 1
# define DEFINE_LOCAL_DATA 1
#endif
//#define USE_INTRINSICS 1

#define CONFIG_VBIOS_ENABLE_LIGHT_PEN 1

#define CRLF "\r\n"
//#define CRLF "\n"

#endif  /* DEFINES_H */
