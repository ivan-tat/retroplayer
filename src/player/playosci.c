/* plays3m.c -- full featured tracked music player.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal.h"
#include "startup.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "cc/conio.h"
#include "hw/hwowner.h"
#include "hw/pic.h"
#include "hw/dma.h"
#include "hw/sb/sbctl.h"
#include "hw/vbios.h"
#include "hw/vga.h"
#include "main/s3mvars.h"
#include "main/fillvars.h"
#include "main/musmod.h"
#include "main/mixchn.h"
#include "main/s3mplay.h"

#include "playosci.h"

#define def_rate 45454
#define def_stereo false
#define def_16bits false
#define def_lq false

#define COL_BACKGROUND 1
#define COL_FOREGROUND 14

static uint16_t opt_rate = 0;
static bool opt_stereo = false;
static bool opt_16bits = false;
static bool opt_lq = false;
static char opt_filename[pascal_String_size] = { 0 };
static void *bufdata = NULL;
static uint8_t scr[2][320] = { 0 };

static MUSMOD *song_track;

void __near draw_channels_volume(void)
{
    int16_t i;
    MIXCHNLIST *channels;
    MIXCHN *chn;

    channels = mod_Channels;
    for (i = 0; i < mixchnl_get_count (channels); i++)
    {
        chn = mixchnl_get (channels, i);
        vga_bar (320 * 170 + i * 15 + 10, 10, mixchn_is_playing (chn) ? mixchn_get_note_volume (chn) : 0);
    }
}

void __near get_current_sample1(int16_t *v)
{
    uint16_t pos;
    uint16_t value;
    void *p;

    pos = player_get_buffer_pos ();
    if (opt_16bits)
    {
        p = &((uint16_t *)bufdata)[pos >> 1];
        value = *(uint16_t *)p;
    }
    else
    {
        p = &((uint8_t *)bufdata)[pos];
        value = (*(uint8_t *)p) << 8;
    }
    if (opt_16bits) // FIXME: signed
        *v = value;
    else
        *v = value - 0x8000;
}

void __near get_current_sample2(int16_t *v)
{
    uint16_t pos;
    uint16_t values[2];
    void *p;

    pos = player_get_buffer_pos ();
    if (opt_16bits)
    {
        p = &((uint16_t *)bufdata)[(pos >> 1) & 0xfffe];
        values[0] = ((uint16_t *)p)[0];
        values[1] = ((uint16_t *)p)[1];
    }
    else
    {
        p = &((uint8_t *)bufdata)[pos & 0xfffe];
        values[0] = ((uint8_t *)p)[0] << 8;
        values[1] = ((uint8_t *)p)[1] << 8;
    }
    if (opt_16bits) // FIXME: signed
    {
        v[0] = values[0];
        v[1] = values[1];
    }
    else
    {
        v[0] = values[0] - 0x8000;
        v[1] = values[1] - 0x8000;
    }
}

void __near update_osci_mono(void)
{
    int16_t i, s;

    get_current_sample1 (&s);
    s = (s >> 9) + 64;
    for (i = 0; i < 318; i++)
    {
        vga_line (
            i,      (200 - 128) / 2 + scr[0][i],
            i + 1,  (200 - 128) / 2 + scr[0][i + 1],
            COL_BACKGROUND
        );
        scr[0][i] = s;
        get_current_sample1 (&s);
        s = (s >> 9) + 64;
        vga_line (
            i,      (200 - 128) / 2 + scr[0][i],
            i + 1,  (200 - 128) / 2 + s,
            COL_FOREGROUND
        );
    }
    scr[0][319] = s;
}

void __near update_osci_stereo(void)
{
    int16_t i, s[2];

    get_current_sample2 (&s);
    s[0] = (s[0] >> 9) + 64;
    s[1] = (s[1] >> 9) + 64;
    for (i = 0; i < 318; i++)
    {
        vga_line (
            i,      (200 - 128) / 2 + scr[0][i],
            i + 1,  (200 - 128) / 2 + scr[0][i + 1],
            COL_BACKGROUND
        );
        vga_line (
            i,      (200 - 128) / 2 + 64 + scr[1][i],
            i + 1,  (200 - 128) / 2 + 64 + scr[1][i + 1],
            COL_BACKGROUND
        );
        scr[0][i] = s[0];
        scr[1][i] = s[1];
        get_current_sample2 (&s);
        s[0] = (s[0] >> 9) + 64;
        s[1] = (s[1] >> 9) + 64;
        vga_line (
            i,      (200 - 128) / 2 + scr[0][i],
            i + 1,  (200 - 128) / 2 + s[0],
            COL_FOREGROUND
        );
        vga_line (
            i,      (200 - 128) / 2 + 64 + scr[1][i],
            i + 1,  (200 - 128) / 2 + 64 + s[1],
            COL_FOREGROUND
        );
    }
    scr[0][319] = s[0];
    scr[1][319] = s[1];
}

void __far playosci_main (void)
{
    register_debug ();
    register_hwowner ();
    register_pic ();
    register_dma ();
    register_ems ();
    register_vga ();
    register_sbctl ();
    register_s3mplay ();

    console_init ();

    if (!environ_init ())
    {
        printf ("Failed to setup DOS environment variables." CRLF);
        return;
    }
    textbackground (_black);
    textcolor (_lightgray);
    clrscr ();

    printf (
        "Simple music player with oscillator for DOS, version %s." CRLF
        "Originally written by Andre Baresel, 1994-1995." CRLF
        "Modified by Ivan Tatarinov <ivan-tat@ya.ru>, 2016-2018." CRLF
        "This is free and unencumbered software released into the public domain." CRLF
        "For more information, please refer to <http://unlicense.org>." CRLF,
        PLAYER_VERSION
    );

    opt_rate = def_rate;
    opt_stereo = def_stereo;
    opt_16bits = def_16bits;
    opt_lq = def_lq;

    drawseg = 0xa000;

    custom_argv (opt_filename, pascal_String_size, 1);

    if (!strlen(opt_filename))
    {
        printf ("No filename specified." CRLF);
        exit (1);
    }

    if (!player_load_s3m (opt_filename, &song_track))
    {
        printf ("Failed to load file." CRLF);
        exit (1);
    }

    printf ("Loaded '%s' (%s)" CRLF,
        musmod_get_title (song_track),
        musmod_get_format (song_track)
    );

    if (!player_init ())
    {
        printf ("Failed to initialize player." CRLF);
        exit (1);
    }

    if (!player_init_device (2))
    {
        printf ("Failed to initialize sound device." CRLF);
        exit (1);
    }

    player_set_mode (opt_16bits, opt_stereo, opt_rate, opt_lq);
    player_set_order (true);
    player_set_song_loop (true);

    if (!player_set_mode (opt_16bits, opt_stereo, opt_rate, opt_lq))
        exit (1);

    if (!player_play_start ())
        exit (1);

    printf (
        "DMA buffer frame size: %u" CRLF
        "Stop playing and exit with <ESC>" CRLF
        "Press any key to switch to oscillator..." CRLF,
        sndDMABuf.frameSize
    );
    getch ();

    vbios_set_mode (0x13);
    vga_clear_page_320x200x8 (COL_BACKGROUND);
    bufdata = sndDMABuf.buf->data;
    while (!kbhit ())
    {
        vga_wait_vsync ();
        draw_channels_volume ();
        if (opt_stereo)
            update_osci_stereo ();
        else
            update_osci_mono ();
    }
    while (kbhit ()) getch ();
    vbios_set_mode (3);
    player_free ();
}
