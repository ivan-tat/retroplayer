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

/* Linking */

#ifdef __WATCOMC__
#pragma aux vbiosda_get_text_width "*";
#pragma aux vbiosda_get_text_height "*";
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
#endif

#endif  /* VBIOS_H */
