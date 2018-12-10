/* plays3m.c -- full featured tracked music player.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

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
#include "hw/sb/sbctl.h"
#include "hw/vbios.h"
#include "hw/vga.h"
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

#define upcase(c) (((c >= 'a') && (c <= 'z')) ? c - 'a' + 'A' : c)

/* Command line's options */

#define DEVSEL_AUTO     1
#define DEVSEL_ENV      2
#define DEVSEL_MANUAL   3

static bool     opt_help;
static char     opt_filename[pascal_String_size];
static uint8_t  opt_devselect;
static uint16_t opt_mode_rate;
static bool     opt_mode_stereo;
static bool     opt_mode_16bits;
static bool     opt_mode_lq;
static bool     opt_dumpconf;
static uint8_t  opt_mvolume;
static bool     opt_st3order;
static uint8_t  opt_startpos;
static bool     opt_loop;
static bool     opt_em;
static uint8_t  opt_fps;

uint32_t getFreeDOSMemory(void)
{
    return _memmax();
}

uint32_t getFreeEMMMemory(void)
{
    if (emsInstalled)
        return emsGetFreePagesCount() * 16;
    else
        return 0;
}

void __near memstats(void)
{
    printf("Free DOS memory:........%6lu KiB" CRLF, getFreeDOSMemory() >> 10);
    clreol();
    printf("Free expanded memory:...%6lu KiB" CRLF, getFreeEMMMemory());
    clreol();
}

void __near display_errormsg(void)
{
    printf("PLAYER STATUS - ");

    if (player_is_error())
        printf("Error: %s." CRLF, player_get_error());
    else
        printf("No error." CRLF);
}

void __near display_help(void)
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
        "  -sN       Set output sample rate to N in Hz or KHz (4-48 or 4000-48000)." CRLF
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

void __near display_playercfg(void)
{
    player_device_dump_conf();
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

uint8_t nextord(uint8_t nr)
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

uint8_t prevorder(uint8_t nr)
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

void get_note_name(char *__dest, uint8_t note)
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

void write_Note(uint8_t note)
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

/* Information windows */

/* Pattern window */

// FIXME: make these private:
extern uint8_t PUBLIC_DATA lastrow;
extern uint8_t PUBLIC_DATA startchn;

/* Window's event router */

#define WINLIST_SIZE 7

static SCRWIN *winlist_list[WINLIST_SIZE];
static uint8_t winlist_selected;    /* current info on screen */

bool __near winlist_init(void)
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

void __near winlist_select(uint16_t value)
{
    winlist_selected = value;
}

void __near winlist_show_selected(void)
{
    SCRWIN *win;

    win = winlist_list[winlist_selected];

    if (!(scrwin_get_flags(win) & WINFL_VISIBLE))
        scrwin_show(win);
}

void __near winlist_hide_selected(void)
{
    SCRWIN *win;

    win = winlist_list[winlist_selected];

    if (scrwin_get_flags(win) & WINFL_VISIBLE)
        scrwin_close(win);
}

void __near winlist_show_all(void)
{
    scrwin_show(win_information);
    winlist_show_selected();
}

void __near winlist_hide_all(void)
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

void __near winlist_refresh_all(void)
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

bool __near winlist_keypress(char c)
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

/* channels */

static uint8_t savchn[MAX_CHANNELS];

void __near channels_save_all(void)
{
    int i;
    MIXCHN *chn;

    for (i = 0; i < MAX_CHANNELS; i++)
    {
        chn = &(mod_Channels[i]);
        savchn[i] = mixchn_get_type(chn);
    }
}

void __near channels_swap(uint8_t index)
{
    MIXCHN *chn;

    chn = &(mod_Channels[index]);

    if (mixchn_get_type(chn) == 0)
        mixchn_set_type(chn, savchn[index]);
    else
        mixchn_set_type(chn, 0);
}

void __near channels_toggle_mixing(uint8_t index)
{
    MIXCHN *chn;

    chn = &(mod_Channels[index]);
    mixchn_set_mixing(chn, !mixchn_is_mixing(chn));
}

void __near channels_stop_all(void)
{
    int i;
    MIXCHN *chn;

    for (i = 0; i < mod_ChannelsCount; i++)
    {
        chn = &(mod_Channels[i]);
        mixchn_set_flags(chn, mixchn_get_flags(chn) & ~MIXCHNFL_PLAYING);
    }
}

void __near cursor_hide(void)
{
    vbios_set_cursor_shape(32, 32);
}

void __near cursor_show(void)
{
    vbios_set_cursor_shape(15, 16);
}

void __near desktop_clear(void)
{
    window(1, 1, scrWidth, scrHeight);
    textbackground(_black);
    clrscr();
}

/*** Parsing command line's options ***/

bool __near _opt_check_value_mvolume(long v)
{
    return (v >= 0) && (v <= 255);
}

bool __near _opt_check_value_mode_rate(long v)
{
    return (v >= 4) && (v <= 48) || (v >= 4000) && (v <= 48000);
}

bool __near _opt_check_value_startpos(long v)
{
    return (v >= 0) && (v <= 255);
}

bool __near _opt_check_value_fps(long v)
{
    return (v >= 2) && (v <= 200);
}

void __near _opt_bad_value(char *param)
{
    printf("Bad value in parameter \"%s\".", param);
}

bool __near _opt_parse(char *s)
{
    char *param, *endptr;
    uint8_t b;
    uint16_t w;
    long v;
    int i;

    if (s[0] != '-')
    {
        DEBUG_INFO_("_opt_parse", "opt_filename=\"%s\"", s);
        strncpy(opt_filename, s, pascal_String_size);
        return true;
    }
    param = s + 1;

    if (!strcmp(param, "h"))
    {
        DEBUG_INFO_("_opt_parse", "opt_help=%hu", 1);
        opt_help = true;
        return true;
    }

    if (!strcmp(param, "env"))
    {
        DEBUG_INFO_("_opt_parse", "opt_devselect=%hu", DEVSEL_ENV);
        opt_devselect = DEVSEL_ENV;
        return true;
    }

    if (!strcmp(param, "cfg"))
    {
        DEBUG_INFO_("_opt_parse", "opt_devselect=%hu", DEVSEL_MANUAL);
        opt_devselect = DEVSEL_MANUAL;
        return true;
    }

    if (!strcmp(param, "c"))
    {
        DEBUG_INFO_("_opt_parse", "opt_dumpconf=%hu", 1);
        opt_dumpconf = true;
        return true;
    }

    if (*param == 'f')
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _opt_check_value_fps(v))
        {
            DEBUG_INFO_("_opt_parse", "opt_fps=%hu", (uint8_t)v);
            opt_fps = v;
            return true;
        }
        else
        {
            _opt_bad_value(s);
            return false;
        }
    }

    if (*param == 's')
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _opt_check_value_mode_rate(v))
        {
            if (v < 100)
                v *= 1000;
            DEBUG_INFO_("_opt_parse", "opt_mode_rate=%u", (uint16_t)v);
            opt_mode_rate = v;
            return true;
        }
        else
        {
            _opt_bad_value(s);
            return false;
        }
    }

    if (!strcmp(param, "m"))
    {
        DEBUG_INFO_("_opt_parse", "opt_mode_stereo=%hu", 0);
        opt_mode_stereo = false;
        return true;
    }

    if (!strcmp(param, "8"))
    {
        DEBUG_INFO_("_opt_parse", "opt_mode_16bits=%hu", 0);
        opt_mode_16bits = false;
        return true;
    }

    if (!strcmp(param, "lq"))
    {
        DEBUG_INFO_("_opt_parse", "opt_mode_lq=%hu", 1);
        opt_mode_lq = true;
        return true;
    }

    if (*param == 'v')
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _opt_check_value_mvolume(v))
        {
            DEBUG_INFO_("_opt_parse", "opt_mvolume=%hu", (uint8_t)v);
            opt_mvolume = v;
            return true;
        }
        else
        {
            _opt_bad_value(s);
            return false;
        }
    }

    if (!strcmp(param, "o"))
    {
        DEBUG_INFO_("_opt_parse", "opt_st3order=%hu", 1);
        opt_st3order = true;
        return true;
    }

    if (*param == 'b')
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _opt_check_value_startpos(v))
        {
            DEBUG_INFO_("_opt_parse", "opt_startpos=%hu", (uint8_t)v);
            opt_startpos = v;
            return true;
        }
        else
        {
            _opt_bad_value(s);
            return false;
        }
    }

    if (!strcmp(param, "noems"))
    {
        DEBUG_INFO_("_opt_parse", "opt_em=%hu", 0);
        opt_em = false;
        return true;
    }

    printf("Unknown option \"%s\".", param);
    return false;
}

/*** Shell ***/

void run_os_shell(void)
{
    char *comspec;
    bool result;
    char c;

    vbios_set_mode(3);  // clear screen
    textbackground(_black);
    textcolor(_lightgray);
    printf("Starting DOS shell... (to return to player use 'exit' command)" CRLF);
    comspec = getenv("COMSPEC");
    result = execv(comspec, NULL);
    c = 0;
    vbios_set_mode(3);  // restore text-mode
    cursor_hide();
    if (result)
    {
        while (kbhit())
            getch();
        printf(
            "DOS error: %u." CRLF
            "Error while running command interpreter." CRLF
            "Press any key to continue...",
            errno
        );
        getch();
    }
}

/*** Main **/

void PUBLIC_CODE plays3m_main(void)
{
    int count, i;
    char s[pascal_String_size];
    bool quit, result;
    char c;

    /* TODO: make separate custom_main() */

    /* TODO: all modules initialization goes here */
    // This is the place where all modules must be manually initialized in the right order.
    // Actually this is automatically done by Pascal linker at the moment and
    // must be fixed in the future.
    /*
    register_debug();
    register_hwowner();
    register_pic();
    register_dma();
    register_ems();
    register_vga();
    register_sbctl();
    register_s3mplay();
    register_plays3m();
    */

    console_init ();

    if (!environ_init())
    {
        printf("Failed to setup DOS environment variables." CRLF);
        exit(1);
    }

    printf(
        "Simple music player for DOS, version %s." CRLF
        "Originally written by Andre Baresel, 1994-1995." CRLF
        "Modified by Ivan Tatarinov <ivan-tat@ya.ru>, 2016-2018." CRLF
        "This is free and unencumbered software released into the public domain." CRLF
        "For more information, please refer to <http://unlicense.org>." CRLF,
        PLAYER_VERSION
    );

    opt_help = false;
    opt_filename[0] = 0;
    opt_devselect = DEVSEL_AUTO;
    opt_mode_rate = 48000;
    opt_mode_stereo = true;
    opt_mode_16bits = true;
    opt_mode_lq = false;
    opt_dumpconf = false;
    opt_mvolume = 0;            // use volume from file
    opt_st3order = false;
    opt_startpos = 0;
    opt_loop = false;
    opt_em = true;
    opt_fps = playOption_FPS;

    count = custom_argc();
    for (i = 1; i < count; i++)
    {
        custom_argv(s, pascal_String_size, i);
        printf("option=\"%s\"." CRLF, s);
        if (!_opt_parse(s))
            exit(1);
    }

    if (!strlen(opt_filename))
    {
        display_help();
        exit(1);
    }

    UseEMS = opt_em;

    if (DEBUG_FILE_S3M_LOAD)
    {
        printf ("Before loading:" CRLF);
        memstats ();
    }

    if (!player_load_s3m(opt_filename))
    {
        display_errormsg();
        exit(1);
    }

    if (DEBUG_FILE_S3M_LOAD)
    {
        printf ("After loading:" CRLF);
        memstats ();
    }

    printf("Song \"%s\" loaded (%s)." CRLF, mod_Title, mod_TrackerName);

    if (!player_init())
    {
        display_errormsg();
        exit(1);
    }

    if (!player_init_device(opt_devselect))
    {
        printf("No sound device found." CRLF);
        exit(1);
    }

    if (opt_dumpconf)
    {
        display_playercfg();
        printf("Press a key to continue...");
        getch();
        printf(CRLF);
    }

    if (opt_mvolume)
        player_set_master_volume(opt_mvolume);

    if (!player_set_mode(opt_mode_16bits, opt_mode_stereo, opt_mode_rate, opt_mode_lq))
    {
        display_errormsg();
        exit(1);
    }

    player_set_order(opt_st3order);
    initState_startOrder = opt_startpos;
    playOption_LoopSong = opt_loop;
    playOption_FPS = opt_fps;

    channels_save_all();

    if (!winlist_init())
    {
        DEBUG_FAIL("plays3m_main", "Failed to initialize information windows.");
        exit(1);
    }

    if (!player_play_start())
    {
        display_errormsg();
        exit(1);
    }

    cursor_hide();
    desktop_clear();
    winlist_select(0);
    winlist_show_all();

    quit = false;
    do
    {
        winlist_refresh_all();

        if (kbhit())
        {
            c = getch();

            if (!winlist_keypress(c))
            {
                if (c == 27)
                {
                    quit = true;
                    c = 0;
                }

                if ((c >= 'x') && (c <= 'x' + 16))
                {
                    channels_toggle_mixing(c - 'x');
                    c = 0;
                }
                if ((c >= 16) && (c <= 19))
                {
                    channels_toggle_mixing(c - 4);
                    c = 0;
                }
                /* F1-F6 */
                if ((c >= 59) && (c <= 64))
                {
                    winlist_hide_selected();
                    winlist_select(c - 59);
                    winlist_show_selected();
                    c = 0;
                }
                if (upcase(c) == 'P')
                {
                    player_play_pause();
                    getch();
                    player_play_continue();
                    c = 0;
                }
                if (c == '+')
                {
                    lastrow = 0;
                    player_set_pos(nextord(playState_order), 0, true);
                    c = 0;
                }
                if (c == '-')
                {
                    lastrow = 0;
                    player_set_pos(prevorder(playState_order), 0, false);
                    channels_stop_all();
                    c = 0;
                }
                if (upcase(c) == 'L')
                {
                    playOption_LoopSong = !playOption_LoopSong;
                    c = 0;
                }
                if (upcase(c) == 'D')
                {
                    winlist_hide_all();
                    run_os_shell();
                    desktop_clear();
                    winlist_show_all();
                }
            }
        }
    }
    while (((sndDMABuf.flags & SNDDMABUFFL_SLOW) == 0) && (!quit) && (!playState_songEnded));

    textbackground(_black);
    textcolor(_lightgray);
    clrscr();
    cursor_show();

    if (DEBUG)
        if (sndDMABuf.flags & SNDDMABUFFL_SLOW)
            DEBUG_FAIL ("plays3m_main", "PC is too slow");

    player_free();

    if (DEBUG)
    {
        printf ("After all:" CRLF);
        memstats ();
    }
}

/*** Initialization ***/

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
