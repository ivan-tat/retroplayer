/* smalls3m.c -- small console tracked music player.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "startup.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "cc/malloc.h"
#include "cc/errno.h"
#include "cc/unistd.h"
#include "debug.h"
#include "dos/ems.h"
#include "hw/hwowner.h"
#include "hw/pic.h"
#include "hw/dma.h"
#include "hw/sb/sbctl.h"
#include "hw/vga.h"
#include "main/mixer.h"
#include "main/fillvars.h"
#include "main/musins.h"
#include "main/muspat.h"
#include "main/mixchn.h"
#include "main/effects.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"

#include "player/smalls3m.h"

#define def_rate 45454
#define def_stereo true
#define def_16bits true
#define def_lq false

static uint16_t opt_rate;
static bool opt_stereo;
static bool opt_16bits;
static bool opt_lq;
static char opt_filename[pascal_String_size];

void __near
show_usage (void)
{
    printf (
        CRLF
        "Usage:" CRLF
        "    smalls3m.exe filename.s3m" CRLF
    );
}

void PUBLIC_CODE
smalls3m_main (void)
{
    char *comspec;
    char s[pascal_String_size];
    int result;

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
        printf (
            "Failed to setup DOS environment variables." CRLF
        );
        return;
    }

    comspec = getenv ("COMSPEC");
    if (!comspec)
    {
        printf (
            "Failed to get `%s' environment variable." CRLF,
            "COMSPEC"
        );
        return;
    }

    textbackground (_black);
    textcolor (_lightgray);
    printf (
        "Simple music player for DOS, version %s." CRLF
        "Originally written by Andre Baresel, 1994, 1995." CRLF
        "Modified by Ivan Tatarinov <ivan-tat@ya.ru>, 2016, 2017, 2018." CRLF
        "This is free and unencumbered software released into the public domain." CRLF
        "For more information, please refer to <http://unlicense.org>." CRLF,
        PLAYER_VERSION
    );

    if (custom_argc () != 2)
    {
        show_usage ();
        exit (0);
    }

    custom_argv (s, pascal_String_size, 1);

    if (strlen (s) == 0)
    {
        DEBUG_FAIL ("smalls3m_main", "No filename specified.");
        show_usage ();
        exit (1);
    }

    opt_rate = def_rate;
    opt_stereo = def_stereo;
    opt_16bits = def_16bits;
    opt_lq = def_lq;

    if (!player_load_s3m (s))
        exit (1);

    printf (
        "Music module loaded." CRLF
        "Format: %s" CRLF
        "Title: \"%s\"" CRLF,
        mod_TrackerName,
        mod_Title
    );

    if (!player_init ())
        exit (1);

    if (!player_init_device (2))
        exit (1);

    if (!player_set_mode (opt_16bits, opt_stereo, opt_rate, opt_lq))
        exit (1);

    player_set_order (true);

    playOption_LoopSong = true;

    if (!player_play_start ())
        exit (1);

    printf (CRLF "Type 'EXIT' to return to player and stop playing." CRLF);

    result = execv (comspec, NULL);
    if (result != EZERO)
        printf (
            "DOS error: %u." CRLF
            "Error while running command interpreter." CRLF,
            errno
        );

    player_free ();
}
