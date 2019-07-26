/* errno.c -- part of custom "errno" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$errno$errno$*"
#endif

#include <stdint.h>
#include "pascal.h"
#include "cc/errno.h"

#ifdef DEFINE_LOCAL_DATA

int16_t cc_errno = EZERO;

#endif
