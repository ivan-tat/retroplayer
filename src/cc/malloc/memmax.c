/* memmax.c -- part of custom "malloc" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$malloc$memmax$*"
#endif

#include <stdint.h>
#include "pascal.h"
#include "cc/dos.h"
#include "cc/malloc.h"

uint32_t _memmax(void)
{
    uint16_t max;
    _dos_allocmem(0xffff, &max);
    return (uint32_t)max << 4;
}
