/* w_pat.c -- pattern window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
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

void __near __pascal display_row(uint8_t ordr, uint8_t row)
{
    MUSPAT *pat;
    uint8_t *p;
    uint8_t i;
    uint8_t _note, _ins, _vol, _cmd, _parm;
    char output[12];

    pat = muspatl_get(mod_Patterns, Order[ordr]);

    if (muspat_is_EM_data(pat))
        p = muspat_map_EM_data(pat);
    else
        p = muspat_get_data(pat);

    p = &(p[(row * mod_ChannelsCount + startchn - 1) * 5]);

    textbackground(_black);
    textcolor(_lightgray);
    printf(" %2hu ", row);
    textcolor(_darkgray);
    printf("\xb3");

    for (i = 0; i < 5 ; i++)
    {
        textcolor(_lightgray);

        if (i < mod_ChannelsCount)
        {
            _note = p[0];
            _ins  = p[1];
            _vol  = p[2];
            _cmd  = p[3];
            _parm = p[4];

            write_Note(_note);

            output[0] = ' ';
            if (_ins == 0)
            {
                output[1] = '.';
                output[2] = '.';
            }
            else
            if (_ins <= 99)
            {
                output[1] = '0' + (_ins / 10);
                output[2] = '0' + (_ins % 10);
            }
            else
            {
                output[1] = '?';
                output[2] = '?';
            }

            output[3] = ' ';
            if (_vol <= 64)
            {
                output[4] = '0' + (_vol / 10);
                output[5] = '0' + (_vol % 10);
            }
            else
            if (_vol < 255)
            {
                output[4] = '?';
                output[5] = '?';
            }
            else
            {
                output[4] = '.';
                output[5] = '.';
            }

            output[6] = ' ';
            if (_cmd == EFFIDX_NONE)
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

                sprintf(&(output[8]), "%02X", _parm);
            }
            output[10] = 0;
            printf("%s", output);
        }
        else
            printf("             ");

        textcolor(_darkgray);
        printf("\xb3");

        p += 5;
    }
}

void __near __pascal display_currow(void)
{
    display_row(playState_order, playState_row);
    printf(CRLF);
}

void __far win_pattern_draw(SCRWIN *self)
{
    //_win_pattern_draw(self);

    int i, j, currow_y;
    MUSPAT *pat;

    if (scrwin_is_created(self))
    {
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
            pat = muspatl_get(mod_Patterns, playState_pattern);
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
            pat = muspatl_get(mod_Patterns, playState_pattern);
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
            if (startchn < mod_ChannelsCount)
            {
                startchn++;
                scrwin_set_flags(self, scrwin_get_flags(self) | WINFL_FULLREDRAW);
            }

            return true;
        }
    }

    return false;
}
