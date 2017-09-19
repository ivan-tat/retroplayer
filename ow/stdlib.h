/* stdlib.h -- declarations for stdlib.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CUSTOM_STDLIB_H
#define CUSTOM_STDLIB_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#include "../pascal/pascal.h"

void *malloc(uint16_t size);
void  free(void *ptr);

#endif  /* CUSTOM_STDLIB_H */
