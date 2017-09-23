/* loads3m.h -- declarations for loads3m.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef LOADS3M_H
#define LOADS3M_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

extern void PUBLIC_CODE unpackPattern(uint8_t *src, uint8_t *dst, uint8_t maxrow, uint8_t maxchn);

#endif  /* LOADS3M_H */
