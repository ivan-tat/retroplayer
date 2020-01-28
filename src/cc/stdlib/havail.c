/* havail.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$havail$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/i86.h"
#include "cc/stdlib.h"
#include "cc/stdlib/heap.h"
#include "cc/stdlib/havail.h"

/*
 * Used by memavail() and maxavail().
 */
uint32_t _cc_heap_avail_func (void __far * (*function) (void __far *size, struct cc_heap_free_rec_t __far *rec, uint16_t end_seg))
{
    #pragma pack(push, 1);
    union {
        uint32_t d;
        uint16_t w[2];
        void __far *p;
    } x;
    #pragma pack(pop);
    x.d = (uint32_t) cc_heap_end - (uint32_t) cc_heap_ptr;
    x.w[0] = x.w[0] & 0xf;
    x.p = function (x.p, (struct cc_heap_free_rec_t __far *) cc_heap_free_list, FP_SEG (cc_heap_ptr));
    return ((uint32_t) x.w[1] << 4) + x.w[0];
}
