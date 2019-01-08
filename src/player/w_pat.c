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
#include "main/effects.h"
#include "main/muspat.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"
#include "player/screen.h"
#include "player/plays3m.h"

#include "player/w_pat.h"

void __far win_pattern_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 6, scrWidth, scrHeight,
        &win_pattern_draw,
        &win_pattern_keypress
    );
    startchn = 1;
}

void __near draw_channel_event (MUSPATCHNEVENT *event)
{
    uint8_t _ins, _vol, _cmd, _parm;
    char output[12];

    write_Note (event->data.note);

    _ins  = event->data.instrument;
    _vol  = event->data.note_volume;
    _cmd  = event->data.command;
    _parm = event->data.parameter;

    output[0] = ' ';
    if (_ins == CHN_INS_NONE)
    {
        output[1] = '.';
        output[2] = '.';
    }
    else
    if ((_ins >= CHN_INS_MIN) && (_ins <= CHN_INS_MAX))
    {
        _ins = _get_instrument (_ins) + 1;
        output[1] = '0' + (_ins / 10);
        output[2] = '0' + (_ins % 10);
    }
    else
    {
        output[1] = '?';
        output[2] = '?';
    }

    output[3] = ' ';
    if (_vol <= CHN_NOTEVOL_MAX)
    {
        output[4] = '0' + (_vol / 10);
        output[5] = '0' + (_vol % 10);
    }
    else
    if (_vol == CHN_NOTEVOL_NONE)
    {
        output[4] = '.';
        output[5] = '.';
    }
    else
    {
        output[4] = '?';
        output[5] = '?';
    }

    output[6] = ' ';
    if (_cmd == CHN_CMD_NONE)
    {
        output[7] = '.';
        output[8] = '.';
        output[9] = '.';
    }
    else
    {
        if (_cmd <= MAXEFF)
            output[7] = 'A' + _cmd - 1;
        else
            output[7] = '?';

        snprintf (& (output[8]), 2, "%02X", _parm);
    }
    output[10] = 0;
    printf ("%s", output);
}

void __near __pascal display_row(uint8_t ordr, uint8_t row)
{
    #define DISPLAY_COLUMNS 5
    MUSMOD *track;
    MUSPATLIST *patterns;
    MUSPAT *pat;
    MUSPATIO f;
    MUSPATROWEVENT e;
    MUSPATCHNEVENT line[DISPLAY_COLUMNS];
    MIXCHNLIST *channels;
    uint8_t i, c_start, c_end;

    track = mod_Track;
    patterns = musmod_get_patterns (track);
    channels = mod_Channels;
    pat = muspatl_get (patterns, Order[ordr]);

    c_start = startchn - 1;
    c_end = c_start + DISPLAY_COLUMNS;
    if (c_end >= mixchnl_get_count (channels))
        c_end = mixchnl_get_count (channels);

    for (i = 0; i < c_end - c_start; i++)
        muspatchnevent_clear (& (line [i]));

    muspatio_open (&f, pat, MUSPATIOMD_READ);

    if (muspat_is_data_packed (pat))
    {
        muspatio_seek (&f, row, 0);

        i = 0;
        while (!muspatio_is_end_of_row (&f))
        {
            muspatio_read (&f, &e);
            if ((e.channel >= c_start) && (e.channel < c_end))
                memcpy (& (line [e.channel - c_start]), & (e.event), sizeof (MUSPATCHNEVENT));
        }
    }
    else
    {
        muspatio_seek (&f, row, c_start);

        for (i = c_start; i < c_end ; i++)
        {
            muspatio_read (&f, &e);
            memcpy (& (line [i - c_start]), & (e.event), sizeof (MUSPATCHNEVENT));
        }
    }

    textbackground(_black);
    textcolor(_lightgray);
    printf(" %2hu ", row);
    textcolor(_darkgray);
    printf("\xb3");

    for (i = 0; i < DISPLAY_COLUMNS ; i++)
    {
        textcolor(_lightgray);

        if (startchn - 1 + i < mixchnl_get_count (channels))
            draw_channel_event (&(line[i]));
        else
            printf("             ");

        textcolor(_darkgray);
        printf("\xb3");
    }

    muspatio_close (&f);
    #undef DISPLAY_COLUMNS
}

void __near __pascal display_currow(void)
{
    display_row(playState_order, playState_row);
    printf(CRLF);
}

void __far win_pattern_draw(SCRWIN *self)
{
    //_win_pattern_draw(self);

    MUSMOD *track;
    MUSPATLIST *patterns;
    int i, j, currow_y;
    MUSPAT *pat;

    if (scrwin_is_created(self))
    {
        track = mod_Track;
        patterns = musmod_get_patterns (track);
        currow_y = scrwin_get_height(self);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            textbackground(_black);
            textcolor(_lightgray);
            clrscr();
            gotoxy(5, 1);
            textcolor(_darkgray);
            printf("\xda\xc4\xc4\xc4\xb4");
            for (i = 0; i < 5; i++)
            {
                textcolor(_darkgray);
                textcolor(_white);
                printf(" c%2hu ", startchn + i);
                textcolor(_darkgray);
                if (i < 4)
                    printf("\xc3\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xb4");
                else
                    printf("\xc3\xc4\xc4\xc4\xbf");
            }
            lastrow = playState_row;
            pat = muspatl_get (patterns, playState_pattern);
            if (muspat_is_EM_data(pat))
                muspat_map_EM_data(pat);
        }

        window(self->rect.x0, self->rect.y0+1, self->rect.x1, self->rect.y1);
        textbackground(_black);
        textcolor(_lightgray);

        if (playState_row + 1 < lastrow)
        {
            j = prevorder(playState_order);
            for (i = lastrow; i < 64; i++)
            {
                gotoxy(1, currow_y);
                printf(CRLF);
                gotoxy(1, currow_y);
                display_row(j, i);
            }
            // new pattern
            lastrow = 0;
            pat = muspatl_get (patterns, playState_pattern);
            if (muspat_is_EM_data(pat))
                muspat_map_EM_data(pat);
        }
        for (i = lastrow; i <= playState_row; i++)
        {
            gotoxy(1, currow_y);
            printf(CRLF);
            gotoxy(1, currow_y);
            display_row(playState_order, i);
        }
        lastrow = playState_row + 1;
    }
}

bool __far win_pattern_keypress(SCRWIN *self, char c)
{
    MIXCHNLIST *channels;

    channels = mod_Channels;

    if (scrwin_is_created(self))
    {
        if (c == 75)
        {
            if (startchn > 1)
            {
                startchn--;
                scrwin_set_flags(self, scrwin_get_flags(self) | WINFL_FULLREDRAW);
            }

            return true;
        }
        else
        if (c == 77)
        {
            if (startchn < mixchnl_get_count (channels))
            {
                startchn++;
                scrwin_set_flags(self, scrwin_get_flags(self) | WINFL_FULLREDRAW);
            }

            return true;
        }
    }

    return false;
}
