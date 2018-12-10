/* common.h -- declarations for common.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef COMMON_H
#define COMMON_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

void *__new(size_t size);
void  __delete(void **p);

void *__far __pascal _new_ (uint16_t size);
void  __far __pascal _delete_ (void **p);

#define _new(t)     (t *)__new(sizeof(t))
#define _delete(p)  __delete((void **)&(p))

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux __new "*";
#pragma aux __delete "*";

#pragma aux _new_ "*";
#pragma aux _delete_ "*";

#endif  /* __WATCOMC__ */

#endif  /* COMMON_H */
