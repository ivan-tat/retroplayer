/* malloc.c -- custom memory allocation library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "pascal/pascal.h"
#include "ow/dos_.h"
#include "ow/malloc.h"

uint32_t PUBLIC_CODE _memmax(void)
{
    uint16_t max;
    _dos_allocmem(0xffff, &max);
    return (uint32_t)max << 4;
};
