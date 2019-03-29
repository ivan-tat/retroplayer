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
#include "dos/ems.h"
#include "common.h"
#include "hw/hwowner.h"
#include "hw/pic.h"
#include "hw/dma.h"
#include "hw/sb/sbctl.h"
#include "hw/vbios.h"
#include "hw/vga.h"
#include "main/mixer.h"
#include "main/fillvars.h"
#include "main/musins.h"
#include "main/muspat.h"
#include "main/musmod.h"
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

static MUSMOD  *song_track;

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

uint8_t __near order_find_previous_entry (MUSMOD *track, uint8_t i)
{
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;
    bool found;

    order = musmod_get_order (track);
    found = false;
    while ((!found) && (i > 0))
    {
        order_entry = muspatorder_get (order, i);
        if ((*order_entry != MUSPATORDENT_SKIP)
        &&  (*order_entry != MUSPATORDENT_END))
            found = true;
        else
            i--;
    }

    return i;
}

uint8_t __near order_find_next_entry (MUSMOD *track, uint8_t i)
{
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;
    bool found;

    order = musmod_get_order (track);
    found = false;
    while ((!found) && (i <= LastOrder))
    {
        order_entry = muspatorder_get (order, i);
        if ((*order_entry != MUSPATORDENT_SKIP)
        &&  (*order_entry != MUSPATORDENT_END))
            found = true;
        else
            i++;
    }

    return i;
}

uint8_t __near order_go_to_next_entry (MUSMOD *track, PLAYSTATE *ps, uint8_t i)
{
    i = order_find_next_entry (track, i);

    if (i > LastOrder)
        ps->flags |= PLAYSTATEFL_END; // bad order - no real entry

    return i;
}

uint8_t nextord (MUSMOD *track, PLAYSTATE *ps, uint8_t i)
{
    ps->flags &= ~PLAYSTATEFL_PATLOOP;
    ps->patdelay_count = 0;
    ps->patloop_count = 0;
    ps->patloop_start_row = 0;

    i = order_find_next_entry (track, i + 1);

    if (i > LastOrder)
    {
        if (ps->flags & PLAYSTATEFL_SONGLOOP)
            i = order_go_to_next_entry (track, ps, 0);
        else
            ps->flags |= PLAYSTATEFL_END;
    }

    return i;
}

uint8_t prevorder (MUSMOD *track, PLAYSTATE *ps, uint8_t i)
{
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;

    if (!i)
        return i;

    i = order_find_previous_entry (track, i - 1);

    order = musmod_get_order (track);
    order_entry = muspatorder_get (order, i);
    if ((*order_entry == MUSPATORDENT_SKIP)
    ||  (*order_entry == MUSPATORDENT_END))
        i = order_go_to_next_entry (track, ps, i);

    return i;
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
    if (note == CHN_NOTE_NONE)
    {
        __dest[0] = '.';
        __dest[1] = '.';
        __dest[2] = '.';
        __dest[3] = 0;
    }
    else
    if (note == CHN_NOTE_OFF)
    {
        __dest[0] = '^';
        __dest[1] = '^';
        __dest[2] = '^';
        __dest[3] = 0;
    }
    else
    if (note <= CHN_NOTE_MAX)
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

static SCRWIN *win_information = NULL;
static SCRWIN *win_help = NULL;
static SCRWIN *win_channels = NULL;
static SCRWIN *win_pattern = NULL;
static SCRWIN *win_instruments = NULL;
static SCRWIN *win_samples = NULL;
static SCRWIN *win_debug = NULL;

SCRWIN *__near __window_create (bool (*__far init) (SCRWIN *self))
{
    SCRWIN *w;

    w = _new (SCRWIN);
    if (!w)
    {
        DEBUG_ERR_ ("__window_create", "Failed to allocate memory for %s.", "information window");
        return false;
    }
    if (!init (w))
    {
        DEBUG_ERR_ ("__window_create", "Failed to initialize %s.", "information window");
        return NULL;
    }
    return w;
}

/* Window's event router */

#define WINLIST_SIZE 7

static SCRWIN *winlist_list[WINLIST_SIZE];
static uint8_t winlist_count;
static uint8_t winlist_selected;    /* current info on screen */

void __near winlist_add_item (SCRWIN *win)
{
    winlist_list[winlist_count] = win;
    winlist_count++;
}

bool __near winlist_init (void)
{
    DEBUG_BEGIN ("winlist_init");

    memset (winlist_list, 0, sizeof (winlist_list));
    winlist_count = 0;

    /* Help window */
    win_help = __window_create (&win_help_init);
    if (!win_help)
        return false;
    winlist_add_item (win_help);

    /* Channels window */
    win_channels = __window_create (&win_channels_init);
    if (!win_channels)
        return false;
    winlist_add_item (win_channels);

    /* Pattern window */
    win_pattern = __window_create (&win_pattern_init);
    if (!win_pattern)
        return false;
    winlist_add_item (win_pattern);

    /* Instruments window */
    win_instruments = __window_create (&win_instruments_init);
    if (!win_instruments)
        return false;
    winlist_add_item (win_instruments);

    /* Samples window */
    win_samples = __window_create (&win_samples_init);
    if (!win_samples)
        return false;
    winlist_add_item (win_samples);

    /* Debug window */
    win_debug = __window_create (&win_debug_init);
    if (!win_debug)
        return false;
    winlist_add_item (win_debug);

    /* Information window - must be last in list */
    win_information = __window_create (&win_information_init);
    if (!win_information)
        return false;
    winlist_add_item (win_information);

    DEBUG_SUCCESS ("winlist_init");
    return true;
}

void __near winlist_on_resize (void)
{
    SCRWIN *win;
    SCRRECT r;
    int count, i;

    win = win_information;
    r.x0 = 1;
    r.y0 = 1;
    r.x1 = scrWidth;
    r.y1 = 5;
    scrwin_set_rect (win, &r);
    scrwin_on_resize (win);

    r.x0 = 1;
    r.y0 = 6;
    r.x1 = scrWidth;
    r.y1 = scrHeight;

    count = winlist_count - 1;
    for (i = 0; i < count; i++)
    {
        win = winlist_list[i];
        scrwin_set_rect (win, &r);
        scrwin_on_resize (win);
    }
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
    scrwin_show (win_information);
    winlist_show_selected();
}

void __near winlist_hide_all(void)
{
    int i;
    SCRWIN *win;

    for (i = 0; i < winlist_count; i++)
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

    for (i = 0; i < winlist_count; i++)
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

    for (i = 0; i < winlist_count; i++)
    {
        win = winlist_list[i];
        if (scrwin_get_flags(win) & WINFL_VISIBLE)
            if (scrwin_keypress(win, c))
                return true;
    }

    return false;
}

void __near winlist_free (void)
{
    int i;
    SCRWIN *win;

    DEBUG_BEGIN("winlist_free");
    for (i = 0; i < winlist_count; i++)
    {
        win = winlist_list[i];
        if (win)
        {
            scrwin_free (win);
            _delete (winlist_list[i]);
        }
    }
    DEBUG_END("winlist_free");
}

/* channels */

static bool savchn[MUSMOD_CHANNELS_MAX];

void __near channels_save_all(void)
{
    MIXCHNLIST *channels;
    MIXCHN *chn;
    uint8_t num_channels, i;

    channels = mod_Channels;
    num_channels = mixchnl_get_count (channels);

    for (i = 0; i < num_channels; i++)
    {
        chn = mixchnl_get (channels, i);
        savchn[i] = mixchn_is_mixing (chn);
    }
}

void __near channels_toggle_mixing(uint8_t index)
{
    MIXCHNLIST *channels;
    MIXCHN *chn;

    channels = mod_Channels;
    chn = mixchnl_get (channels, index);
    mixchn_set_mixing (chn, !mixchn_is_mixing(chn));
}

void __near channels_stop_all(void)
{
    MIXCHNLIST *channels;
    MIXCHN *chn;
    uint8_t num_channels, i;

    channels = mod_Channels;
    num_channels = mixchnl_get_count (channels);

    for (i = 0; i < num_channels; i++)
    {
        chn = mixchnl_get (channels, i);
        mixchn_set_flags (chn, mixchn_get_flags(chn) & ~MIXCHNFL_PLAYING);
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

void __far plays3m_main (void)
{
    PLAYSTATE *ps;
    MUSMOD *track;
    MIXCHNLIST *channels;
    int count, i;
    char s[pascal_String_size];
    bool quit, result;
    char c;

    /* TODO: make separate custom_main() */

    register_debug ();
    register_hwowner ();
    register_pic ();
    register_dma ();
    register_ems ();
    register_vga ();
    register_sbctl ();
    register_s3mplay ();
    register_plays3m ();

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

    if (!player_load_s3m (opt_filename, &song_track))
    {
        display_errormsg();
        exit(1);
    }

    if (DEBUG_FILE_S3M_LOAD)
    {
        printf ("After loading:" CRLF);
        memstats ();
    }

    track = song_track;
    ps = &playState;

    printf ("Song \"%s\" loaded (%s)." CRLF,
        musmod_get_title (track),
        musmod_get_format (track));

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
    player_set_song_loop (opt_loop);
    playOption_FPS = opt_fps;

    channels_save_all();

    if (!winlist_init())
    {
        DEBUG_FAIL("plays3m_main", "Failed to initialize information windows.");
        exit(1);
    }

    winlist_on_resize ();

    win_information_set_track (win_information, track);
    win_information_set_play_state (win_information, ps);
    win_pattern_set_track (win_pattern, track);
    win_pattern_set_play_state (win_pattern, ps);
    win_pattern_set_start_channel (win_pattern, 1);
    win_instruments_set_track (win_instruments, track);
    win_instruments_set_page_start (win_instruments, 0);
    win_samples_set_track (win_samples, track);
    win_samples_set_page_start (win_samples, 0);
    win_debug_set_track (win_debug, track);
    win_debug_set_play_state (win_debug, ps);

    if (!player_play_start())
    {
        display_errormsg();
        exit(1);
    }

    channels = mod_Channels;

    /* FIXME: move these here for now (MIXCHNLIST is allocated in player_play_start() ) */
    win_channels_set_channels (win_channels, channels);
    win_pattern_set_channels (win_pattern, channels);
    win_instruments_set_channels (win_instruments, channels);
    win_samples_set_channels (win_samples, channels);

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
                    player_set_pos (nextord (track, ps, ps->order), 0, true);
                    c = 0;
                }
                if (c == '-')
                {
                    player_set_pos (prevorder (track, ps, ps->order), 0, false);
                    channels_stop_all();
                    c = 0;
                }
                if (upcase(c) == 'L')
                {
                    ps->flags ^= PLAYSTATEFL_SONGLOOP;
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
    while (((sndDMABuf.flags & SNDDMABUFFL_SLOW) == 0) && (!quit) && (!(ps->flags & PLAYSTATEFL_END)));

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

DEFINE_REGISTRATION (plays3m, plays3m_init, plays3m_done)
