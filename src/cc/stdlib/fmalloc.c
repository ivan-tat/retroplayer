/* fmalloc.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$fmalloc$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/stdlib.h"

extern void __far __pascal pascal_getmem(void **p, uint16_t size);

#ifdef __WATCOMC__
#pragma aux pascal_getmem "*" modify [ ax bx cx dx si di es ];
#endif  /* __WATCOMC__ */

void *cc_malloc(size_t size)
{
    size_t n;
    void *data;
    n = size + sizeof(size_t);
    pascal_getmem(&data, n);
    if (data)
    {
        *((size_t *)data) = n;
        data = MK_FP(FP_SEG(data), FP_OFF(data) + sizeof(size_t));
    }
    return data;
}
