/* vbios.c -- video BIOS library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"

#include "hw/vbios.h"

#define Seg0040 0x40

unsigned vbiosda_get_text_width(void)
{
    // Byte at 0x0040:0x004a - screen width in text columns
    uint8_t value;
    value = *(uint8_t *)(MK_FP(Seg0040, 0x4a));
    return value ? value : 80;
}

unsigned vbiosda_get_text_height(void)
{
    // Byte at 0x0040:0x0084 - EGA text rows-1 (maximum valid row value)
    uint8_t value;
    value = *(uint8_t *)(MK_FP(Seg0040, 0x84));
    return value ? value + 1 : 25;
}

char *vbiosda_get_ega_misc_info(void)
{
    return (char *)MK_FP(Seg0040, 0x87);
}

void vbios_set_mode(uint8_t mode)
{
    union REGPACK regs;

    regs.h.ah = 0;
    regs.h.al = mode;
    intr(0x10, &regs);
}

/*
 * Description:
 *      "start" and "stop" are 0-based.
 */
void vbios_set_cursor_shape(uint8_t start, uint8_t stop)
{
    union REGPACK regs;

    regs.h.ah = 1;
    regs.h.cl = stop;
    regs.h.ch = start;
    intr(0x10, &regs);
}

/*
 * Description:
 *      "page", "x" and "y" are 0-based.
 */
void vbios_set_cursor_pos(uint8_t page, uint8_t x, uint8_t y)
{
    union REGPACK regs;

    regs.h.ah = 2;
    regs.h.bh = page;
    regs.h.dl = x;
    regs.h.dh = y;
    intr(0x10, &regs);
}

/*
 * Description:
 *      "page" is 0-based.
 */
void vbios_query_cursor_state(uint8_t page, struct vbios_cursor_state_t *state)
{
    union REGPACK regs;

    regs.h.ah = 3;
    regs.h.bh = page;
    intr(0x10, &regs);
    state->start = regs.h.ch;
    state->end = regs.h.cl;
    state->x = regs.h.dl;
    state->y = regs.h.dh;
}

#ifdef CONFIG_VBIOS_ENABLE_LIGHT_PEN
bool vbios_read_light_pen(struct vbios_light_pen_state_t *state)
{
    union REGPACK regs;

    regs.h.ah = 4;
    intr(0x10, &regs);
    state->text_x = regs.h.dl;
    state->text_y = regs.h.dh;
    state->graph_x = regs.w.bx;
    state->graph_y = regs.w.cx;
    return regs.h.al ? true : false;
}
#endif  /* CONFIG_VBIOS_ENABLE_LIGHT_PEN */

/*
 * Description:
 *      "page" is 0-based.
 */
void vbios_set_active_page(uint8_t page)
{
    union REGPACK regs;

    regs.h.ah = 5;
    regs.h.al = page;
    intr(0x10, &regs);
}

/*
 * Description:
 *      "x0", "y0", "x1", "y1" are 0-based.
 *      (x0, y0) - upper left corner. (0, 0) is the top left.
 *      (x1, y1) - lower right corner.
 *      If count is zero, this function clears the specified text area.
 */
void vbios_scroll_up(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t count, uint8_t color)
{
    union REGPACK regs;

    regs.h.ah = 6;
    regs.h.al = count;
    regs.h.bh = color;
    regs.h.cl = x0;
    regs.h.ch = y0;
    regs.h.dl = x1;
    regs.h.dh = y1;
    intr(0x10, &regs);
}

/*
 * Description:
 *      "x0", "y0", "x1", "y1" are 0-based.
 *      (x0, y0) - upper left corner. (0, 0) is the top left.
 *      (x1, y1) - lower right corner.
 *      If count is zero, this function clears the specified text area.
 */
void vbios_scroll_down(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t count, uint8_t color)
{
    union REGPACK regs;

    regs.h.ah = 7;
    regs.h.al = count;
    regs.h.bh = color;
    regs.h.cl = x0;
    regs.h.ch = y0;
    regs.h.dl = x1;
    regs.h.dh = y1;
    intr(0x10, &regs);
}

/*
 * Description:
 *      Return value is (character + color * 256).
 *      When the screen is in graphics mode the character color is not returned.
 */
uint16_t vbios_get_character_and_attribute(uint8_t page)
{
    union REGPACK regs;

    regs.h.ah = 8;
    regs.h.bh = page;
    intr(0x10, &regs);
    return regs.h.al + (regs.h.bh << 8);
}

void vbios_query_video_info(struct vbios_video_info_t *info)
{
    union REGPACK regs;

    if (info)
    {
        regs.h.ah = 0x0f;
        intr(0x10, &regs);
        info->mode = regs.h.al;
        info->columns = regs.h.ah;
        info->page = regs.h.bh;
    }
}

/*
 * Description:
 *      Block number is 0-3 for EGA and 0-7 for VGA.
 */
void vbios_load_rom_font_8x8(char block)
{
    union REGPACK regs;

    regs.w.ax = 0x1112;
    regs.h.bl = block;
    intr(0x10, &regs);
}

void vbios_query_font_info(struct vbios_font_info_t *info, vbios_font_type_t type)
{
    union REGPACK regs;

    if (info)
    {
        regs.w.ax = 0x1130;
        regs.h.bh = type;
        regs.h.dl = 0;  /* Unused, set to zero for CGA */
        intr(0x10, &regs);
        info->height = regs.w.cx;   /* same as word at Seg0040:0x0085 */
        info->rows = regs.h.dl;     /* same as byte at Seg0040:0x0084 */
        info->font = MK_FP(regs.w.es, regs.w.bp);   /* same as interrupt vector 0x1f */
    }
}

void vbios_use_alternate_print_screen(void)
{
    union REGPACK regs;

    regs.h.ah = 0x12;
    regs.h.bl = 0x20;
    intr(0x10, &regs);
}