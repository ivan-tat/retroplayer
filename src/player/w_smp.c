/* w_smp.c -- samples window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$player$w_smp$*"
#endif

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
#include "player/w_smp.h"

void __far win_samples_on_resize (SCRWIN *self);
void __far win_samples_draw (SCRWIN *self);
bool __far win_samples_keypress (SCRWIN *self, char key);
void __far win_samples_free (SCRWIN *self);

static const SCRWINVMT __win_samples_vmt =
{
    &win_samples_on_resize,
    &win_samples_draw,
    &win_samples_keypress,
    &win_samples_free
};

/* private data */

typedef struct win_samples_data_t
{
    MUSMOD *track;
    MIXCHNLIST *channels;
    int last_chn_smp[MUSMOD_CHANNELS_MAX];
    int lines_count;
    int page_start;
    int page_end;
};

#define HEAD_Y 1
#define LIST_X 1
#define LIST_Y (HEAD_Y + 1)

/* private methods */

int __near check_sample_index (int value, int start, int end)
{
    if (value < start)
        return start;
    if (value > end)
        return end;
    return value;
}

void __near print_sample_index (uint8_t x, uint8_t y, uint16_t index, uint8_t start, uint8_t end)
{
    if ((start <= index) && (index < end))
    {
        gotoxy (x, y + index - start);
        printf (
            "%03hX",
            (uint16_t) index + 1
        );
    }
}

void __near win_samples_draw_header (SCRWIN *self)
{
    textbackground (_black);
    textcolor (_white);
    gotoxy (LIST_X - 1 + 2, HEAD_Y);
    printf (
        "%s",
        "No. Title                    V. B. Rate  LT  LS    LE    L.    Memory     H."
    );
}

void __near win_samples_draw_line (SCRWIN *self, PCMSMP *sample, int index, uint8_t y)
{
    #define _BUF_SIZE 32
    #define _TITLE_LEN 24
    char buf[_BUF_SIZE];
    PCMSMPLOOP s_loop;
    char *s_loop_str;
    void *s_data;

    textbackground (_black);
    textcolor(_lightgray);
    gotoxy (LIST_X - 1 + 2, y);

    if (sample && pcmsmp_is_available (sample))
    {
        strncpy (buf, pcmsmp_get_title (sample), _TITLE_LEN);
        buf[_TITLE_LEN] = 0;
        printf (
            "%03hX %s",
            (uint16_t) index + 1,
            (char *) buf
        );
        gotoxy (LIST_X - 1 + 31, y);
        printf (
            "%02hhX %2hhu %5hu",
            (uint8_t) pcmsmp_get_volume (sample),
            (uint8_t) pcmsmp_get_bits (sample),
            (uint16_t) pcmsmp_get_rate (sample)
        );
        s_loop = pcmsmp_get_loop (sample);
        switch (s_loop)
        {
        case PCMSMPLOOP_NONE:
            s_loop_str = "off";
            break;
        case PCMSMPLOOP_FORWARD:
            s_loop_str = "fwd";
            break;
        case PCMSMPLOOP_BACKWARD:
            s_loop_str = "bck";
            break;
        case PCMSMPLOOP_PINGPONG:
            s_loop_str = "p-p";
            break;
        default:
            s_loop = PCMSMPLOOP_NONE;
            s_loop_str = "unk";
            break;
        }
        gotoxy (LIST_X - 1 + 43, y);
        if (s_loop != PCMSMPLOOP_NONE)
            snprintf (buf, _BUF_SIZE,
                "%s %5u %5u",
                (char *) s_loop_str,
                (uint16_t) pcmsmp_get_loop_start (sample),
                (uint16_t) pcmsmp_get_loop_end (sample)
            );
        else
            snprintf (buf, _BUF_SIZE,
                "%s     -     -",
                (char *) s_loop_str
            );
        printf (
            "%s %5hu",
            (char *) buf,
            (uint16_t) pcmsmp_get_length (sample)
        );
        if (pcmsmp_is_EM_data (sample))
            snprintf (buf, _BUF_SIZE,
                "E%04X:%04X %04X",
                (uint16_t) pcmsmp_get_EM_data_page (sample),
                (uint16_t) pcmsmp_get_EM_data_offset (sample),
                (uint16_t) pcmsmp_get_EM_data_handle (sample)
            );
        else
        {
            s_data = pcmsmp_get_data (sample);
            snprintf (buf, _BUF_SIZE,
                "D%04X:%04X    -",
                FP_SEG (s_data),
                FP_OFF (s_data)
            );
        }
        gotoxy (LIST_X - 1 + 65, y);
        printf (
            "%s",
            (char *) buf
        );
    }
    else
    {
        printf (
            "%03hX -",
            (uint16_t) index + 1
        );
        gotoxy (LIST_X - 1 + 31, y);
        printf (
            "%s",
            " -  -     - -       -     -     -     -:-       -"
        );
    }
    #undef _BUF_SIZE
    #undef _TITLE_LEN
}

void __near win_samples_draw_list (SCRWIN *self)
{
    struct win_samples_data_t *data;
    MUSMOD *track;
    PCMSMPLIST *samples;
    PCMSMP *sample;
    int index, count, i, y;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    track = data->track;
    samples = musmod_get_samples (track);
    index = data->page_start;
    count = data->page_end - data->page_start;
    y = LIST_Y;
    for (i = 0; i < count; i++)
    {
        sample = pcmsmpl_get (samples, index);
        win_samples_draw_line (self, sample, index, y);
        index++;
        y++;
    }
}

void __near win_samples_draw_footer (SCRWIN *self)
{
    struct win_samples_data_t *data;
    MUSMOD *track;
    PCMSMPLIST *samples;
    uint8_t y;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    track = data->track;
    samples = musmod_get_samples (track);

    y = scrwin_get_height (self);
    textbackground (_black);
    textcolor (_lightgray);
    gotoxy (LIST_X - 1 + 2, y);
    printf (
        "%s",
        "Samples:                   (Volume, Bits, Loop Type/Start/End, Length, Handle)"
    );
    textcolor (_yellow);
    gotoxy (LIST_X - 1 + 11, y);
    printf (
        "%3hu",
        (uint16_t) pcmsmpl_get_count (samples)
    );
}

void __near win_samples_draw_indexes (SCRWIN *self)
{
    struct win_samples_data_t *data;
    MUSMOD *track;
    PCMSMPLIST *samples;
    PCMSMP *sample;
    MIXCHNLIST *channels;
    MIXCHN *chn;
    int page_start, page_end, count, i, n, old_n;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    track = data->track;
    samples = musmod_get_samples (track);
    channels = data->channels;
    page_start = data->page_start;
    page_end = data->page_end;
    count = mixchnl_get_count (channels);
    for (i = 0; i < count; i++)
    {
        chn = mixchnl_get (channels, i);

        if (mixchn_is_playing (chn) && (mixchn_get_type (chn) == MIXCHNTYPE_PCM))
        {
            sample = mixchn_get_sample (chn);
            if (sample)
                n = pcmsmpl_indexof (samples, sample);
            else
                n = -1;
        }
        else
            n = -1;

        old_n = data->last_chn_smp[i];
        data->last_chn_smp[i] = n;
        if ((old_n != n) && (old_n != -1))
        {
            textbackground (_black);
            textcolor (_lightgray);
            print_sample_index (LIST_X - 1 + 2, LIST_Y, old_n, page_start, page_end);
        }
        if (n != -1)
        {
            textbackground (_lightgray);
            textcolor (_black);
            print_sample_index (LIST_X - 1 + 2, LIST_Y, n, page_start, page_end);
        }
    }
}

/* private events */

void __near win_samples_on_page_change (SCRWIN *self)
{
    struct win_samples_data_t *data;
    MUSMOD *track;
    PCMSMPLIST *samples;
    int count, page_start, page_end;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    track = data->track;
    samples = musmod_get_samples (track);

    count = data->lines_count;
    page_start = data->page_start;
    page_end = page_start + count;
    count = pcmsmpl_get_count (samples);
    if (page_end > count)
        page_end = count;
    data->page_end = page_end;
}

/* initialization */

bool __far win_samples_init (SCRWIN *self)
{
    struct win_samples_data_t *data;

    scrwin_init (self, "samples list window");
    _copy_vmt (self, __win_samples_vmt, SCRWINVMT);
    data = _new (struct win_samples_data_t);
    if (!data)
        return false;
    scrwin_set_data (self, data);
    memset (data, 0, sizeof (struct win_samples_data_t));
    return true;
}

/* public events */

void __far win_samples_on_resize (SCRWIN *self)
{
    struct win_samples_data_t *data;
    MUSMOD *track;
    PCMSMPLIST *samples;
    int lines_count;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    track = data->track;
    samples = musmod_get_samples (track);

    lines_count = scrwin_get_height (self) - 3;
    if (lines_count < 0)
        lines_count = 0;
    data->lines_count = lines_count;
    win_samples_on_page_change (self);
}

/* public methods */

void __far win_samples_set_track (SCRWIN *self, MUSMOD *value)
{
    struct win_samples_data_t *data;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    data->track = value;
}

void __far win_samples_set_channels (SCRWIN *self, MIXCHNLIST *value)
{
    struct win_samples_data_t *data;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    data->channels = value;
}

void __far win_samples_set_page_start (SCRWIN *self, int value)
{
    struct win_samples_data_t *data;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    data->page_start = value;
    win_samples_on_page_change (self);
}

void __far win_samples_draw (SCRWIN *self)
{
    if (scrwin_get_flags (self) & WINFL_FULLREDRAW)
    {
        textbackground (_black);
        textcolor (_lightgray);
        clrscr ();
        win_samples_draw_header (self);
        win_samples_draw_list (self);
        win_samples_draw_footer (self);
    }

    win_samples_draw_indexes (self);
}

bool __far win_samples_keypress (SCRWIN *self, char c)
{
    struct win_samples_data_t *data;
    MUSMOD *track;
    PCMSMPLIST *samples;
    int start, end, cur, add;

    data = (struct win_samples_data_t *) scrwin_get_data (self);
    track = data->track;
    samples = musmod_get_samples (track);
    start = 0;
    end = pcmsmpl_get_count (samples) - data->lines_count;
    if (end < 0)
        end = 0;
    switch (c)
    {
    case '<':
    case '>':
        add = data->lines_count;
        if (c == '<')
            add = -add;
        cur = check_sample_index (data->page_start + add, start, end);
        if (cur != data->page_start)
        {
            data->page_start = cur;
            win_samples_on_page_change (self);
            scrwin_set_flags (self, scrwin_get_flags (self) | WINFL_FULLREDRAW);
        }
        return true;
    default:
        return false;
    }
}

/* free */

void __far win_samples_free (SCRWIN *self)
{
    if (scrwin_get_data (self))
        _delete (scrwin_get_data (self));
}
