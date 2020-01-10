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

void vga_wait_sync (uint16_t vid_port, bool disable_irqs);
void vga_wait_vsync (uint16_t vid_port, bool disable_irqs);
void vga_set_text_cursor_position (uint16_t vid_port, uint16_t offset);
void vga_clear_page_320x200x8 (void *buf, char color);

extern void __far __pascal vga_line (void *buf, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);
extern void __far __pascal vga_bar (void *buf, uint16_t o, uint16_t b, uint16_t l);

/*** Initialization ***/

DECLARE_REGISTRATION (vga)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux vga_wait_sync "*";
#pragma aux vga_wait_vsync "*";
#pragma aux vga_set_text_cursor_position "*";
#pragma aux vga_clear_page_320x200x8 "*";

#pragma aux vga_line "*" modify [ ax bx cx dx si di es ];
#pragma aux vga_bar  "*" modify [ ax bx cx dx si di es ];

#pragma aux register_vga "*";
#pragma aux unregister_vga "*";

#endif  /* __WATCOMC__ */

#endif  /* VGA_H */
