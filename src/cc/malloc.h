/* malloc.h -- declarations for custom "malloc" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_MALLOC_H_INCLUDED
#define _CC_MALLOC_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "pascal.h"

uint32_t _memmax(void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _memmax "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_MALLOC_H_INCLUDED */
