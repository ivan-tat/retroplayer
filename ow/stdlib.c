/* stdlib.c -- custom standard library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#include "../pascal/pascal.h"
#include "stdlib.h"

void *malloc(uint16_t size)
{
    uint16_t n;
    void *data;
    n = size + sizeof(uint16_t);
    pascal_getmem(&data, n);
    if (data)
    {
        *((uint16_t *)data) = n;
        data = MK_FP(FP_SEG(data), FP_OFF(data) + sizeof(uint16_t));
    };
    return data;
}

void free(void *ptr)
{
    void *data;
    data = MK_FP(FP_SEG(ptr), FP_OFF(ptr) - sizeof(uint16_t));
    pascal_freemem(data, *((uint16_t *)data));
}
