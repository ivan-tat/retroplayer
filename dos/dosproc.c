/* dosproc.c -- DOS memory management.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"
#include "..\watcomc\dos_.h"
#include "dosproc.h"

bool PUBLIC_CODE getdosmem(void **p, uint32_t size)
{
    uint16_t seg;
    if (_dos_allocmem(_dos_para(size), &seg))
        return false;
    else
    {
        *p = MK_FP(seg, 0);
        return true;
    }
}

void PUBLIC_CODE freedosmem(void **p)
{
    _dos_freemem(FP_SEG(*p));
    *p = (void *)0;
}

bool PUBLIC_CODE setsize(void *p, uint32_t size)
{
    uint16_t max;
    return !_dos_setblock(_dos_para(size), FP_SEG(p), &max);
}

uint32_t PUBLIC_CODE getfreesize(void)
{
    uint16_t size;
    _dos_allocmem(0xffff, &size);
    return (uint32_t)size << 4;
}
