/* w_chn.c -- channels window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "main/mixchn.h"
#include "main/effects.h"
#include "player/screen.h"
#include "player/plays3m.h"

#include "player/w_chn.h"

static const char *CHANTYPES[5] =
{
    "--", "LT", "RT", "AM", "AD"
};

void __far win_channels_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 6, scrWidth, scrHeight,
        &win_channels_draw,
        NULL
    );
}

#define _EFFECT_DESC_MAX 40

void __far win_channels_draw(SCRWIN *self)
{
    uint8_t i, count;
    MIXCHN *chn;
    uint8_t type;
    uint8_t cmd;
    MIXCHNFLAGS flags;
    char flagsstr[6];
    char notestr[4];
    char effectstr[_EFFECT_DESC_MAX];
    uint32_t step;

    if (scrwin_is_created(self))
    {
        textbackground(_black);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            textcolor(_white);
            clrscr();
            printf("Ch Type Flags Inst Note  Period  Step   Vol Effect");
        }

        textcolor(_lightgray);
        i = 0;
        count = 0;
        while (i < UsedChannels)
        {
            chn = &Channel[i];
            type = mixchn_get_type(chn);
            flags = mixchn_get_flags(chn);
            if  ((type < 3)
            &&  ((UsedChannels < 17)
            ||   (type > 0)))
            {
                count++;

                flagsstr[0] = '-';
                flagsstr[1] = '-';
                flagsstr[2] = '-';
                flagsstr[3] = '-';
                flagsstr[4] = '-';
                flagsstr[5] = 0;
                if (flags & MIXCHNFL_ENABLED)
                    flagsstr[0] = 'e';
                if (flags & MIXCHNFL_PLAYING)
                    flagsstr[1] = 'p';
                if (flags & MIXCHNFL_MIXING)
                    flagsstr[2] = 'm';
                if (chn->bSmpFlags & SMPFLAG_LOOP)
                    flagsstr[3] = 'l';
                if (chn->bEffFlags & EFFFLAG_CONTINUE)
                    flagsstr[4] = 'c';

                if (mixchn_is_enabled(chn))
                    get_note_name(notestr, chn->bNote);
                else
                {
                    notestr[0] = '.';
                    notestr[1] = '.';
                    notestr[2] = '.';
                    notestr[3] = 0;
                }

                cmd = mixchn_get_command(chn);
                if (cmd && (cmd <= MAXEFF))
                    chn_get_effect_desc(chn, effectstr, _EFFECT_DESC_MAX);
                else
                    effectstr[0] = 0;

                gotoxy(1, 2 + i);
                step = mixchn_get_sample_step(chn);
                printf("%2hu (%s) %s (%2hu) [%s] %5u %4X.%04X %2hu %s",
                    i+1,
                    CHANTYPES[type],
                    &flagsstr,
                    mixchn_get_instrument_num(chn),
                    &notestr,
                    mixchn_get_sample_period(chn),
                    (uint16_t)(step >> 16),
                    (uint16_t)(step & 0xffff),
                    mixchn_get_sample_volume(chn),
                    &effectstr
                );
                clreol();
            }
            i++;
        }

        if (!count)
            printf("No channels to mix.");
    }
}

#undef _EFFECT_DESC_MAX
