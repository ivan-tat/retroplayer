/* errno.c -- part of custom "errno" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "pascal.h"

#if DEFINE_LOCAL_DATA

static int16_t cc_errno;

#endif
