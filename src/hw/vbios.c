/* vbios.c -- video BIOS library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$hw$vbios$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "hw/bios.h"
#include "hw/vbios.h"

unsigned vbiosda_get_text_width(void)
{
    uint16_t value;
    value = get_BIOS_data_area_ptr ()->text_screen_width;
    return value ? value : 80;
}

unsigned vbiosda_get_text_height(void)
{
    uint8_t value;
    value = get_BIOS_data_area_ptr ()->EGA_text_rows;
    return value ? value + 1 : 25;
}

char *vbiosda_get_ega_misc_info(void)
{
    return get_BIOS_data_area_var_ptr (char, EGA_misc_info);
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

#if CONFIG_VBIOS_ENABLE_LIGHT_PEN == 1
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
#endif  /* CONFIG_VBIOS_ENABLE_LIGHT_PEN == 1 */

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
    return regs.w.ax;
}

/*
 * Description:
 *      Write character and attribute at current cursor location.
 *      In graphics modes "color" is a color number.
 *      Does not update the cursor position.
 */
void vbios_put_character_and_attribute (uint8_t page, char c, uint8_t color, uint16_t count)
{
    union REGPACK regs;

    regs.h.ah = 9;
    regs.h.al = c;
    regs.h.bl = color;
    regs.h.bh = page;
    regs.w.cx = count;
    intr (0x10, &regs);
}

/*
 * Description:
 *      Write character without attribute at current cursor location and
 *      update the cursor position. When the cursor advances to the end of
 *      the last screen line - scroll the text up one line.
 *      "color" is used only in graphics modes as a foreground color.
 *      Characters 7, 0x0a, 0x0d are treated specially.
 */
void vbios_write_character_as_tty (char c, char color)
{
    union REGPACK regs;

    regs.h.ah = 0x0e;
    regs.h.al = c;
    regs.h.bl = color;
    intr(0x10, &regs);
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
