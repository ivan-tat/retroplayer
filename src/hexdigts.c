/* hexdigts.c -- hexadecimal digits, shared across multiple source files.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$hexdigts$*"
#endif  /* __WATCOMC__ */

#include "hexdigts.h"

#if DEFINE_LOCAL_DATA == 1

const char HEXDIGITS[16] = "0123456789ABCDEF";

#endif  /* DEFINE_LOCAL_DATA == 1 */
