/* crwdata.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$crwdata$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "pascal.h"
#include "cc/stdlib.h"

#if DEFINE_LOCAL_DATA == 1

//static uint16_t _cc_psp = 0;  /* defined in startup.c */

#endif  /* DEFINE_LOCAL_DATA == 1 */
