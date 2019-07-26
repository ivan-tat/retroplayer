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

/* Video BIOS data area functions */

unsigned vbiosda_get_text_width(void);
unsigned vbiosda_get_text_height(void);

/*
 * Byte at 0x0040:0x0087 - EGA miscellaneous information
 *
 * 0x01: 1 = cursor emulation is enabled
 * 0x02: 1 = EGA is attached to a monochrome display
 * 0x04: always 0
 * 0x08: 1 = EGA is not active
 * 0x10: always 0
 * 0x40,0x20: EGA total RAM (00 = 64KiB, 01 = 128KiB, 10 = 192KiB, 11 = 256KiB)
 * 0x80: high bit of video mode (1 = screen is not cleared)
 */
char *vbiosda_get_ega_misc_info(void);

/* Video BIOS interrupt 0x10 functions */

#pragma pack(push, 1);
typedef struct vbios_cursor_state_t
{
    uint8_t start;
    uint8_t end;
    uint8_t x;
    uint8_t y;
};
#pragma pack(pop);

#ifdef CONFIG_VBIOS_ENABLE_LIGHT_PEN
#pragma pack(push, 1);
typedef struct vbios_light_pen_state_t
{
    uint8_t text_x;
    uint8_t text_y;
    uint16_t graph_x;
    uint16_t graph_y;
};
#pragma pack(pop);
#endif  /* CONFIG_VBIOS_ENABLE_LIGHT_PEN */

#pragma pack(push, 1);
typedef struct vbios_video_info_t {
    uint8_t mode;
    uint8_t columns;
    uint8_t page;
};

typedef struct vbios_font_info_t {
    uint16_t height;
    uint8_t rows;   /* actual value is (rows - 1) or 0 for CGA BIOS */
    void __far *font;
};
#pragma pack(pop);

typedef uint8_t vbios_font_type_t;

#define VBIOS_FONT_CGA_VEC  0
#define VBIOS_FONT_EGA_VEC  1
#define VBIOS_FONT_8X14     2
#define VBIOS_FONT_8X8      3
#define VBIOS_FONT_8X8_ALT  4
#define VBIOS_FONT_8X14_ALT 5
#define VBIOS_FONT_8X16     6
#define VBIOS_FONT_8X16_ALT 7

void vbios_set_mode(uint8_t mode);
void vbios_set_cursor_shape(uint8_t start, uint8_t stop);
void vbios_set_cursor_pos(uint8_t page, uint8_t x, uint8_t y);
void vbios_query_cursor_state(uint8_t page, struct vbios_cursor_state_t *state);
#ifdef CONFIG_VBIOS_ENABLE_LIGHT_PEN
bool vbios_read_light_pen(struct vbios_light_pen_state_t *state);
#endif  /* CONFIG_VBIOS_ENABLE_LIGHT_PEN */
void vbios_set_active_page(uint8_t page);
void vbios_scroll_up(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t count, uint8_t color);
void vbios_scroll_down(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t count, uint8_t color);
uint16_t vbios_get_character_and_attribute(uint8_t page);
void vbios_query_video_info(struct vbios_video_info_t *info);
void vbios_load_rom_font_8x8(char block);
void vbios_query_font_info(struct vbios_font_info_t *info, vbios_font_type_t type);
void vbios_use_alternate_print_screen(void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux vbiosda_get_text_width "*";
#pragma aux vbiosda_get_text_height "*";
#pragma aux vbiosda_get_ega_misc_info "*";
#pragma aux vbios_set_mode "*";
#pragma aux vbios_set_cursor_shape "*";
#pragma aux vbios_set_cursor_pos "*";
#pragma aux vbios_query_cursor_state "*";
#ifdef CONFIG_VBIOS_ENABLE_LIGHT_PEN
#pragma aux vbios_read_light_pen "*";
#endif  /* CONFIG_VBIOS_ENABLE_LIGHT_PEN */
#pragma aux vbios_set_active_page "*";
#pragma aux vbios_scroll_up "*";
#pragma aux vbios_scroll_down "*";
#pragma aux vbios_get_character_and_attribute "*";
#pragma aux vbios_query_video_info "*";
#pragma aux vbios_load_rom_font_8x8 "*";
#pragma aux vbios_query_font_info "*";
#pragma aux vbios_use_alternate_print_screen "*";

#endif  /* __WATCOMC__ */

#endif  /* VBIOS_H */
