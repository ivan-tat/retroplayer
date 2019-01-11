/* w_ins.c -- instruments window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "main/musins.h"
#include "main/musmod.h"
#include "main/mixchn.h"
#include "main/s3mvars.h"
#include "player/screen.h"
#include "player/plays3m.h"

#include "player/w_ins.h"

// private variables
static uint8_t data_lines_count = 0;
static uint8_t data_page_start = 0;

// private methods
void __near on_size_change (SCRWIN *self)
{
    uint8_t height;

    height = scrwin_get_height (self);
    if (height >= 3)
        data_lines_count = height - 3;
    else
        data_lines_count = 0;
}

void __far win_instruments_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 6, scrWidth, scrHeight,
        &win_instruments_draw,
        &win_instruments_keypress
    );
    on_size_change (self);
    data_page_start = 0;
}

/*
void __near posstring(char *__dest, uint8_t i)
{
    MUSMOD *track;
    MUSINSLIST *instruments;
    PCMSMPLIST *samples;
    MUSINS *ins;
    PCMSMP *smp;
    uint16_t data;

    track = mod_Track;
    instruments = musmod_get_instruments (track);
    samples = musmod_get_samples (track);
    ins = musinsl_get (instruments, i - 1);
    if (musins_get_type (ins) == MUSINST_PCM)
    {
        smp = musins_get_sample (ins);
        if (pcmsmp_is_EM_data (smp))
        {
            __dest[0] = 'E';
            data = pcmsmp_get_EM_data_page (smp);
        }
        else
        {
            __dest[0] = 'D';
            data = FP_SEG (pcmsmp_get_data (smp));
        }
        snprintf (& (__dest[1]), 4, "%04X", data);
    }
    else
        memcpy(__dest, "     ", 6);
}
*/

void __near write_no (uint8_t x, uint8_t y, uint16_t index, uint8_t start, uint8_t end)
{
    if ((index >= start) && (index < end))
    {
        gotoxy (x, y + index - start);
        printf ("%02hhX", index + 1);
    }
}

#define HEAD_Y 1
#define LIST_X 1
#define LIST_Y (HEAD_Y + 1)

void __far win_instruments_draw(SCRWIN *self)
{
    #define INS_TITLE_LEN 58
    MUSMOD *track;
    MUSINSLIST *instruments;
    MUSINS *instrument;
    MIXCHNLIST *channels;
    MIXCHN *chn;
    uint8_t i_index;
    char i_title[INS_TITLE_LEN + 1];
    MUSINSTYPE i_type;
    char *i_type_str;
    uint8_t i_samples;
    uint8_t i_volume;
    uint8_t i_note_volume;
    int i, n, old_n, count;
    uint8_t y;
    uint8_t index_count, index_start, index_end;
    uint8_t line_color;

    if (scrwin_is_created(self))
    {
        track = mod_Track;
        instruments = musmod_get_instruments (track);
        channels = mod_Channels;

        textbackground(_black);
        textcolor(_lightgray);

        index_count = data_lines_count;
        index_start = data_page_start;
        index_end = index_start + index_count;
        count = musinsl_get_count (instruments);
        if (index_end > count)
            index_end = count;
        count = index_end - index_start;

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            clrscr ();
            textcolor (_white);
            gotoxy (LIST_X, HEAD_Y);
            printf ("%s", " No Title                                                      Type    SN IV NV ");

            i_index = index_start;
            for (i = 0; i < count; i++)
            {
                instrument = musinsl_get (instruments, i_index);
                strncpy (i_title, musins_get_title (instrument), INS_TITLE_LEN);
                i_title[INS_TITLE_LEN] = 0;
                i_type = musins_get_type (instrument);

                switch (i_type)
                {
                case MUSINST_EMPTY:
                    line_color = _darkgray;
                    i_type_str = "none   ";
                    break;
                case MUSINST_PCM:
                    line_color = _lightgray;
                    i_type_str = "pcm  ";
                    break;
                case MUSINST_ADLIB:
                    line_color = _darkgray;
                    i_type_str = "adlib  ";
                    break;
                default:
                    line_color = _darkgray;
                    i_type_str = "unknown";
                    break;
                }

                y = LIST_Y + i;

                textcolor (line_color);
                gotoxy (LIST_X - 1 + 2, y); printf ("%02hhX %s %s", i_index + 1, i_title);
                gotoxy (LIST_X - 1 + 64 , y); printf ("%s", i_type_str);
                gotoxy (LIST_X - 1 + 72, y);

                switch (i_type)
                {
                case MUSINST_PCM:
                    printf ("%2hhu %02hhX %02hhX",
                        1,
                        musins_get_volume (instrument),
                        musins_get_note_volume (instrument)
                    );
                    break;
                default:
                    printf ("-- -- --");
                    break;
                }
                i_index++;
            }

            y = scrwin_get_height (self) + 1;
            textcolor (_lightgray);
            gotoxy (LIST_X - 1 + 2, y);
            printf ("%s", "Instruments:");
            gotoxy (LIST_X - 1 + 30, y);
            printf ("%s", "(SN: samples count, IV/NV: instrument/note volume)");
            textcolor (_yellow);
            gotoxy (LIST_X - 1 + 15, y);
            printf ("%3u", musinsl_get_count (instruments));
        }

        count = mixchnl_get_count (channels);
        for (i = 0; i < count; i++)
        {
            chn = mixchnl_get (channels, i);

            if (mixchn_is_playing (chn) && (mixchn_get_type (chn) == MIXCHNTYPE_PCM))
                n = mixchn_get_instrument_num(chn);
            else
                n = 0;

            old_n = chnlastinst[i];
            chnlastinst[i] = n;
            if ((old_n != n) && (old_n != CHN_INS_NONE))
            {
                textbackground(_black);
                textcolor(_lightgray);
                write_no (LIST_X - 1 + 2, LIST_Y, old_n - 1, index_start, index_end);
            }
            if (n != CHN_INS_NONE)
            {
                textbackground(_lightgray);
                textcolor(_black);
                write_no (LIST_X - 1 + 2, LIST_Y, n - 1, index_start, index_end);
            }
        }
    }
    #undef INS_TITLE_LEN
}

int __near check_bounds (int value, int start, int end)
{
    if (value < start)
        return start;
    if (value > end)
        return end;
    return value;
}

bool __far win_instruments_keypress(SCRWIN *self, char c)
{
    MUSMOD *track;
    MUSINSLIST *instruments;
    int count, start, end, cur;
    bool f;

    track = mod_Track;
    instruments = musmod_get_instruments (track);
    count = musinsl_get_count (instruments);
    start = 0;
    end = count - data_lines_count;
    if (end < 0)
        end = 0;
    switch (c)
    {
    case '<':
        cur = check_bounds (data_page_start - data_lines_count, start, end);
        f = true;
        break;
    case '>':
        cur = check_bounds (data_page_start + data_lines_count, start, end);
        f = true;
        break;
    default:
        f = false;
        break;
    }

    if (f && (cur != data_page_start))
    {
        data_page_start = cur;
        scrwin_set_flags (self, scrwin_get_flags (self) | WINFL_FULLREDRAW);
    }

    return f;
}
