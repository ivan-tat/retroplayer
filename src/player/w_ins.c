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

void __far win_instruments_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 6, scrWidth, scrHeight,
        &win_instruments_draw,
        &win_instruments_keypress
    );
}

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

void __near write_no(uint16_t num, uint8_t height, uint8_t line)
{
    gotoxy(2 + (num / height) * line, 3 + (num % height));
    printf("%2hu", num + 1);
}

void __far win_instruments_draw(SCRWIN *self)
{
    MUSMOD *track;
    MUSINSLIST *instruments;
    PCMSMPLIST *samples;
    int i;
    uint8_t n;
    MIXCHNLIST *channels;
    MIXCHN *chn;
    uint8_t height, page_height, line_len;
    char data[6];

    if (scrwin_is_created(self))
    {
        track = mod_Track;
        instruments = musmod_get_instruments (track);
        channels = mod_Channels;

        textbackground(_black);
        textcolor(_lightgray);

        height = scrwin_get_height(self);
        page_height = height - 4;
        line_len = 11;

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            clrscr();
            gotoxy(2, 1);
            printf("Sample positions in memory: (D: DOS memory, E: Expanded memory)");

            textcolor(_white);
            for (i = 1; i <= 99; i++)
            {
                write_no(i - 1, page_height, line_len);
                posstring(&data, i);
                printf(": %s", &data);
            }
        }

        for (i = 0; i < mixchnl_get_count (channels); i++)
        {
            chn = mixchnl_get (channels, i);

            if (mixchn_is_playing (chn) && (mixchn_get_type (chn) == MIXCHNTYPE_PCM))
                n = mixchn_get_instrument_num(chn);
            else
                n = 0;

            if ((chnlastinst[i] != n) && (chnlastinst[i] > 0))
            {
                textbackground(_black);
                textcolor(_white);
                write_no(chnlastinst[i] - 1, page_height, line_len);
            }
            chnlastinst[i] = n;
            if (n)
            {
                textbackground(_white);
                textcolor(_black);
                write_no(n - 1, page_height, line_len);
            }
        }
    }
}

bool __far win_instruments_keypress(SCRWIN *self, char c)
{
    return false;
}
