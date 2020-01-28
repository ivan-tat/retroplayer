/* heap.h -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_STDLIB_HEAP_H_INCLUDED
#define _CC_STDLIB_HEAP_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

#pragma pack(push, 1);
typedef struct cc_heap_free_rec_t
{
    struct cc_heap_free_rec_t __far *next;
    void __far *size;
};  /* 8 bytes */
#pragma pack(pop);

/*** Aliases ***/

/*** Linking ***/

#ifdef __WATCOMC__
#endif  /* __WATCOMC__ */

#endif  /* !_CC_STDLIB_HEAP_H_INCLUDED */
