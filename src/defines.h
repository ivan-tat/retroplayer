/* Common defines */

#ifndef _DEFINES_H_INCLUDED
#define _DEFINES_H_INCLUDED 1

#ifdef __WATCOMC__
#pragma once
#define __attribute__(x)
#endif  /* __WATCOMC__ */

#include "conf/defs.h"

#if LINKER_TPC == 1
# define DEFINE_LOCAL_DATA 0
#else   /* LINKER_TPC != 1 */
# define DEFINE_LOCAL_DATA 1
#endif  /* LINKER_TPC != 1 */

#if SYSDEBUG != 1
# undef  SYSDEBUG
# define SYSDEBUG 0
# undef  SYSDEBUG_IOBUF
# define SYSDEBUG_IOBUF 0
# undef  SYSDEBUG_ENV
# define SYSDEBUG_ENV 0
#endif  /* SYSDEBUG != 1 */

#if DEBUG != 1
# undef  DEBUG
# define DEBUG 0
# undef  DEBUG_WRITE_LOG
# define DEBUG_WRITE_LOG 0
# undef  DEBUG_FILE_S3M_LOAD
# define DEBUG_FILE_S3M_LOAD 0
# undef  DEBUG_WRITE_OUTPUT_STREAM
# define DEBUG_WRITE_OUTPUT_STREAM 0
#endif  /* DEBUG != 1 */

#define CRLF "\r\n"
//#define CRLF "\n"

#ifdef __WATCOMC__
# ifndef __noreturn
#  define __noreturn __declspec (noreturn)
# endif /* !__noreturn */
#else   /* !__WATCOMC__ */
# ifndef __noreturn
#  define __noreturn
# endif /* !__noreturn */
#endif  /* !__WATCOMC__ */

#endif  /* !_DEFINES_H_INCLUDED */
