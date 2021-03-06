/* ffree.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$ffree$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/stdlib.h"

extern void __far __pascal pascal_freemem(void *p, uint16_t size);

#ifdef __WATCOMC__
#pragma aux pascal_freemem "*" modify [ ax bx cx dx si di es ];
#endif  /* __WATCOMC__ */

void cc_free(void *ptr)
{
    void *data;
    data = MK_FP(FP_SEG(ptr), FP_OFF(ptr) - sizeof(size_t));
    pascal_freemem(data, *((size_t *)data));
}
