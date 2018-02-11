/* vga.h -- declarations for vga.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef VGA_H
#define VGA_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

#include "pascal.h"

void PUBLIC_CODE vbios_set_mode(uint8_t mode);
void PUBLIC_CODE vbios_set_cursor_shape(uint8_t start, uint8_t stop);

void PUBLIC_CODE vga_wait_vsync(void);

/* Initialization */

DECLARE_REGISTRATION(vga)

#endif  /* VGA_H */
