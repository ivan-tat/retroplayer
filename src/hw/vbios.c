/* vbios.c -- video BIOS library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"

#include "hw/vbios.h"

unsigned vbiosda_get_text_width(void)
{
    // Byte at 0x0040:0x004a - screen width in text columns
    return *(uint8_t *)(MK_FP(0x40, 0x4a));
}

unsigned vbiosda_get_text_height(void)
{
    // Byte at 0x0040:0x0084 - EGA text rows-1 (maximum valid row value)
    return *(uint8_t *)(MK_FP(0x40, 0x84)) + 1;
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
    regs.h.dl = x0;
    regs.h.dh = y0;
    regs.h.cl = x1;
    regs.h.ch = y1;
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
    regs.h.dl = x0;
    regs.h.dh = y0;
    regs.h.cl = x1;
    regs.h.ch = y1;
    intr(0x10, &regs);
}
