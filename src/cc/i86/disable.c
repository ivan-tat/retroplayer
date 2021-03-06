/* disable.c -- part of custom "i86" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$i86$disable$*"
#endif  /* __WATCOMC__ */

#include "cc/i86.h"

#ifndef __WATCOMC__

void _cc_disable(void)
{
    _asm "cli";
}

#endif  /* !__WATCOMC__ */
