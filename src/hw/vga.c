/* vga.c -- simple VGA library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$hw$vga$*"
#endif

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "debug.h"
#include "hw/vbios.h"
#include "hw/vga.h"

/*
 * "vid_port" - port address for 6845 video controller chip (0x3B4 or 0x3D4).
 */
void vga_wait_sync (uint16_t vid_port, bool disable_irqs)
{
    uint16_t io;
    io = vid_port + 6;  /* port 0x3da: read status register */
    /* bit 0 - if set, display is in vertical or horizontal retrace */
    while (inp (io) & 1);
    if (disable_irqs) _disable ();
    while (!(inp (io) & 1));
}

/*
 * "vid_port" - port address for 6845 video controller chip (0x3B4 or 0x3D4).
 */
void vga_wait_vsync (uint16_t vid_port, bool disable_irqs)
{
    uint16_t io;
    io = vid_port + 6;  /* port 0x3da: read status register */
    /* bit 3 - if set, display is in vertical retrace */
    while (!(inp (io) & 8));
    if (disable_irqs) _disable ();
    while (inp (io) & 8);
}

/*
 * "vid_port" - port address for 6845 video controller chip (0x3B4 or 0x3D4).
 */
void vga_set_text_cursor_position (uint16_t vid_port, uint16_t offset)
{
    /* port 0x3d4: CRT controller register select (write) */
    /* port 0x3d5: CRT controller register read/write */
    outp (vid_port + 0, 0x0e);  /* 0x0e - cursor position (MSB) */
    outp (vid_port + 1, offset >> 8);
    outp (vid_port + 0, 0x0f);  /* 0x0f - cursor position (LSB) */
    outp (vid_port + 1, offset & 0xff);
}

void vga_clear_page_320x200x8 (void *buf, char color)
{
    memset (buf, color, 320*200);
}

/*** Initialization ***/

void __near vga_init(void)
{
}

void __near vga_done(void)
{
}

DEFINE_REGISTRATION(vga, vga_init, vga_done)
