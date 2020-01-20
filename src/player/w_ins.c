/* w_ins.c -- instruments window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$player$w_ins$*"
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "common.h"
#include "main/musins.h"
#include "main/musmod.h"
#include "main/mixchn.h"
#include "player/screen.h"
#include "player/plays3m.h"
#include "player/w_ins.h"

void __far win_instruments_on_resize (SCRWIN *self);
void __far win_instruments_draw (SCRWIN *self);
bool __far win_instruments_keypress (SCRWIN *self, char key);
void __far win_instruments_free (SCRWIN *self);

static const SCRWINVMT __win_instruments_vmt =
{
    &win_instruments_on_resize,
    &win_instruments_draw,
    &win_instruments_keypress,
    &win_instruments_free
};

/* private data */

typedef struct win_instruments_data_t
{
    MUSMOD *track;
    MIXCHNLIST *channels;
    uint8_t last_chn_ins[MUSMOD_CHANNELS_MAX];
    int lines_count;
    int page_start;
    int page_end;
};

#define HEAD_Y 1
#define LIST_X 1
#define LIST_Y (HEAD_Y + 1)

/* private methods */

int __near check_instrument_index (int value, int start, int end)
{
    if (value < start)
        return start;
    if (value > end)
        return end;
    return value;
}

void __near print_instrument_index (uint8_t x, uint8_t y, uint16_t index, uint8_t start, uint8_t end)
{
    if ((start <= index) && (index < end))
    {
        gotoxy (x, y + index - start);
        printf (
            "%02hhX",
            (uint8_t) index + 1
        );
    }
}

void __near win_instruments_draw_header (SCRWIN *self)
{
    textbackground (_black);
    textcolor (_white);
    gotoxy (LIST_X - 1 + 2, HEAD_Y);
    printf (
        "%s",
        "No Title                                                      Type    SN IV NV"
    );
}

void __near win_instruments_draw_line (SCRWIN *self, MUSINS *instrument, int index, uint8_t y)
{
    #define _BUF_SIZE 64
    #define _TITLE_LEN 58
    char buf[_BUF_SIZE];
    MUSINSTYPE type;
    char *type_str;
    uint8_t color;

    strncpy (buf, musins_get_title (instrument), _TITLE_LEN);
    buf[_TITLE_LEN] = 0;

    type = musins_get_type (instrument);
    switch (type)
    {
    case MUSINST_EMPTY:
        color = _darkgray;
        type_str = "none   ";
        break;
    case MUSINST_PCM:
        color = _lightgray;
        type_str = "pcm  ";
        break;
    case MUSINST_ADLIB:
        color = _darkgray;
        type_str = "adlib  ";
        break;
    default:
        color = _darkgray;
        type_str = "unknown";
        break;
    }

    textcolor (color);
    gotoxy (LIST_X - 1 + 2, y);
    printf (
        "%02hhX %s",
        (uint8_t) index + 1,
        (char *) buf
    );
    gotoxy (LIST_X - 1 + 64 , y);
    printf (
        "%s",
        (char *) type_str
    );

    gotoxy (LIST_X - 1 + 72, y);
    switch (type)
    {
    case MUSINST_PCM:
        printf (
            "%2hhu %02hhX %02hhX",
            (uint8_t) 1,
            (uint8_t) musins_get_volume (instrument),
            (uint8_t) musins_get_note_volume (instrument)
        );
        break;
    default:
        printf (
            "%s",
            "-- -- --"
        );
        break;
    }
    #undef _BUF_SIZE
    #undef _TITLE_LEN
}

void __near win_instruments_draw_list (SCRWIN *self)
{
    struct win_instruments_data_t *data;
    MUSMOD *track;
    MUSINSLIST *instruments;
    MUSINS *instrument;
    int index, count, i, y;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);
    track = data->track;
    instruments = musmod_get_instruments (track);
    index = data->page_start;
    count = data->page_end - data->page_start;
    y = LIST_Y;
    for (i = 0; i < count; i++)
    {
        instrument = musinsl_get (instruments, index);
        win_instruments_draw_line (self, instrument, index, y);
        index++;
        y++;
    }
}

void __near win_instruments_draw_footer (SCRWIN *self)
{
    struct win_instruments_data_t *data;
    MUSMOD *track;
    MUSINSLIST *instruments;
    int y;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);
    track = data->track;
    instruments = musmod_get_instruments (track);

    y = scrwin_get_height (self);
    textbackground (_black);
    textcolor (_lightgray);
    gotoxy (LIST_X - 1 + 2, y);
    printf (
        "%s",
        "Instruments:                          (Samples Number, Instrument/Note Volume)"
    );
    textcolor (_yellow);
    gotoxy (LIST_X - 1 + 15, y);
    printf (
        "%3hu",
        (uint16_t) musinsl_get_count (instruments)
    );
}

void __near win_instruments_draw_indexes (SCRWIN *self)
{
    struct win_instruments_data_t *data;
    MIXCHNLIST *channels;
    MIXCHN *chn;
    int page_start, page_end, count, i, n, old_n;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);
    channels = data->channels;
    page_start = data->page_start;
    page_end = data->page_end;
    count = mixchnl_get_count (channels);
    for (i = 0; i < count; i++)
    {
        chn = mixchnl_get (channels, i);

        if (mixchn_is_playing (chn) && (mixchn_get_type (chn) == MIXCHNTYPE_PCM))
            n = mixchn_get_instrument_num (chn);
        else
            n = 0;

        old_n = data->last_chn_ins[i];
        data->last_chn_ins[i] = n;
        if ((old_n != n) && (old_n != CHN_INS_NONE))
        {
            textbackground (_black);
            textcolor (_lightgray);
            print_instrument_index (LIST_X - 1 + 2, LIST_Y, old_n - 1, page_start, page_end);
        }
        if (n != CHN_INS_NONE)
        {
            textbackground (_lightgray);
            textcolor (_black);
            print_instrument_index (LIST_X - 1 + 2, LIST_Y, n - 1, page_start, page_end);
        }
    }
}

/* private events */

void __near win_instruments_on_page_change (SCRWIN *self)
{
    struct win_instruments_data_t *data;
    MUSMOD *track;
    MUSINSLIST *instruments;
    int count, page_start, page_end;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);
    track = data->track;
    instruments = musmod_get_instruments (track);

    count = data->lines_count;
    page_start = data->page_start;
    page_end = page_start + count;
    count = musinsl_get_count (instruments);
    if (page_end > count)
        page_end = count;
    data->page_end = page_end;
}

/* initialization */

bool __far win_instruments_init (SCRWIN *self)
{
    struct win_instruments_data_t *data;

    scrwin_init (self, "instruments list window");
    _copy_vmt (self, __win_instruments_vmt, SCRWINVMT);
    data = _new (struct win_instruments_data_t);
    if (!data)
        return false;
    scrwin_set_data (self, data);
    memset (data, 0, sizeof (struct win_instruments_data_t));
    return true;
}

/* public events */

void __far win_instruments_on_resize (SCRWIN *self)
{
    struct win_instruments_data_t *data;
    MUSMOD *track;
    MUSINSLIST *instruments;
    int lines_count;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);
    track = data->track;
    instruments = musmod_get_instruments (track);

    lines_count = scrwin_get_height (self) - 3;
    if (lines_count < 0)
        lines_count = 0;
    data->lines_count = lines_count;
    win_instruments_on_page_change (self);
}

/* public methods */

void __far win_instruments_set_track (SCRWIN *self, MUSMOD *value)
{
    struct win_instruments_data_t *data;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);

    data->track = value;
}

void __far win_instruments_set_channels (SCRWIN *self, MIXCHNLIST *value)
{
    struct win_instruments_data_t *data;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);

    data->channels = value;
}

void __far win_instruments_set_page_start (SCRWIN *self, int value)
{
    struct win_instruments_data_t *data;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);

    data->page_start = value;
    win_instruments_on_page_change (self);
}

void __far win_instruments_draw (SCRWIN *self)
{
    if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
    {
        textbackground (_black);
        textcolor (_lightgray);
        clrscr ();

        win_instruments_draw_header (self);
        win_instruments_draw_list (self);
        win_instruments_draw_footer (self);
    }

    win_instruments_draw_indexes (self);
}

bool __far win_instruments_keypress (SCRWIN *self, char c)
{
    struct win_instruments_data_t *data;
    MUSMOD *track;
    MUSINSLIST *instruments;
    int start, end, cur, add;

    data = (struct win_instruments_data_t *) scrwin_get_data (self);
    track = data->track;
    instruments = musmod_get_instruments (track);
    start = 0;
    end = musinsl_get_count (instruments) - data->lines_count;
    if (end < 0)
        end = 0;
    switch (c)
    {
    case '<':
    case '>':
        add = data->lines_count;
        if (c == '<')
            add = -add;
        cur = check_instrument_index (data->page_start + add, start, end);
        if (cur != data->page_start)
        {
            data->page_start = cur;
            win_instruments_on_page_change (self);
            scrwin_set_flags (self, scrwin_get_flags (self) | WINFL_FULLREDRAW);
        }
        return true;
    default:
        return false;
    }
}

/* free */

void __far win_instruments_free (SCRWIN *self)
{
    if (scrwin_get_data (self))
        _delete (scrwin_get_data (self));
}
