/* vga.c -- simple VGA library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "debug.h"

#include "hw/vga.h"

void PUBLIC_CODE vbios_set_mode(uint8_t mode)
{
    union REGPACK regs;

    regs.h.ah = 0;
    regs.h.al = mode;
    intr(0x10, &regs);
}

void PUBLIC_CODE vbios_set_cursor_shape(uint8_t start, uint8_t stop)
{
    union REGPACK regs;

    regs.h.ah = 1;
    regs.h.cl = stop;
    regs.h.ch = start;
    intr(0x10, &regs);
}

void PUBLIC_CODE vga_wait_vsync(void)
{
    _disable();

    while ((inp(0x3da) & 8) == 0);
    while ((inp(0x3da) & 8) != 0);

    _enable();
}

/* Initialization */

void __near vga_init(void)
{
}

void __near vga_done(void)
{
}

DEFINE_REGISTRATION(vga, vga_init, vga_done)
