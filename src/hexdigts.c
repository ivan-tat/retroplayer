/* hexdigts.c -- hexadecimal digits, shared across multiple source files.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$hexdigts$*"
#endif

#include "hexdigts.h"

#ifdef DEFINE_LOCAL_DATA

const char HEXDIGITS[16] = "0123456789ABCDEF";

#endif  /* DEFINE_LOCAL_DATA */
