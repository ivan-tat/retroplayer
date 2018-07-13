/* vbios.h -- declarations for vbios.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef VBIOS_H
#define VBIOS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

#include "pascal.h"

void vbios_set_mode(uint8_t mode);
void vbios_set_cursor_shape(uint8_t start, uint8_t stop);

/* Linking */

#ifdef __WATCOMC__
#pragma aux vbios_set_mode "*";
#pragma aux vbios_set_cursor_shape "*";
#endif

#endif  /* VBIOS_H */
