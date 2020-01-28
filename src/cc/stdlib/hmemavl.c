/* hmemavl.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$hmemavl$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/i86.h"
#include "cc/stdlib.h"
#include "cc/stdlib/heap.h"
#include "cc/stdlib/havail.h"

/*
 * Returns total free heap size as far pointer.
 */
void __far * _memavail (void __far *size, struct cc_heap_free_rec_t __far *rec, uint16_t end_seg)
{
    #pragma pack(push, 1);
    union
    {
        uint32_t d;
        uint16_t w[2];
        void __far *p;
    } x;
    #pragma pack(pop);
    while (FP_SEG (rec) != end_seg)
    {
        x.d = (uint32_t) FP_OFF (size) + FP_OFF (rec->size);
        if (x.w[1]) x.w[1] = 0x1000;
        x.w[1] += FP_SEG (size);
        size = x.p;
        rec = rec->next;
    }
    return size;
}

uint32_t cc_memavail (void)
{
    return _cc_heap_avail_func (&_memavail);
}
