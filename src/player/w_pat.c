/* w_pat.c -- pattern window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/string.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "common.h"
#include "main/muspat.h"
#include "main/effects.h"
#include "main/musmodps.h"
#include "main/s3mplay.h"
#include "player/screen.h"
#include "player/plays3m.h"

#include "player/w_pat.h"

void __far win_pattern_on_resize (SCRWIN *self);
bool __far win_pattern_keypress (SCRWIN *self, char key);
void __far win_pattern_draw (SCRWIN *self);
void __far win_pattern_free (SCRWIN *self);

static const SCRWINVMT __win_pattern_vmt =
{
    &win_pattern_on_resize,
    &win_pattern_draw,
    &win_pattern_keypress,
    &win_pattern_free
};

/* private data */

static const char _hexdigits[16] = "0123456789ABCDEF";

#define DISPLAY_COLUMNS 5

typedef struct win_pattern_data_t
{
    MUSMOD *track;
    PLAYSTATE *ps;
    MIXCHNLIST *channels;
    int pattern_num;
    int row;
    uint8_t line_start;
    uint8_t line_cur;
    uint8_t line_end;
    uint8_t columns;
    uint8_t start_channel;
};

#define HEAD_Y 1
#define LIST_X 1
#define LIST_Y 2

/* private methods */

// Format: "B-7 99 40 ZFF"

void __near draw_channel_dumb (void)
{
    printf (
        "             "
    );
}

void __near draw_channel_event (MUSPATCHNEVENT *event)
{
    uint8_t _ins, _vol, _cmd, _parm;
    char buf[12];

    get_note_name (buf, event->data.note);
    printf (
        "%s",
        (char *) buf
    );

    _ins  = event->data.instrument;
    _vol  = event->data.note_volume;
    _cmd  = event->data.command;
    _parm = event->data.parameter;

    buf[0] = ' ';
    if (_ins == CHN_INS_NONE)
    {
        buf[1] = '.';
        buf[2] = '.';
    }
    else
    if ((_ins >= CHN_INS_MIN) && (_ins <= CHN_INS_MAX))
    {
        _ins = _get_instrument (_ins) + 1;
        buf[1] = _hexdigits[_ins >> 4];
        buf[2] = _hexdigits[_ins & 0x0f];
    }
    else
    {
        buf[1] = '?';
        buf[2] = '?';
    }

    buf[3] = ' ';
    if (_vol <= CHN_NOTEVOL_MAX)
    {
        buf[4] = _hexdigits[_vol >> 4];
        buf[5] = _hexdigits[_vol & 0x0f];
    }
    else
    if (_vol == CHN_NOTEVOL_NONE)
    {
        buf[4] = '.';
        buf[5] = '.';
    }
    else
    {
        buf[4] = '?';
        buf[5] = '?';
    }

    buf[6] = ' ';
    if (_cmd == CHN_CMD_NONE)
    {
        buf[7] = '.';
        buf[8] = '.';
        buf[9] = '.';
    }
    else
    {
        if (_cmd <= MAXEFF)
            buf[7] = 'A' + _cmd - 1;
        else
            buf[7] = '?';

        buf[8] = _hexdigits[_parm >> 4];
        buf[9] = _hexdigits[_parm & 0x0f];
    }
    buf[10] = 0;
    printf (
        "%s",
        (char *) buf
    );
}

void __near win_pattern_draw_row (SCRWIN *self, MUSPATIO *f, uint8_t row, bool active)
{
    struct win_pattern_data_t *data;
    MUSPAT *pat;
    MUSPATROWEVENT e;
    MUSPATCHNEVENT line[DISPLAY_COLUMNS];
    MIXCHNLIST *channels;
    uint8_t num_channels, columns, chn, i, c_start, c_end;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    channels = data->channels;
    num_channels = mixchnl_get_count (channels);
    pat = f->pattern;
    columns = data->columns;

    c_start = data->start_channel - 1;
    c_end = c_start + columns;
    if (c_end >= num_channels)
        c_end = num_channels;

    for (i = 0; i < c_end - c_start; i++)
        muspatchnevent_clear (& (line [i]));

    if (muspat_is_data_packed (pat))
    {
        muspatio_seek (f, row, 0);

        i = 0;
        while (!muspatio_is_end_of_row (f))
        {
            muspatio_read (f, &e);
            if ((e.channel >= c_start) && (e.channel < c_end))
                memcpy (& (line [e.channel - c_start]), & (e.event), sizeof (MUSPATCHNEVENT));
        }
    }
    else
    {
        muspatio_seek (f, row, c_start);

        for (i = c_start; i < c_end ; i++)
        {
            muspatio_read (f, &e);
            memcpy (& (line [i - c_start]), & (e.event), sizeof (MUSPATCHNEVENT));
        }
    }

    if (active)
        textbackground (_blue);
    else
        textbackground (_black);

    textcolor (_lightgray);
    printf (
        " %02hX ",
        (uint16_t) row
    );
    textcolor (_darkgray);
    printf (
        "\xb3"
    );

    for (i = 0; i < columns ; i++)
    {
        textcolor (_lightgray);

        if (data->start_channel - 1 + i < num_channels)
            draw_channel_event (& (line[i]));
        else
            draw_channel_dumb ();

        textcolor (_darkgray);
        printf (
            "\xb3"
        );
    }
}

void __near win_pattern_draw_row_empty (SCRWIN *self)
{
    struct win_pattern_data_t *data;
    uint8_t columns, i;

    data = scrwin_get_data (self);
    columns = data->columns;

    textbackground (_black);
    textcolor (_lightgray);
    printf (
        "    "
    );
    textcolor (_darkgray);
    printf (
        "\xb3"
    );

    for (i = 0; i < columns ; i++)
    {
        textcolor (_lightgray);

        draw_channel_dumb ();

        textcolor (_darkgray);
        printf (
            "\xb3"
        );
    }
}

void __near win_pattern_draw_list (SCRWIN *self)
{
    struct win_pattern_data_t *data;
    MUSMOD *track;
    PLAYSTATE *ps;
    MUSPATLIST *patterns;
    MUSPAT *pattern;
    int pattern_num, row, rows;
    MUSPATIO f;
    int line_start, line_cur, line_end, y;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);

    /* get play state */
    track = data->track;
    ps = data->ps;
    pattern_num = ps->pattern;
    row = ps->row;
    patterns = musmod_get_patterns (track);
    pattern = muspatl_get (patterns, pattern_num);
    rows = muspat_get_rows (pattern);
    line_start = data->line_start;
    line_cur = data->line_cur;
    line_end = data->line_end;

    muspatio_open (&f, pattern, MUSPATIOMD_READ);

    row -= line_cur - 1;
    for (y = line_start; y <= line_end; y++)
    {
        gotoxy (LIST_X, y);

        if ((row >= 0) && (row < rows))
            win_pattern_draw_row (self, &f, row, y == line_cur);
        else
            win_pattern_draw_row_empty (self);

        row++;
    }

    muspatio_close (&f);
}

void __near win_pattern_draw_header (SCRWIN *self)
{
    struct win_pattern_data_t *data;
    uint8_t columns, i, chn;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    columns = data->columns;

    textbackground (_black);
    textcolor (_darkgray);
    gotoxy (LIST_X - 1 + 5, HEAD_Y);
    printf (
        "\xda\xc4\xc4\xc4\xb4"
    );

    chn = data->start_channel;
    for (i = 0; i < columns; i++)
    {
        textcolor (_white);
        printf (
            " c%2hu ",
            (uint16_t) chn
        );
        textcolor (_darkgray);
        if (i < columns - 1)
            printf (
                "\xc3\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xb4"
            );
        else
            printf (
                "\xc3\xc4\xc4\xc4\xbf"
            );
        chn++;
    }
}

/* private events */

/* initialization */

bool __far win_pattern_init (SCRWIN *self)
{
    struct win_pattern_data_t *data;

    scrwin_init (self, "pattern window");
    _copy_vmt (self, __win_pattern_vmt, SCRWINVMT);
    data = _new (struct win_pattern_data_t);
    if (!data)
        return false;
    scrwin_set_data (self, data);
    memset (data, 0, sizeof (struct win_pattern_data_t));
    data->pattern_num = -1;
    data->row = -1;
    data->columns = DISPLAY_COLUMNS;
    return true;
}

/* public events */

void __far win_pattern_on_resize (SCRWIN *self)
{
    struct win_pattern_data_t *data;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    data->line_start = 2;
    data->line_end = scrwin_get_height (self);
    data->line_cur = (data->line_start + data->line_end) / 2;
}

/* public methods */

void __far win_pattern_set_track (SCRWIN *self, MUSMOD *value)
{
    struct win_pattern_data_t *data;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    data->track = value;
}

void __far win_pattern_set_play_state (SCRWIN *self, PLAYSTATE *value)
{
    struct win_pattern_data_t *data;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    data->ps = value;
}

void __far win_pattern_set_channels (SCRWIN *self, MIXCHNLIST *value)
{
    struct win_pattern_data_t *data;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    data->channels = value;
}

void __far win_pattern_set_start_channel (SCRWIN *self, int value)
{
    struct win_pattern_data_t *data;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    data->start_channel = value;
}

void __far win_pattern_draw (SCRWIN *self)
{
    struct win_pattern_data_t *data;
    PLAYSTATE *ps;
    int pattern_num, row;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);

    /* get play state */
    ps = data->ps;
    pattern_num = ps->pattern;
    row = ps->row;

    if (scrwin_get_flags (self) & WINFL_FULLREDRAW)
    {
        textbackground(_black);
        textcolor(_lightgray);
        clrscr();

        win_pattern_draw_header (self);
    }

    if ((scrwin_get_flags (self) & WINFL_FULLREDRAW)
    ||  (pattern_num != data->pattern_num)
    ||  (row != data->row))
        win_pattern_draw_list (self);
}

bool __far win_pattern_keypress (SCRWIN *self, char c)
{
    struct win_pattern_data_t *data;
    MIXCHNLIST *channels;
    int num_channels, chn;

    data = (struct win_pattern_data_t *) scrwin_get_data (self);
    channels = data->channels;
    num_channels = mixchnl_get_count (channels);

    switch (c)
    {
    case 75:
    case 77:
        chn = data->start_channel;
        if (c == 77)
            chn++;
        else
            chn--;

        if ((chn >= 0) && (chn < num_channels))
        {
            data->start_channel = chn;
            scrwin_set_flags (self, scrwin_get_flags (self) | WINFL_FULLREDRAW);
        }
        return true;
    default:
        return false;
    }
}

/* free */

void __far win_pattern_free (SCRWIN *self)
{
    if (scrwin_get_data (self))
        _delete (scrwin_get_data (self));
}
