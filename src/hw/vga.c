/* vga.c -- simple VGA library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "debug.h"
#include "hw/vbios.h"

#include "hw/vga.h"

void PUBLIC_CODE vga_wait_vsync(void)
{
    _disable();

    while ((inp(0x3da) & 8) == 0);
    while ((inp(0x3da) & 8) != 0);

    _enable();
}

void PUBLIC_CODE vga_clear_page_320x200x8(char c)
{
    memset(MK_FP(drawseg, 0), c, 320*200);
}

/* Initialization */

void __near vga_init(void)
{
}

void __near vga_done(void)
{
}

DEFINE_REGISTRATION(vga, vga_init, vga_done)
