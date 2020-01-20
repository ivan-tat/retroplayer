/* crt.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$crt$*"
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "hw/bios.h"
#include "hw/vbios.h"
#include "hw/vga.h"
#include "sysdbg.h"
#include "startup.h"
#include "cc/conio.h"

cc_inoutres_t __far _crt_open (_cc_iobuf *f);   /* -InOutRes- */
cc_inoutres_t __far _crt_read (_cc_iobuf *f);   /* -InOutRes- */
cc_inoutres_t __far _crt_write (_cc_iobuf *f);  /* -InOutRes- */
cc_inoutres_t __far _crt_dummy (_cc_iobuf *f);  /* -InOutRes- */

void __near _crt_VBIOS_print_pchar (const char *s);
void __near _crt_VBIOS_print_new_line (void);
void __near _crt_VBIOS_print_char (char c);
void __near _crt_VBIOS_new_line (uint8_t *y);
void __near _crt_direct_write (const char *s, uint16_t count);
void __near _crt_direct_flush (uint8_t x, uint8_t y, const char *s, const char *end);

void __near _crt_catch_break (void)
{
    if (cc_gotbreak)
    {
        cc_gotbreak = false;
        while (cc_kbhit ())
            cc_getch ();
        _crt_VBIOS_print_pchar ("^C");
        _crt_VBIOS_print_new_line ();
        _cc_ExitWithError (255, NULL);
    }
}

void __far cc_TextAssignCrt (_cc_iobuf *f, void *buffer, uint16_t size)
{
#if SYSDEBUG_IOBUF == 1
    SYSDEBUG_INFO ("Called.");
#endif  /* SYSDEBUG_IOBUF == 1 */
    f->mode = cc_fmClosed;
    f->buf_size = size;
    f->buf_ptr = buffer;
    f->io.by_name.open = _crt_open;
    f->name[0] = 0;
}

cc_inoutres_t __far _crt_open (_cc_iobuf *f)
{
    if (f->mode == cc_fmInput)
    {
        f->io.by_name.in_out = _crt_read;
        f->io.by_name.flush  = _crt_dummy;
    }
    else
    {
        f->mode = cc_fmOutput;
        f->io.by_name.in_out = _crt_write;
        f->io.by_name.flush  = _crt_write;
    }
    f->io.by_name.close = _crt_dummy;
    return EINOUTRES_SUCCESS;
}

void __near __crt_read_backspace (uint16_t *pos, uint16_t count)
{
    do
    {
        if (!*pos) return;
        _crt_VBIOS_print_pchar ("\x8 \x8");
        (*pos)--;
        count--;
    } while (count);
}

void __near __crt_read_EOT (char *buf_ptr, uint16_t *pos, uint16_t end_pos, uint16_t count)
{
    char c;

    do
    {
        if (*pos == end_pos) return;
        c = buf_ptr [*pos];
        if (c < ' ') return;
        _crt_VBIOS_print_char (c);
        (*pos)++;
        count--;
    } while (count);
}

cc_inoutres_t __far _crt_read (_cc_iobuf *f)
{
    uint16_t buf_size, buf_pos, i;
    char *buf_ptr;
    bool loop;
    char c;

    buf_size = f->buf_size - 2;
    buf_pos = f->buf_pos;
    buf_ptr = (char *) f->buf_ptr;
    i = 0;
    loop = true;
    do
    {
        cc_lastscancode = 0;
        c = cc_getch ();
        switch (c)
        {
        case 8:     /* ^H - BS - backspace */
        case 0x13:  /* ^S - DC3 - dev ctrl 3 (X-OFF) */
            __crt_read_backspace (&i, 1);
            break;
        case 4:
            __crt_read_EOT (buf_ptr, &i, buf_pos, 1);
            break;
        case 1:     /* ^A - SOH - start of heading */
        case 0x1b:  /* ^[ - ESC - escape */
            __crt_read_backspace (&i, ~0U); /* FIXME: implement better loop */
            break;
        case 6:     /* ^F - ACK - acknowledge */
            __crt_read_EOT (buf_ptr, &i, buf_pos, ~0U); /* FIXME: implement better loop */
            break;
        case 0x1a:  /* ^Z - SUB - substitute */
            if (cc_checkeof)
            {
                buf_ptr [i] = c;
                i++;
                loop = false;
            }
            break;
        case 0x0d:  /* ^M - CR - carriage return */
            _crt_VBIOS_print_new_line ();
            buf_ptr [i + 0] = 0x0d;
            buf_ptr [i + 1] = 0x0a;
            i += 2;
            loop = false;
            break;
        default:
            if (c >= ' ' && i != buf_size)
            {
                buf_ptr [i] = c;
                i++;
                _crt_VBIOS_print_char (c);
                if (i > buf_pos)
                    buf_pos = i;
            }
            break;
        }
    } while (loop);
    f->buf_pos = 0;
    f->buf_end = i;
    return EINOUTRES_SUCCESS;
}

cc_inoutres_t __far _crt_write (_cc_iobuf *f)
{
    uint16_t count;
    const char *s;

    count = f->buf_pos;
    if (count)
    {
        f->buf_pos -= count;
        s = (const char *) f->buf_ptr;
        if (cc_directvideo)
            _crt_direct_write (s, count);
        else
            do
            {
                _crt_VBIOS_print_char (*s);
                s++;
                count--;
            } while (count);
    }
    _crt_catch_break ();
    return EINOUTRES_SUCCESS;
}

cc_inoutres_t __far _crt_dummy (_cc_iobuf *f)
{
    return EINOUTRES_SUCCESS;
}

void __near _crt_VBIOS_print_pchar (const char *s)
{
    while (*s)
    {
        _crt_VBIOS_print_char (*s);
        s++;
    }
}

void __near _crt_VBIOS_print_new_line (void)
{
    _crt_VBIOS_print_pchar (CRLF);
}

void __near _crt_VBIOS_print_char (char c)
{
    char page;
    struct vbios_cursor_state_t curs;

    page = 0;   /* FIXME: why active page is always zero? */
    vbios_query_cursor_state (page, &curs);
    switch (c)
    {
    case 7:     /* ^G - BEL - bell */
        vbios_write_character_as_tty (7, 0);
        break;
    case 8:     /* ^H - BS - backspace */
        if (curs.x != cc_windmin.rect.x)
            curs.x--;
        break;
    case 0x0a:  /* ^J - LF - line feed */
        _crt_VBIOS_new_line (&curs.y);
        break;
    case 0x0d:  /* ^M - CR - carriage return */
        curs.x = cc_windmin.rect.x;
        break;
    default:
        vbios_put_character_and_attribute (page, c, cc_textattr, 1);
        curs.x++;
        if (curs.x > cc_windmax.rect.x)
        {
            curs.x = cc_windmin.rect.x;
            _crt_VBIOS_new_line (&curs.y);
        }
        break;
    }
    vbios_set_cursor_pos (page, curs.x, curs.y);
}

void __near _crt_VBIOS_new_line (uint8_t *y)
{
    if (*y+1 <= cc_windmax.rect.y)
        (*y)++;
    else
        vbios_scroll_up (
            cc_windmin.rect.x, cc_windmin.rect.y,
            cc_windmax.rect.x, cc_windmax.rect.y, 1, cc_textattr);
}

void __near _crt_direct_write (const char *s, uint16_t count)
{
    char page;
    struct vbios_cursor_state_t curs;
    uint8_t cur_x, cur_y;
    const char *cur_s;
    bool flushed;
    BIOS_data_area_t *info;

    page = 0;   /* FIXME: why active page is always zero? */
    vbios_query_cursor_state (page, &curs);
    cur_x = curs.x;
    cur_y = curs.y;
    cur_s = s;
    flushed = false;
    do
    {
        switch (*cur_s)
        {
        case 7:     /* ^G - BEL - bell */
            _crt_direct_flush (curs.x, curs.y, s, cur_s);
            vbios_write_character_as_tty (7, 0);
            cur_s++;
            flushed = true;
            break;
        case 8:     /* ^H - BS - backspace */
            _crt_direct_flush (curs.x, curs.y, s, cur_s);
            if (cur_x != cc_windmin.rect.x)
                cur_x--;
            cur_s++;
            flushed = true;
            break;
        case 0x0a:  /* ^J - LF - line feed */
            _crt_direct_flush (curs.x, curs.y, s, cur_s);
            _crt_VBIOS_new_line (&cur_y);
            cur_s++;
            flushed = true;
            break;
        case 0x0d:  /* ^M - CR - carriage return */
            _crt_direct_flush (curs.x, curs.y, s, cur_s);
            cur_x = cc_windmin.rect.x;
            cur_s++;
            flushed = true;
            break;
        default:
            cur_s++;
            cur_x++;
            if (cur_x > cc_windmax.rect.x)
            {
                _crt_direct_flush (curs.x, curs.y, s, cur_s);
                _crt_VBIOS_new_line (&cur_y);
                cur_x = cc_windmin.rect.x;
                flushed = true;
            }
            break;
        }
        if (flushed)
        {
            flushed = false;
            s = cur_s;
            curs.x = cur_x;
            curs.y = cur_y;
        }
        count--;
    } while (count);
    _crt_direct_flush (curs.x, curs.y, s, cur_s);
    info = get_BIOS_data_area_ptr ();
    info->cursor_pos [page][0] = cur_x;
    info->cursor_pos [page][1] = cur_y;
    vga_set_text_cursor_position (
        info->video_3D4_port, info->text_screen_width * cur_y + cur_x);
}

void __near _crt_direct_flush (uint8_t x, uint8_t y, const char *s, const char *end)
{
    uint16_t count;
    BIOS_data_area_t *info;
    uint16_t vid_seg, vid_port;
    #pragma pack(push, 1);
    union
    {
        struct { char c, attr; } symb;
        uint16_t value;
    } v;
    #pragma pack(pop);
    uint16_t *p;

    count = FP_OFF (end) - FP_OFF (s);
    if (count)
    {
        info = get_BIOS_data_area_ptr ();
        vid_seg = (info->active_video_mode == _TEXTMONO) ? cc_SegB000 : cc_SegB800;
        /* FIXME: why active page is always zero? */
        p = MK_FP (vid_seg, (info->text_screen_width * y + x) * 2);
        v.symb.attr = cc_textattr;
        if (cc_checksnow)
        {
            vid_port = info->video_3D4_port;
            do
            {
                v.symb.c = *s;
                s++;
                vga_wait_sync (vid_port, true);
                *p = v.value;
                p++;
                _enable ();
                count--;
            } while (count);
        }
        else
        {
            do
            {
                v.symb.c = *s;
                s++;
                *p = v.value;
                p++;
                count--;
            } while (count);
        }
    }
}
