/* havail.h -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_STDLIB_HAVAIL_H_INCLUDED
#define _CC_STDLIB_HAVAIL_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdint.h>

uint32_t _cc_heap_avail_func (void __far * (*function) (void __far *size, struct cc_heap_free_rec_t __far *rec, uint16_t end_seg));

/*** Aliases ***/

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_heap_avail_func "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_STDLIB_HAVAIL_H_INCLUDED */
