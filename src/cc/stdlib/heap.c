/* heap.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$heap$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/stdlib.h"
#include "cc/stdlib/heap.h"

#if DEFINE_LOCAL_DATA == 1

void __far *cc_heap_org = NULL;
void __far *cc_heap_end = NULL;
void __far *cc_heap_ptr = NULL;
void __far *cc_heap_free_list = NULL;
uint16_t (* __far cc_heap_error) (uint16_t size) = NULL;

#endif  /* DEFINE_LOCAL_DATA == 1 */

uint16_t __far _heap_error_func (uint16_t size)
{
    return 0;
}

/*** Initialization ***/

void __far _heap_done (void);

void __near _heap_init (uint16_t seg_start, uint16_t size_paras)
{
    cc_heap_org = MK_FP (seg_start, 0);
    cc_heap_end = MK_FP (seg_start + size_paras, 0);
    cc_heap_ptr = cc_heap_org;
    cc_heap_free_list = cc_heap_org;
    cc_heap_error = &_heap_error_func;
}

void __far _heap_done (void)
{
    if (cc_heap_org)
        _cc_dos_freemem (FP_SEG (cc_heap_org));
}

bool cc_heap_init (uint16_t size_min_paras, uint16_t size_max_paras)
{
    uint16_t size, seg;

    if (size_min_paras > size_max_paras || size_max_paras == 0)
    {
        cc_errno = CC_EINVAL;
        return false;
    }

    size = size_max_paras;
    if (_cc_dos_allocmem (size, &seg) != CC_EZERO)
    {
        size = seg;
        if (size < size_min_paras)
            return false;
        if (_cc_dos_allocmem (size, &seg) != CC_EZERO)
            return false;
    }

    _heap_init (seg, size);
    cc_atexit (&_heap_done);
    return true;
}
