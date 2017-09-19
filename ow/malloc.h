/* malloc.h -- declarations for malloc.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CUSTOM_MALLOC_H
#define CUSTOM_MALLOC_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#include "../pascal/pascal.h"

uint32_t PUBLIC_CODE _memmax(void);

#endif  /* CUSTOM_MALLOC_H */
