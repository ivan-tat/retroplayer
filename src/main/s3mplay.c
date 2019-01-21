/* s3mplay.c -- main library for playing music modules.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "pascal.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "common.h"
#include "hw/cpu.h"
#include "hw/sb/sbctl.h"
#include "main/mixer.h"
#include "main/fillvars.h"
#include "main/filldma.h"
#include "main/voltab.h"
#include "main/posttab.h"
#include "main/effects.h"
#include "main/musins.h"
#include "main/muspat.h"
#include "main/s3mvars.h"
#include "main/loads3m.h"

#include "main/s3mplay.h"

#define _PLAYER_ERROR_LEN 128

static char     player_error[_PLAYER_ERROR_LEN];
static bool     player_flags_i386;
static bool     player_flags_bufalloc;
static bool     player_flags_snddev;
static SBDEV   *player_device;
static bool     player_mode_set;
static uint8_t  player_mode_bits;
static bool     player_mode_signed;
static uint8_t  player_mode_channels;
static uint16_t player_mode_rate;
static bool     player_mode_lq;
static bool     inside;

static EMSNAME _EM_map_name = "saveMAP";

/* IRQ routines */

void __far ISR_play(void)
{
    bool err;

    while (inside)
    {
        __asm "nop";
    }

    inside = true;
    sndDMABuf.frameActive = (sndDMABuf.frameActive + 1) & (sndDMABuf.framesCount - 1);
    inside = false;

    err = false;

    if (UseEMS)
    {
        err = true;
        if (emsSaveMap(SavHandle))
            err = false;
    }

    fill_DMAbuffer(&mixBuf, &sndDMABuf);

    if (UseEMS & !err)
        emsRestoreMap(SavHandle);
}

/*** Player ***/

void __far player_clear_error (void)
{
    memset (player_error, 0, _PLAYER_ERROR_LEN);
}

bool __far player_is_error (void)
{
    return player_error[0] != 0;
}

const char *__far player_get_error (void)
{
    if (player_error[0] != 0)
        return player_error;
    else
        return NULL;
}

#if DEBUG == 1

void __near player_set_error (const char *method, int line, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vsnprintf (player_error, _PLAYER_ERROR_LEN, format, ap);
    _DEBUG_LOG (DBGLOG_ERR, __FILE__, line, method, "%s", player_error);
}

#define ERROR(m, f, ...) player_set_error (m, __LINE__, f, __VA_ARGS__)

#else

void __near player_set_error (const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vsnprintf (player_error, _PLAYER_ERROR_LEN, format, ap);
}

#define ERROR(m, f, ...) player_set_error (f, __VA_ARGS__)

#endif  /* DEBUG */

/*** Initialization ***/

bool __far player_init (void)
{
    uint16_t len;

    DEBUG_BEGIN("player_init");

    player_clear_error();

    if (!player_flags_i386)
    {
        ERROR ("player_init", "%s", "CPU is not supported.");
        return false;
    }

    if (UseEMS)
    {
        SavHandle = emsAlloc(1);    // is 1 page enough?
        if (emsEC != E_EMS_SUCCESS)
        {
            ERROR ("player_init", "%s", "Failed to allocate EM handle for mapping.");
            return false;
        }
        emsSetHandleName(SavHandle, &_EM_map_name);
    }

    if (!volumetableptr)
        if (!voltab_alloc())
        {
            ERROR ("player_init", "Failed to initialize %s.", "volume table");
            return false;
        }

    if (!sndDMABuf.buf)
        if (!snddmabuf_alloc(&sndDMABuf, DMA_BUF_SIZE_MAX))
        {
            ERROR ("player_init", "Failed to initialize %s.", "sound buffer");
            return false;
        }

    len = sndDMABuf.buf->size / 2;  // half DMA transfer buffer size (but in samples)

    if (!smpbuf_get(&smpbuf))
        if (!smpbuf_alloc(&smpbuf, len))
        {
            ERROR ("player_init", "Failed to initialize %s.", "sample buffer");
            return false;
        }

    if (!mixbuf_get(&mixBuf))
        if (!mixbuf_alloc(&mixBuf, len))
        {
            ERROR ("player_init", "Failed to initialize %s.", "mixing buffer");
            return false;
        }

    player_flags_bufalloc = true;

    DEBUG_SUCCESS("player_init");
    return true;
}

bool __far player_init_device (uint8_t type)
{
    DEBUG_BEGIN("player_init_device");

    if (type <= 3)
    {
        player_device = sb_new();
        if (!player_device)
        {
            ERROR ("player_init_device", "%s", "Failed to create sound device object.");
            return false;
        }
        sb_init(player_device);
    }
    else
    {
        ERROR ("player_init_device", "%s", "Unknown method.");
        return false;
    }

    switch (type)
    {
    case 0:
        player_flags_snddev = true;
        break;
    case 1:
        player_flags_snddev = sb_conf_detect(player_device);
        break;
    case 2:
        player_flags_snddev = sb_conf_env(player_device);
        break;
    case 3:
        player_flags_snddev = sb_conf_input(player_device);
        break;
    default:
        break;
    }

    if (player_flags_snddev)
    {
        DEBUG_SUCCESS("player_init_device");
        return true;
    }
    else
    {
        ERROR ("player_init_device", "%s", "No sound device.");
        return false;
    }
}

void __far player_device_dump_conf (void)
{
    if (player_flags_snddev)
        sb_conf_dump(player_device);
}

char *__far player_device_get_name (void)
{
    if (player_flags_snddev)
        return sb_get_name(player_device);
    else
        return NULL;
}

bool __far player_set_mode (bool f_16bits, bool f_stereo, uint16_t rate, bool LQ)
{
    DEBUG_BEGIN("player_set_mode");

    if (f_16bits)
    {
        player_mode_bits = 16;
        player_mode_signed = true;
    }
    else
    {
        player_mode_bits = 8;
        player_mode_signed = false;
    }

    if (f_stereo)
        player_mode_channels = 2;
    else
        player_mode_channels = 1;

    player_mode_rate = rate;
    player_mode_lq = LQ;
    player_mode_set = true;

    DEBUG_SUCCESS("player_set_mode");
    return true;
}

uint16_t __far player_get_output_rate (void)
{
    return player_mode_rate;
}

uint8_t __far player_get_output_channels (void)
{
    return player_mode_channels;
}

uint8_t __far player_get_output_bits (void)
{
    return player_mode_bits;
}

bool __far player_get_output_lq (void)
{
    return player_mode_lq;
}

bool __near _player_setup_outbuf(SNDDMABUF *outbuf, uint16_t spc)
{
    uint16_t size;
    uint16_t i, count;

    DEBUG_BEGIN("_player_setup_outbuf");

    if (player_mode_set)
    {
        outbuf->flags = 0;

        if (player_mode_lq)
            outbuf->flags |= SNDDMABUFFL_LQ;

        set_sample_format(&(outbuf->format),
            player_mode_bits, player_mode_signed, player_mode_channels);

        size = spc;
        if (player_mode_bits == 16)
            size *= 2;

        if (player_mode_channels == 2)
            size *= 2;

        outbuf->frameSize = size;

        i = outbuf->buf->size / size;
        count = 1;
        while (count < i)
            count *= 2;

        if (player_mode_lq)
            count /= 2;

        outbuf->framesCount = count / 2;

        DEBUG_SUCCESS("_player_setup_outbuf");
        return true;
    }
    else
    {
        ERROR ("_player_setup_outbuf", "%s", "No play mode was set.");
        return false;
    }
}

void __far player_set_master_volume (uint8_t value)
{
    if (value > MUSMOD_MASTER_VOLUME_MAX)
        value = MUSMOD_MASTER_VOLUME_MAX;
    playState.master_volume = value;
    amptab_set_volume(value);
}

uint8_t __far player_get_master_volume (void)
{
    return playState.master_volume;
}

void __near _player_setup_patterns_order (MUSMOD *track)
{
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;
    int i, last;
    bool found;

    if (track && musmod_is_loaded (track))
    {
        order = musmod_get_order (track);
        last = muspatorder_get_count (order) - 1;
        found = false;

        if (playOption_ST3Order)
        {
            /* search for first '--' */
            i = 0;
            while ((!found) && (i < last))
            {
                order_entry = muspatorder_get (order, i);
                if (*order_entry == MUSPATORDENT_END)
                    found = true;
                else
                    i++;
            }
            i--;
        }
        else
        {
            /* it is not important, we can also do simply LastOrder = order_length - 1 */
            i = last;
            while ((!found) && (i > 0))
            {
                order_entry = muspatorder_get (order, i);
                if ((*order_entry != MUSPATORDENT_SKIP)
                &&  (*order_entry != MUSPATORDENT_END))
                    found = true;
                else
                    i--;
            }
        }
    }
    else
        i = 0;

    LastOrder = i;
}

void __far player_set_order (bool extended)
{
    MUSMOD *track;

    track = mod_Track;

    if (playOption_ST3Order != extended)
    {
        playOption_ST3Order = extended;
    }

    _player_setup_patterns_order (track);
}

bool __far player_load_s3m (char *name)
{
    LOADER_S3M *p;
    MUSMOD *track;

    p = load_s3m_new();
    if (!p)
    {
        ERROR ("player_load_s3m", "Failed to initialize %s.", "S3M loader");
        return false;
    }
    load_s3m_init(p);

    track = load_s3m_load (p, name);
    mod_Track = track;
    if ((!track) || (!musmod_is_loaded (track)))
    {
        ERROR ("player_load_s3m", "Failed to load S3M file (%s).", load_s3m_get_error (p));
        load_s3m_free(p);
        load_s3m_delete(&p);
        player_free_module();
        return false;
    }

    load_s3m_free(p);
    load_s3m_delete(&p);

    DEBUG_SUCCESS("player_load_s3m");
    return true;
}

bool __near _player_alloc_channels (MIXCHNLIST *channels, MUSMOD *track)
{
    uint8_t num_channels;
    uint8_t i;
    MIXCHN *chn;
    MIXCHNTYPE type;
    MIXCHNPAN pan;
    MIXCHNFLAGS flags;

    num_channels = musmod_get_channels_count (track);

    if (mixchnl_get_count (channels) != num_channels)
        if (!mixchnl_set_count (channels, musmod_get_channels_count (track)))
        {
            ERROR ("_player_alloc_channels", "Failed to allocate memory for %s.", "mixing channels");
            return false;
        }

    for (i = 0; i < num_channels; i++)
    {
        chn = mixchnl_get (channels, i);
        pan = musmod_get_channels (track)[i].pan & MUSMODCHNPANFL_PAN_MASK;

        if (musmod_get_channels (track)[i].pan & MUSMODCHNPANFL_ENABLED)
        {
            type = MIXCHNTYPE_PCM;
            flags = MIXCHNFL_ENABLED | MIXCHNFL_MIXING;
        }
        else
        {
            type = MIXCHNTYPE_NONE;
            flags = 0;
        }

        mixchn_set_type (chn, type);
        mixchn_set_pan (chn, pan);
        mixchn_set_flags (chn, flags);
    }

    return true;
}

void __near _player_reset_channels (MIXCHNLIST *channels)
{
    uint8_t num_channels, i;
    MIXCHN *chn;

    num_channels = mixchnl_get_count (channels);

    for (i = 0; i < num_channels; i++)
    {
        chn = mixchnl_get (channels, i);
        if (mixchn_get_type (chn) != MIXCHNTYPE_NONE)
            mixchn_reset_wave_tables (chn);
    }
}

void __near _player_set_initial_state (MUSMOD *track)
{
    playState_set_tempo (&playState, musmod_get_tempo (track)); // first priority (is output mixer-dependant)
    playState_set_speed (&playState, musmod_get_speed (track)); // second priority (is song's internal value)
    playState.global_volume = musmod_get_global_volume (track); // is song's internal value
    playState.master_volume = musmod_get_master_volume (track); // is song's output
}

void __far player_set_pos (uint8_t start_order, uint8_t start_row, bool keep)
{
    MUSMOD *track;
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;

    track = mod_Track;
    order = musmod_get_order (track);

    // Module
    playState.tick = 1;         // last tick (go to next row)
    playState.row = start_row;  // next row to read from
    playState.order = start_order;  // next order to read from
    order_entry = muspatorder_get (order, start_order);
    playState.pattern = *order_entry;   // next pattern to read from

    if (!keep)
    {
        // reset pattern effects:
        playState.patdelay_count = 0;
        playState.flags &= ~PLAYSTATEFL_PATLOOP;
        playState.patloop_count = 0;
        playState.patloop_start_row = 0;
    }
}

bool __far player_play_start (void)
{
    MUSMOD *track;
    MIXCHNLIST *channels;
    SNDDMABUF *outbuf;
    uint16_t frame_size;

    DEBUG_BEGIN("player_play_start");

    if (!player_flags_bufalloc)
    {
        ERROR ("player_play_start", "%s", "No sound buffers were allocated.");
        return false;
    }

    if (!player_flags_snddev)
    {
        ERROR ("player_play_start", "%s", "No sound device was set.");
        return false;
    }

    if (!player_mode_set)
    {
        ERROR ("player_play_start", "%s", "No play mode was set.");
        return false;
    }

    track = mod_Track;
    if ((!track) || (!musmod_is_loaded (track)))
    {
        ERROR ("player_play_start", "%s", "No music module was loaded.");
        return false;
    }

    // Allocate mixing channels

    channels = _new (MIXCHNLIST);
    if (!channels)
    {
        ERROR ("_player_alloc_channels", "Failed to allocate memory for %s.", "mixing channels");
        return false;
    }
    mixchnl_init (channels);
    mod_Channels = channels;

    if (!_player_alloc_channels (channels, track))
        return false;

    // 1. Setup output mode

    sb_set_transfer_mode(player_device, player_mode_rate, player_mode_channels, player_mode_bits, player_mode_signed);
    player_mode_rate     = sb_mode_get_rate(player_device);
    player_mode_channels = sb_mode_get_channels(player_device);
    player_mode_bits     = sb_mode_get_bits(player_device);
    player_mode_signed   = sb_mode_is_signed(player_device);

    playState.rate = player_mode_lq ? player_mode_rate / 2 : player_mode_rate;

    // 2. Setup mixer mode

    mixbuf_set_mode(
        &mixBuf,
        player_mode_channels,
        ((1000000L / (uint16_t)(1000000L / playState.rate)) / playOption_FPS) + 1
    );

    // 3. Setup output buffer

    outbuf = &sndDMABuf;

    if (!_player_setup_outbuf(outbuf, mixbuf_get_samples_per_channel(&mixBuf)))
    {
        DEBUG_FAIL("player_play_start", "Failed to setup output buffer.");
        return false;
    }

    frame_size = outbuf->frameSize;
    if (player_mode_lq)
        frame_size *= 2;

    sb_set_transfer_buffer(player_device, outbuf->buf->data, frame_size, outbuf->framesCount, true, &ISR_play);

    // 4. Setup mixer tables

    voltab_calc();

    // 5. Setup playing state

    _player_setup_patterns_order (track);
    _player_set_initial_state (track);  // master volume affects mixer tables

    // mixer
    amptab_set_volume (playState.master_volume);

    _player_reset_channels (channels);
    player_set_pos (initState_startOrder, 0, false);
    playState.tick_samples_per_channel_left = 0;    // emmidiately next tick
    playState.flags = 0;    // resume playing

    // 6. Prefill output buffer

    outbuf->frameActive = outbuf->framesCount - 1;
    outbuf->frameLast = outbuf->framesCount;
    fill_DMAbuffer(&mixBuf, outbuf);

    // 7. Start sound

    if (!sb_transfer_start(player_device))
    {
        ERROR ("player_play_start", "%s", "Failed to start transfer.");
        return false;
    }

    DEBUG_SUCCESS("player_play_start");
    return true;
}

void __far player_play_pause (void)
{
    if (player_device)
        sb_transfer_pause(player_device);
}

void __far player_play_continue (void)
{
    if (player_device)
        sb_transfer_continue(player_device);
}

void __far player_play_stop (void)
{
    if (player_device)
        sb_transfer_stop(player_device);
}

uint16_t __far player_get_buffer_pos (void)
{
    return sb_get_buffer_pos(player_device);
}

uint8_t __far player_get_speed (void)
{
    return playState.speed;
}

uint8_t __far player_get_tempo (void)
{
    return playState.tempo;
}

uint8_t __far player_get_pattern_delay (void)
{
    return playState.patdelay_count;
}

void __far player_free_module (void)
{
    MUSMOD *track;
    MIXCHNLIST *channels;

    DEBUG_BEGIN("player_free_module");

    track = mod_Track;
    if (track)
    {
        musmod_free (track);
        _delete (track);
    }
    mod_Track = NULL;

    channels = mod_Channels;
    if (channels)
    {
        mixchnl_free (channels);
        _delete (channels);
    }
    channels = NULL;

    DEBUG_END("player_free_module");
}

void __far player_free_device (void)
{
    DEBUG_BEGIN("player_free_device");

    if (player_device)
    {
        sb_free(player_device);
        sb_delete(&player_device);
    }

    DEBUG_END("player_free_device");
}

void __far player_free (void)
{
    DEBUG_BEGIN("player_free");

    player_play_stop();
    player_free_module();
    player_free_device();
    voltab_free();
    snddmabuf_free(&sndDMABuf);
    mixbuf_free(&mixBuf);
    smpbuf_free(&smpbuf);
    player_flags_bufalloc = false;

    if (UseEMS)
        emsFree(SavHandle);

    DEBUG_END("player_free");
}

/*** Initialization ***/

void __near s3mplay_init(void)
{
    if (DEBUG_WRITE_OUTPUT_STREAM)
    {
        _debug_stream[0] = fopen ("_STREAM0", "wb");
        _debug_stream[1] = fopen ("_STREAM1", "wb");
    }

    player_flags_i386 = isCPU_i386();
    player_flags_bufalloc = false;
    player_flags_snddev = false;
    UseEMS = emsInstalled;
    inside = false;
    player_device = NULL;
    player_mode_set = false;
    player_mode_bits = 0;
    player_mode_signed = false;
    player_mode_channels = 0;
    player_mode_rate = 0;
    player_mode_lq = false;
    playOption_LoopSong = false;
    playOption_ST3Order = false;
    playOption_FPS = 70;
    voltab_init();
    smpbuf_init(&smpbuf);
    mixbuf_init(&mixBuf);
    snddmabuf_init(&sndDMABuf);
    SavHandle = EMSBADHDL;
    mod_Track = NULL;
    mod_Channels = NULL;
}

void __near s3mplay_done(void)
{
    player_free();
    if (DEBUG_WRITE_OUTPUT_STREAM)
    {
        if (_debug_stream[0])
            fclose (_debug_stream[0]);
        if (_debug_stream[1])
            fclose (_debug_stream[1]);
    }
}

DEFINE_REGISTRATION(s3mplay, s3mplay_init, s3mplay_done)
