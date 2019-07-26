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

extern uint16_t drawseg;

void __far vga_wait_vsync (void);
void __far vga_clear_page_320x200x8 (char c);

extern void __far __pascal vga_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t f);
extern void __far __pascal vga_bar(uint16_t o, uint16_t b, uint16_t l);

/*** Initialization ***/

DECLARE_REGISTRATION (vga)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux drawseg "*";

#pragma aux vga_wait_vsync "*";
#pragma aux vga_clear_page_320x200x8 "*";

#pragma aux vga_line "*" modify [ ax bx cx dx si di es ];
#pragma aux vga_bar  "*" modify [ ax bx cx dx si di es ];

#pragma aux register_vga "*";
#pragma aux unregister_vga "*";

#endif  /* __WATCOMC__ */

#endif  /* VGA_H */
