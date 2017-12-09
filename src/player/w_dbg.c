/* w_dbg.c -- debug window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "main/fillvars.h"
#include "main/mixer.h"
#include "main/musins.h"
#include "main/muspat.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"
#include "player/screen.h"
#include "player/plays3m.h"

#include "player/w_dbg.h"

void __far win_debug_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 6, scrWidth, scrHeight,
        &win_debug_draw,
        NULL
    );
}

void __far win_debug_draw(SCRWIN *self)
{
    uint8_t y;
    MIXBUF *mixbuf;
    SNDDMABUF *sndbuf;
    DMABUF *dmabuf;

    if (scrwin_is_created(self))
    {
        textbackground(_black);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            textcolor(_lightgray);
            clrscr();
            gotoxy(5, 2);
            textcolor(_white);
            printf("Mixing buffer:");
            gotoxy(5, 4);
            textcolor(_lightgray);
            printf("Channels:");
            gotoxy(5, 5);
            printf("Rate:");
            gotoxy(5, 6);
            printf("Samples per channel:");
            gotoxy(5, 7);
            printf("Samples:");
            gotoxy(5, 8);
            printf("Tick samples per channel:");
            gotoxy(5, 9);
            printf("Tick samples per channel left: ");
            gotoxy(49, 2);
            textcolor(_white);
            printf("DMA buffer:");
            gotoxy(49, 4);
            textcolor(_lightgray);
            printf("Allocated:");
            gotoxy(49, 5);
            printf("Aligned:");
            gotoxy(49, 6);
            printf("Frame size:");
            gotoxy(49, 7);
            printf("Frames count:");
            gotoxy(49, 8);
            printf("Last frame:");
            gotoxy(49, 9);
            printf("Current frame:");
            gotoxy(49, 10);
            printf("Frames per second:");
            gotoxy(5, 12);
            textcolor(_lightgray);
            printf("Free DOS memory:            KiB");
            gotoxy(27, 12);
            textcolor(_yellow);
            printf("%5u", getFreeDOSMemory() >> 10);
            if (UseEMS)
            {
                gotoxy(5, 13);
                textcolor(_lightgray);
                printf("Free expanded memory:       KiB");
                gotoxy(27, 13);
                textcolor(_yellow);
                printf("%5u", getFreeEMMMemory());
                gotoxy(5, 14);
                textcolor(_lightgray);
                printf("Used expanded memory:       KiB");
                gotoxy(27, 14);
                textcolor(_yellow);
                printf("%5u", musinsl_get_used_EM(mod_Instruments) + muspatl_get_used_EM(mod_Patterns));
            }
            else
            {
                gotoxy(5, 13);
                printf("Expanded memory usage:");
                gotoxy(28, 13);
                textcolor(_yellow);
                printf("off");
            }
            y = scrwin_get_height(self) - 1;
            gotoxy(5, y);
            textcolor(_white);
            printf("Player version:");
            gotoxy(21, y);
            textcolor(_yellow);
            printf("%s", PLAYER_VERSION);
        }

        mixbuf = &mixBuf;
        sndbuf = &sndDMABuf;
        dmabuf = sndbuf->buf;
        gotoxy(36, 4);
        textcolor(_yellow);
        printf("%5u", mixbuf->channels);
        gotoxy(36, 5);
        printf("%5u", playState_rate);
        gotoxy(36, 6);
        printf("%5u", mixbuf->samples_per_channel);
        gotoxy(36, 7);
        printf("%5u", mixbuf->channels * mixbuf->samples_per_channel);
        gotoxy(36, 8);
        printf("%5u", playState_tick_samples_per_channel);
        gotoxy(36, 9);
        printf("%5u", playState_tick_samples_per_channel_left);
        gotoxy(68, 4);
        printf("%04X:%04X", FP_SEG(dmabuf->unaligned), FP_OFF(dmabuf->unaligned));
        gotoxy(68, 5);
        printf("%04X:%04X", FP_SEG(dmabuf->data), FP_OFF(dmabuf->data));
        gotoxy(72, 6);
        printf("%5u", sndbuf->frameSize);
        gotoxy(72, 7);
        printf("%5u", sndbuf->framesCount);
        gotoxy(72, 8);
        printf("%5u", sndbuf->frameLast);
        gotoxy(72, 9);
        printf("%5u", sndbuf->frameActive);
        gotoxy(72, 10);
        printf("%5u", playOption_FPS);
    }
}
