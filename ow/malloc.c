/* malloc.c -- custom memory allocation library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "../pascal/pascal.h"
#include "dos_.h"
#include "malloc.h"

uint32_t PUBLIC_CODE _memmax(void)
{
    uint16_t max;
    _dos_allocmem(0xffff, &max);
    return (uint32_t)max << 4;
};
