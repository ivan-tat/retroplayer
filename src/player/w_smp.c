/* w_smp.c -- samples window methods.

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
#include "main/mixchn.h"
#include "player/screen.h"
#include "player/plays3m.h"

#include "player/w_smp.h"

static uint8_t samplepage;

void __far win_samples_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 6, scrWidth, scrHeight,
        &win_samples_draw,
        &win_samples_keypress
    );
}

bool __near _is_sample_visible(uint8_t i, uint8_t page_height)
{
    uint8_t n;

    n = chnlastinst[i];
    return (n && (samplepage * page_height < n) && (n < (samplepage + 1) * page_height + 1));
}

void __near print_sample_num(uint8_t i, uint8_t first_line, uint8_t page_height)
{
    uint8_t n;

    n = chnlastinst[i];
    gotoxy(1, first_line + n - 1 - samplepage * page_height);
    printf("%3hu.", n);
}

void __far win_samples_draw(SCRWIN *self)
{
    uint8_t i, j, k, n;
    uint8_t page_height, height, first_line;
    MUSINS *ins;
    MIXCHN *chn;
    char title[30];

    if (scrwin_is_created(self))
    {
        textbackground(_black);
        textcolor(_white);

        height = scrwin_get_height(self);
        page_height = height - 1;
        first_line = 2;

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            clrscr();
            textbackground(_black);
            textcolor(_white);
            printf(" ##. Title                      Bits Rate Loop Beg   End   Len");
            clreol();
            gotoxy(1, 2);

            if (page_height <= InsNum)
            {
                samplepage = 0;
                i = 0;
            }
            else
            {
                i = samplepage * page_height;
                if (i >= InsNum)
                {
                    samplepage = InsNum / page_height;
                    if (samplepage)
                        samplepage--;
                    i = samplepage * page_height;
                }
            }

            height = scrwin_get_height(self);
            for (j = 1; j < height; j++)
                if (i < InsNum)
                {
                    ins = musinsl_get(mod_Instruments, i);

                    strncpy(&title, musins_get_title(ins), MUSINS_TITLE_LENGTH_MAX);

                    for (k = strlen(&title); k < MUSINS_TITLE_LENGTH_MAX - 1; k++)
                        title[k] = ' ';

                    title[MUSINS_TITLE_LENGTH_MAX - 1] = 0;

                    if (musins_get_type(ins) == MUSINST_PCM)
                        textcolor(_lightgray);
                    else
                        textcolor(_darkgray);

                    if (j > 1)
                        printf(CRLF);

                    printf(
                        " %2hu. %s %2hu %5u %s %5u %5u %5u",
                        i + 1,
                        title,
                        (uint8_t)8,
                        (uint16_t)musins_get_rate(ins),
                        musins_is_looped(ins) ? "On " : "Off",
                        (uint16_t)musins_get_loop_start(ins),
                        (uint16_t)musins_get_loop_end(ins),
                        (uint16_t)musins_get_length(ins)
                    );

                    i++;
                }
        }

        textbackground(_black);
        textcolor(_white);
        for (i = 0; i < UsedChannels; i++)
        {
            chn = &Channel[i];

            if (mixchn_is_playing(chn) && (mixchn_get_type(chn) > 0))
                n = mixchn_get_instrument_num(chn);
            else
                n = 0;

            if ((chnlastinst[i] != n) && _is_sample_visible(i, page_height))
            {
                textbackground(_black);
                textcolor(_lightgray);
                print_sample_num(i, first_line, page_height);
            }

            chnlastinst[i] = n;

            if (_is_sample_visible(i, page_height))
            {
                textbackground(_lightgray);
                textcolor(_black);
                print_sample_num(i, first_line, page_height);
            }
        }
    }
}

bool __far win_samples_keypress(SCRWIN *self, char c)
{
    uint8_t page_height;

    if (scrwin_is_created(self))
    {
        if (c == '[')
        {
            if (samplepage > 0)
            {
                samplepage--;
                scrwin_set_flags(self, scrwin_get_flags(self) | WINFL_FULLREDRAW);
            }
            return true;
        }
        else
        if (c == ']')
        {
            page_height = scrwin_get_height(self);
            if ((samplepage + 1) * page_height + 1 <= InsNum)
            {
                samplepage++;
                scrwin_set_flags(self, scrwin_get_flags(self) | WINFL_FULLREDRAW);
            }
            return true;
        }
    }

    return false;
}
