/* w_dbg.c -- debug window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$player$w_dbg$*"
#endif

#include <stddef.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "common.h"
#include "main/musins.h"
#include "main/muspat.h"
#include "main/musmod.h"
#include "main/mixer.h"
#include "main/musmodps.h"
#include "main/fillvars.h"
#include "main/s3mplay.h"
#include "player/screen.h"
#include "player/plays3m.h"
#include "player/w_dbg.h"

/*
void __far win_debug_on_resize (SCRWIN *self);
*/
void __far win_debug_draw (SCRWIN *self);
/*
void __far win_debug_keypress (SCRWIN *self, char key);
*/
void __far win_debug_free (SCRWIN *self);

static const SCRWINVMT __win_debug_vmt =
{
    /*
    &win_debug_on_resize,
    */
    NULL,
    &win_debug_draw,
    /*
    &win_debug_keypress,
    */
    NULL,
    &win_debug_free
};

/* private data */

typedef struct win_debug_data_t
{
    MUSPLAYER *player;
    MUSMOD *track;
    PLAYSTATE *ps;
};

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

/* private methods */

/* public methods */

bool __far win_debug_init (SCRWIN *self)
{
    struct win_debug_data_t *data;

    scrwin_init (self, "debug window");
    _copy_vmt (self, __win_debug_vmt, SCRWINVMT);
    data = _new (struct win_debug_data_t);
    if (!data)
        return false;
    scrwin_set_data (self, data);
    memset (data, 0, sizeof (struct win_debug_data_t));
    return true;
}

void __far win_debug_set_player (SCRWIN *self, MUSPLAYER *value)
{
    struct win_debug_data_t *data;

    data = (struct win_debug_data_t *) scrwin_get_data (self);
    data->player = value;
}

void __far win_debug_set_track (SCRWIN *self, MUSMOD *value)
{
    struct win_debug_data_t *data;

    data = (struct win_debug_data_t *) scrwin_get_data (self);
    data->track = value;
}

void __far win_debug_set_play_state (SCRWIN *self, PLAYSTATE *value)
{
    struct win_debug_data_t *data;

    data = (struct win_debug_data_t *) scrwin_get_data (self);
    data->ps = value;
}

/*
void __far win_debug_on_resize (SCRWIN *self)
{
}
*/

void __far win_debug_draw(SCRWIN *self)
{
    struct win_debug_data_t *data;
    MUSPLAYER *player;
    MUSMOD *track;
    PLAYSTATE *ps;
    PCMSMPLIST *samples;
    MUSINSLIST *instruments;
    MUSPATLIST *patterns;
    uint8_t y;
    MIXER *mixer;
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

    data = (struct win_debug_data_t *) scrwin_get_data (self);
    player = data->player;
    track = data->track;
    ps = player_get_play_state (player);
    samples = musmod_get_samples (track);
    instruments = musmod_get_instruments (track);
    patterns = musmod_get_patterns (track);

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

        if (player_is_EM_in_use (player))
        {
            gotoxy (MEMORY_V - 5, MEMORY_Y + 2); printf ("%5u", pcmsmpl_get_used_EM (samples));
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

    mixer = player_get_mixer (player);
    mixbuf = mixer_get_mixbuf (mixer);
    sndbuf = player_get_sound_buffer (player);
    dmabuf = sndbuf->buf;
    out_channels = mixbuf_get_channels (mixbuf);
    out_rate = ps->rate;
    out_samples_per_channel = mixbuf_get_samples_per_channel (mixbuf);
    out_tick_samples_per_channel = ps->tick_samples_per_channel;
    out_tick_samples_per_channel_left = ps->tick_samples_per_channel_left;
    out_dma_buf_unaligned = dmabuf->unaligned;
    out_dma_buf = dmabuf->data;
    out_frame_size = sndbuf->frameSize;
    out_frames_count = sndbuf->framesCount;
    out_frame_last = sndbuf->frameLast;
    out_frame_active = sndbuf->frameActive;
    out_fps = (long) player_get_output_rate (player) * get_sample_format_width (& (sndbuf->format)) / sndbuf->frameSize;

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

/*
void __far win_debug_keypress (SCRWIN *self, char key)
{
}
*/

/* free */

void __far win_debug_free (SCRWIN *self)
{
    if (scrwin_get_data (self))
        _delete (scrwin_get_data (self));
}
