/* atexit.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$atexit$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "startup.h"
#include "cc/i86.h"
#include "cc/stdlib.h"

int cc_atexit(void (*__far function)())
{
    if (_cc_ExitCount < CC_ATEXIT_MAX)
    {
        _cc_ExitList[_cc_ExitCount] = function;
        _cc_ExitCount++;
        return 0;
    }
    else
        return 1;
}
