/* common.c -- simple common library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>
#include "cc/i86.h"
#include "cc/dos.h"

#include "common.h"

void *__new(uint16_t size)
{
    uint16_t seg;

    if (!_dos_allocmem(_dos_para(size), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void __delete(void **p)
{
    if (p)
        if (*p)
        {
            _dos_freemem(FP_SEG(*p));
            *p = NULL;
        }
}

void *PUBLIC_CODE _new_(uint16_t size)
{
    return __new(size);
}

void PUBLIC_CODE _delete_(void **p)
{
    __delete(p);
}
