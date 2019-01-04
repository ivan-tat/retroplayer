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

#define MIXBUF_Y    2
#define MIXBUF_X    3
#define MIXBUF_V    38 + 1

#define DMABUF_Y    2
#define DMABUF_X    49
#define DMABUF_V    74 + 1

#define MEMORY_Y    11
#define MEMORY_X    3
#define MEMORY_V    17 + 1

#define VERSION_X   3
#define VERSION_V   19

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
    MUSINSLIST *instruments;
    MUSPATLIST *patterns;
    uint8_t y;
    MIXBUF *mixbuf;
    SNDDMABUF *sndbuf;
    DMABUF *dmabuf;
    unsigned out_channels;
    unsigned out_rate;
    unsigned out_samples_per_channel;
    unsigned out_tick_samples_per_channel;
    unsigned out_tick_samples_per_channel_left;
    void    *out_dma_buf_unaligned;
    void    *out_dma_buf;
    unsigned out_frame_size;
    unsigned out_frames_count;
    unsigned out_frame_last;
    unsigned out_frame_active;
    unsigned out_fps;

    if (scrwin_is_created(self))
    {
        instruments = mod_Instruments;
        patterns = mod_Patterns;

        textbackground (_black);

        if (scrwin_get_flags (self) & WINFL_FULLREDRAW)
        {
            y = scrwin_get_height (self) - 1;

            textcolor (_lightgray);
            clrscr ();

            textcolor (_white);

            gotoxy (MIXBUF_X, MIXBUF_Y); printf ("%s", "Mixing buffer:");
            gotoxy (DMABUF_X, DMABUF_Y); printf ("%s", "DMA buffer:");
            gotoxy (MEMORY_X, MEMORY_Y); printf ("%s", "Memory usage:");
            gotoxy (VERSION_X, y); printf ("%s", "Player version:");

            textcolor (_lightgray);

            gotoxy (MIXBUF_X, MIXBUF_Y + 2); printf ("%s", "Channels:");
            gotoxy (MIXBUF_X, MIXBUF_Y + 3); printf ("%s", "Rate:");
            gotoxy (MIXBUF_X, MIXBUF_Y + 4); printf ("%s", "Samples per channel:");
            gotoxy (MIXBUF_X, MIXBUF_Y + 5); printf ("%s", "Samples:");
            gotoxy (MIXBUF_X, MIXBUF_Y + 6); printf ("%s", "Tick samples per channel:");
            gotoxy (MIXBUF_X, MIXBUF_Y + 7); printf ("%s", "Tick samples per channel left: ");

            gotoxy (DMABUF_X, DMABUF_Y + 2); printf ("%s", "Allocated:");
            gotoxy (DMABUF_X, DMABUF_Y + 3); printf ("%s", "Aligned:");
            gotoxy (DMABUF_X, DMABUF_Y + 4); printf ("%s", "Frame size:");
            gotoxy (DMABUF_X, DMABUF_Y + 5); printf ("%s", "Frames count:");
            gotoxy (DMABUF_X, DMABUF_Y + 6); printf ("%s", "Last frame:");
            gotoxy (DMABUF_X, DMABUF_Y + 7); printf ("%s", "Current frame:");
            gotoxy (DMABUF_X, DMABUF_Y + 8); printf ("%s", "Frames per second:");

            gotoxy (MEMORY_X, MEMORY_Y + 2); printf ("%s", "Samples:        KiB EM");
            gotoxy (MEMORY_X, MEMORY_Y + 3); printf ("%s", "Patterns:       KiB EM");
            gotoxy (MEMORY_X, MEMORY_Y + 4); printf ("%s", "Free:           KiB EM");
            gotoxy (MEMORY_X, MEMORY_Y + 5); printf ("%s", "Free:           KiB DOS");

            textcolor (_yellow);

            if (UseEMS)
            {
                gotoxy (MEMORY_V - 5, MEMORY_Y + 2); printf ("%5u", musinsl_get_used_EM (instruments));
                gotoxy (MEMORY_V - 5, MEMORY_Y + 3); printf ("%5u", muspatl_get_used_EM (patterns));
                gotoxy (MEMORY_V - 5, MEMORY_Y + 4); printf ("%5u", getFreeEMMMemory ());
            }
            else
            {
                gotoxy (MEMORY_V - 4, MEMORY_Y + 2); printf ("%s", "none");
                gotoxy (MEMORY_V - 4, MEMORY_Y + 3); printf ("%s", "none");
                gotoxy (MEMORY_V - 4, MEMORY_Y + 4); printf ("%s", "none");
            }

            gotoxy (MEMORY_V - 5, MEMORY_Y + 5); printf ("%5u", getFreeDOSMemory () >> 10);
            gotoxy (VERSION_V, y); printf ("%s", PLAYER_VERSION);
        }

        mixbuf = &mixBuf;
        sndbuf = &sndDMABuf;
        dmabuf = sndbuf->buf;
        out_channels = mixbuf_get_channels (mixbuf);
        out_rate = playState_rate;
        out_samples_per_channel = mixbuf_get_samples_per_channel (mixbuf);
        out_tick_samples_per_channel = playState_tick_samples_per_channel;
        out_tick_samples_per_channel_left = playState_tick_samples_per_channel_left;
        out_dma_buf_unaligned = dmabuf->unaligned;
        out_dma_buf = dmabuf->data;
        out_frame_size = sndbuf->frameSize;
        out_frames_count = sndbuf->framesCount;
        out_frame_last = sndbuf->frameLast;
        out_frame_active = sndbuf->frameActive;
        out_fps = playOption_FPS;

        textcolor (_yellow);

        gotoxy (MIXBUF_V - 5, MIXBUF_Y + 2); printf ("%5u", out_channels);
        gotoxy (MIXBUF_V - 5, MIXBUF_Y + 3); printf ("%5u", out_rate);
        gotoxy (MIXBUF_V - 5, MIXBUF_Y + 4); printf ("%5u", out_samples_per_channel);
        gotoxy (MIXBUF_V - 5, MIXBUF_Y + 5); printf ("%5u", out_channels * out_samples_per_channel);
        gotoxy (MIXBUF_V - 5, MIXBUF_Y + 6); printf ("%5u", out_tick_samples_per_channel);
        gotoxy (MIXBUF_V - 5, MIXBUF_Y + 7); printf ("%5u", out_tick_samples_per_channel_left);
        gotoxy (DMABUF_V - 9, DMABUF_Y + 2); printf ("%04X:%04X", FP_SEG (out_dma_buf_unaligned), FP_OFF (out_dma_buf_unaligned));
        gotoxy (DMABUF_V - 9, DMABUF_Y + 3); printf ("%04X:%04X", FP_SEG (out_dma_buf), FP_OFF (out_dma_buf));
        gotoxy (DMABUF_V - 5, DMABUF_Y + 4); printf ("%5u", out_frame_size);
        gotoxy (DMABUF_V - 5, DMABUF_Y + 5); printf ("%5u", out_frames_count);
        gotoxy (DMABUF_V - 5, DMABUF_Y + 6); printf ("%5u", out_frame_last);
        gotoxy (DMABUF_V - 5, DMABUF_Y + 7); printf ("%5u", out_frame_active);
        gotoxy (DMABUF_V - 5, DMABUF_Y + 8); printf ("%5u", out_fps);
    }
}
