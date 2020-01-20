/* errno.c -- part of custom "errno" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$errno$errno$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "pascal.h"
#include "cc/errno.h"

#if DEFINE_LOCAL_DATA == 1

int16_t cc_errno = EZERO;

#endif  /* DEFINE_LOCAL_DATA == 1 */
