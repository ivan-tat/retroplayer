/* crwdata.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$crwdata$*"
#endif

#include <stdint.h>
#include "pascal.h"
#include "cc/stdlib.h"

#ifdef DEFINE_LOCAL_DATA

const static uint16_t cc_psp = 0;

#endif  /* DEFINE_LOCAL_DATA */
