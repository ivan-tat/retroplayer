/* hmaxavl.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$hmaxavl$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/i86.h"
#include "cc/stdlib.h"
#include "cc/stdlib/heap.h"
#include "cc/stdlib/havail.h"

/*
 * Returns maximum free heap's chunk size as far pointer.
 */
void __far * __far _maxavail (void __far *size, struct cc_heap_free_rec_t __far *rec, uint16_t end_seg)
{
    while (FP_SEG (rec) != end_seg)
    {
        if ((uint32_t) size < (uint32_t) rec->size) size = rec->size;
        rec = rec->next;
    }
    return size;
}

uint32_t cc_maxavail (void)
{
    return _cc_heap_avail_func (&_maxavail);
}
