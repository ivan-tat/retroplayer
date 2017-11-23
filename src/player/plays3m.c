/* plays3m.c -- full featured tracked music player.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "cc/malloc.h"
#include "hw/sb/sbctl.h"
#include "dos/ems.h"
#include "main/mixer.h"
#include "main/fillvars.h"
#include "main/musins.h"
#include "main/muspat.h"
#include "main/mixchn.h"
#include "main/effects.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"
#include "player/screen.h"
#include "player/w_inf.h"
#include "player/w_hlp.h"
#include "player/w_chn.h"
#include "player/w_pat.h"
#include "player/w_ins.h"
#include "player/w_smp.h"
#include "player/w_dbg.h"

#include "player/plays3m.h"

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

uint8_t __near order_find_previous_entry(uint8_t nr)
{
    while (nr && (Order[nr] >= 254))
        nr--;

    return nr;
}

uint8_t __near order_find_next_entry(uint8_t nr)
{
    while ((nr <= LastOrder) && (Order[nr] >= 254))
        nr++;

    return nr;
}

uint8_t __near order_go_to_next_entry(uint8_t nr)
{
    nr = order_find_next_entry(nr);

    if (nr > LastOrder)
        playState_songEnded = true; // bad order - no real entry

    return nr;
}

uint8_t __near __pascal nextord(uint8_t nr)
{
    playState_patDelayCount = 0;
    playState_patLoopActive = false;
    playState_patLoopCount = 0;
    playState_patLoopStartRow = 0;

    nr = order_find_next_entry(nr + 1);

    if (nr > LastOrder)
    {
        if (playOption_LoopSong)
            nr = order_go_to_next_entry(0);
        else
            playState_songEnded = true;
    }

    return nr;
}

uint8_t __near __pascal prevorder(uint8_t nr)
{
    if (!nr)
        return nr;

    nr = order_find_previous_entry(nr - 1);

    if (Order[nr] >= 254)
        nr = order_go_to_next_entry(nr);

    return nr;
}

/* Information windows */

static const char _halftone_names[12][2] =
{
    "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};

void __near get_note_name(char *__dest, uint8_t note)
{
    uint8_t halftone, octave;
    bool valid;

    valid = true;
    if (note == 255)
    {
        __dest[0] = '.';
        __dest[1] = '.';
        __dest[2] = '.';
        __dest[3] = 0;
    }
    else
    if (note == 254)
    {
        __dest[0] = '^';
        __dest[1] = '^';
        __dest[2] = '^';
        __dest[3] = 0;
    }
    else
    if (note <= (9 << 4) + 11)
    {
        halftone = note & 0x0f;
        octave = note >> 4;
        if ((halftone <= 11) && (octave <= 9))
        {
            __dest[0] = _halftone_names[halftone][0];
            __dest[1] = _halftone_names[halftone][1];
            __dest[2] = '0' + octave;
            __dest[3] = 0;
        }
        else
            valid = false;
    }
    else
        valid = false;

    if (!valid)
    {
        __dest[0] = '?';
        __dest[1] = '?';
        __dest[2] = '?';
        __dest[3] = 0;
    }
}

void __near __pascal write_Note(uint8_t note)
{
    char s[4];

    get_note_name(s, note);
    printf(s);
}

static SCRWIN win_information_static;
static SCRWIN win_help_static;
static SCRWIN win_channels_static;
static SCRWIN win_pattern_static;
static SCRWIN win_instruments_static;
static SCRWIN win_samples_static;
static SCRWIN win_debug_static;

static SCRWIN *win_information = NULL;
static SCRWIN *win_help = NULL;
static SCRWIN *win_channels = NULL;
static SCRWIN *win_pattern = NULL;
static SCRWIN *win_instruments = NULL;
static SCRWIN *win_samples = NULL;
static SCRWIN *win_debug = NULL;

/* Window's event router */

#define WINLIST_SIZE 7

static SCRWIN *winlist_list[WINLIST_SIZE];
static uint8_t winlist_selected;    /* current info on screen */

bool __near __pascal winlist_init(void)
{
    DEBUG_BEGIN("winlist_init");

    win_information = &win_information_static;
    win_information_init(win_information);

    win_help = &win_help_static;
    win_help_init(win_help);

    win_channels = &win_channels_static;
    win_channels_init(win_channels);

    win_pattern = &win_pattern_static;
    win_pattern_init(win_pattern);

    win_instruments = &win_instruments_static;
    win_instruments_init(win_instruments);

    win_samples = &win_samples_static;
    win_samples_init(win_samples);

    win_debug = &win_debug_static;
    win_debug_init(win_debug);

    winlist_list[0] = win_help;
    winlist_list[1] = win_channels;
    winlist_list[2] = win_pattern;
    winlist_list[3] = win_instruments;
    winlist_list[4] = win_samples;
    winlist_list[5] = win_debug;
    winlist_list[6] = win_information;

    DEBUG_SUCCESS("winlist_init");
    return true;
}

void __near __pascal winlist_select(uint16_t value)
{
    winlist_selected = value;
}

void __near __pascal winlist_show_selected(void)
{
    SCRWIN *win;

    win = winlist_list[winlist_selected];

    if (!(scrwin_get_flags(win) & WINFL_VISIBLE))
        scrwin_show(win);
}

void __near __pascal winlist_hide_selected(void)
{
    SCRWIN *win;

    win = winlist_list[winlist_selected];

    if (scrwin_get_flags(win) & WINFL_VISIBLE)
        scrwin_close(win);
}

void __near __pascal winlist_show_all(void)
{
    scrwin_show(win_information);
    winlist_show_selected();
}

void __near __pascal winlist_hide_all(void)
{
    int i;
    SCRWIN *win;

    for (i = 0; i < WINLIST_SIZE; i++)
    {
        win = winlist_list[i];
        if (scrwin_get_flags(win) & WINFL_VISIBLE)
            scrwin_close(win);
    }
}

void __near __pascal winlist_refresh_all(void)
{
    int i;
    SCRWIN *win;

    for (i = 0; i < WINLIST_SIZE; i++)
    {
        win = winlist_list[i];
        if (scrwin_get_flags(win) & WINFL_VISIBLE)
            scrwin_draw(win);
    }
}

bool __near __pascal winlist_keypress(char c)
{
    int i;
    SCRWIN *win;

    for (i = 0; i < WINLIST_SIZE; i++)
    {
        win = winlist_list[i];
        if (scrwin_get_flags(win) & WINFL_VISIBLE)
            if (scrwin_keypress(win, c))
                return true;
    }

    return false;
}

void __near __pascal winlist_free(void)
{
    DEBUG_BEGIN("winlist_free");
    DEBUG_END("winlist_free");
}

extern void __near __pascal winlist_free(void);

/*** Initialization ***/

DECLARE_REGISTRATION(plays3m)

void __near plays3m_init(void)
{
    DEBUG_BEGIN("plays3m_init");
    DEBUG_END("plays3m_init");
}

void __near plays3m_done(void)
{
    DEBUG_BEGIN("plays3m_done");
    winlist_free();
    DEBUG_END("plays3m_done");
}

DEFINE_REGISTRATION(plays3m, plays3m_init, plays3m_done)
