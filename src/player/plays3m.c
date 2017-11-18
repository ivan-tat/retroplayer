/* plays3m.c -- full featured tracked music player.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/stdio.h"
#include "cc/malloc.h"
#include "hw/sb/sbctl.h"
#include "dos/ems.h"
#include "main/s3mplay.h"
#include "player/screen.h"

/* program's options */

extern bool PUBLIC_DATA opt_help;

uint32_t PUBLIC_CODE getFreeDOSMemory(void)
{
    return _memmax();
}

uint32_t PUBLIC_CODE getFreeEMMMemory(void)
{
    if (emsInstalled)
        return emsGetFreePagesCount() * 16;
    else
        return 0;
}

void PUBLIC_CODE display_errormsg(void)
{
    printf("PLAYER STATUS - ");

    if (player_is_error())
        printf("Error: %s." CRLF, player_get_error());
    else
        printf("No error." CRLF);
}

void PUBLIC_CODE display_help(void)
{
    printf(
        "USAGE" CRLF
        "  plays3m  [<options>] <filename> [<options>]" CRLF
        CRLF
        "OPTIONS" CRLF
        "Sound card hardware:" CRLF
        "  -env      Use BLASTER environment variable to get sound card configuration." CRLF
        "  -cfg      Input sound card configuration manually." CRLF
        "            Default is to auto-detect." CRLF
        "  -c        Dump sound card configuration after detection." CRLF
        "  -fN       Set frames per second to N Hz for filling output buffer." CRLF
        "            Default is 70." CRLF
        "Output sound format:" CRLF
        "  -sN       Set output sample rate to N in Hz or KHz (4-46 or 4000-45454)." CRLF
        "  -m        Force output channels to 1 (mono mixing)." CRLF
        "            Default is 2 channels (stereo mixing)." CRLF
        "  -8        Force output sample bits to 8." CRLF
        "            Default is 16." CRLF
        "  -lq       Force low quality output mode." CRLF
        "            Default is normal quality." CRLF
        "Sound playback:" CRLF
        "  -vN       Set master volume to N (0-255)." CRLF
        "            Default is 0 (use volume as specified in file)." CRLF
        "  -o        Force to handle patterns order like ST3 does." CRLF
        "            Default is to play all patterns in specified order." CRLF
        "  -bN       Start to play at Nth position in order." CRLF
        "            Default is 0." CRLF
        "System:" CRLF
        "  -noems    Disable EMS usage." CRLF
        "Others:" CRLF
        "  -h, -?    Show this help." CRLF
    );
    if (!opt_help)
        printf(CRLF "Error: no filename specified." CRLF);
}

void PUBLIC_CODE display_playercfg(void)
{
    writelnSBConfig();
}

void PUBLIC_CODE display_keys(void)
{
    printf(
        "Keys while playing:" CRLF
        CRLF
        "  F1 .... this help screen" CRLF
        "  F2 .... display channels information" CRLF
        "  F3 .... display current pattern" CRLF
        "  F4 .... display instruments information" CRLF
        "  F5 .... display samples memory positions" CRLF
        "  F6 .... display debug information" CRLF
        "  P ..... pause playback (only on SB16)" CRLF
        "  L ..... enable/disable song loop." CRLF
        "  <Alt>+(<1>-<0>,<Q>-<R>) ... toggle channel 1-16 on/off." CRLF
        "  + ..... jump to next pattern." CRLF
        "  - ..... jump to previous pattern" CRLF
        "  D ..... DOS shell." CRLF
        "  ESC ... stop playback and exit." CRLF
    );
}
