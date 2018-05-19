/* plays3m.c -- full featured tracked music player.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"

#include "playosci.h"

void PUBLIC_CODE playosci_init (void)
{
    if (!player_load_s3m (opt_filename))
    {
        printf ("Failed to load file." CRLF);
        exit (1);
    }

    printf ("Loaded '%s' (%s)" CRLF, mod_Title, mod_TrackerName);

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
    playOption_LoopSong = true;
}
