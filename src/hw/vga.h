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

extern uint16_t PUBLIC_DATA drawseg;

void PUBLIC_CODE vga_wait_vsync(void);
void PUBLIC_CODE vga_clear_page_320x200x8(char c);
void PUBLIC_CODE vga_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t f);
extern void PUBLIC_CODE vga_bar(uint16_t o, uint16_t b, uint16_t l);

#ifdef __WATCOMC__
#pragma aux vga_line    modify [ ax bx cx dx si di es ];
#pragma aux vga_bar     modify [ ax bx cx dx si di es ];
#endif

/* Initialization */

DECLARE_REGISTRATION(vga)

/* Linking */

#endif  /* VGA_H */
