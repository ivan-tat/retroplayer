/* init.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$init$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "cc/i86.h"
#include "hw/vbios.h"
#include "cc/conio.h"

#ifdef DEFINE_LOCAL_DATA

uint16_t cc_lastmode = 0;
uint16_t cc_screenwidth = 0;
uint16_t cc_screenheight = 0;
bool cc_checksnow = false;
bool cc_directvideo = false;
union text_rect_t cc_windmin = { .value = 0 };
union text_rect_t cc_windmax = { .value = 0 };
uint8_t cc_textattrorig = 0;
uint8_t cc_textattr = 0;
char cc_lastkey = 0;
bool cc_checkeof = false;
bool cc_gotbreak = false;
bool cc_checkbreak = false;

#endif  /* DEFINE_LOCAL_DATA */

void _cc_console_set_mode(uint16_t mode)
{
    uint8_t *p;
    struct vbios_font_info_t font;

    /* Disable cursor emulation */
    p = vbiosda_get_ega_misc_info();
    *p &= ~0x01;

    if ((mode > _TEXTC80) && (mode != _TEXTMONO))
        mode = _TEXTC80;

    vbios_set_mode(mode);

    if (mode & _Font8x8)
    {
        vbios_load_rom_font_8x8(0);
        vbios_query_font_info(&font, VBIOS_FONT_CGA_VEC);
        if ((font.rows == 43 - 1) || (font.rows == 50 - 1))
        {
            /* Enable cursor emulation */
            *p |= 0x01;
            vbios_set_cursor_shape(6, 0);
            vbios_use_alternate_print_screen();
        }
    }
}

void _cc_console_on_mode_change(void)
{
    struct vbios_video_info_t video;
    struct vbios_font_info_t font;
    uint16_t origmode;
    uint16_t rows;
    bool chksnow;
    bool smallfont;

    vbios_query_video_info(&video);

    origmode = video.mode;
    if (rows != 25)
        origmode += _Font8x8;

    vbios_query_font_info(&font, VBIOS_FONT_CGA_VEC);

    chksnow = false;
    if (font.rows)
        rows = font.rows + 1;
    else
    {
        rows = 25;
        chksnow = (video.mode <= _TEXTC80) ? true : false;
    }

    cc_lastmode = origmode;
    cc_screenwidth = video.columns;
    cc_screenheight = rows;
    cc_checksnow = chksnow;
    cc_directvideo = true;
    cc_windmin.value = 0;
    cc_windmax.rect.x = cc_screenwidth - 1;
    cc_windmax.rect.y = cc_screenheight - 1;
}

void _cc_console_on_start(void)
{
    struct vbios_video_info_t video;
    uint16_t c;

    vbios_query_video_info(&video);

    if ((video.mode > _TEXTC80) || (video.mode != _TEXTMONO))
        _cc_console_set_mode(_TEXTC80);

    _cc_console_on_mode_change();

    cc_textattrorig = (vbios_get_character_and_attribute(0) >> 8) & 0x7f;
    cc_textattr = cc_textattrorig;
    cc_checkeof = false;
    cc_lastkey = 0;
    cc_gotbreak = false;
    cc_checkbreak = true;
}

void cc_console_init(void)
{
    _cc_console_on_start();
}
