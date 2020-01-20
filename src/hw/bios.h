/* bios.h -- simple BIOS support library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _BIOS_H_INCLUDED
#define _BIOS_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "cc/i86.h"

#define Seg0040 0x0040

#pragma pack(push, 1);
typedef struct
{   /* offset, size */
    /* 0x0000, -  */ uint8_t  unused_0_48 [0x0048-0x0000+1];
    /* 0x0049, 1  */ uint8_t  active_video_mode;
    /* 0x004a, 2  */ uint16_t text_screen_width; /* Screen width in text columns */
    /* 0x004c, -  */ uint8_t  unused_4c_4f [0x004f-0x004c+1];
    /* 0x0050, 16 */ uint8_t  cursor_pos [8][2]; /* 8 pairs of cursor location (x is LSB, y is MSB) */
    /* 0x0060, -  */ uint8_t  unused_60_62 [0x0062-0x0060+1];
    /* 0x0063, 2  */ uint16_t video_3D4_port;    /* Port address for 6845 video controller chip (0x3B4 or 0x3D4) */
    /* 0x0065, -  */ uint8_t  unused_65_6b [0x006b-0x0065+1];
    /* 0x006c, 4  */ uint32_t timer_ticks_counter; /* Timer ticks counter (count of 55ms ticks since CPU reset) */
    /* 0x0070, -  */ uint8_t  unused_70_83 [0x0083-0x0070+1];
    /* 0x0084, 1  */ uint8_t  EGA_text_rows;     /* EGA text rows-1 (maximum valid row value) */
    /* 0x0085, -  */ uint8_t  unused_85_86 [0x0086-0x0085+1];
    /* 0x0087, 1  */ uint8_t  EGA_misc_info;
    /* 0x0088, -  */ uint8_t  unused_88_132 [0x0132-0x0088+1];
} BIOS_data_area_t;
#pragma pack(pop);

#define get_BIOS_data_area_ptr() ((BIOS_data_area_t *) MK_FP (Seg0040, 0))
#define get_BIOS_data_area_var_ptr(type, var) ((type *) MK_FP (Seg0040, offsetof (BIOS_data_area_t, var)))

/*** Linking ***/

#ifdef __WATCOMC__

#endif  /* __WATCOMC__ */

#endif  /* !_BIOS_H_INCLUDED */
