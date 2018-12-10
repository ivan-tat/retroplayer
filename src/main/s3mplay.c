/* s3mplay.c -- main library for playing music modules.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

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

/* Error messages */

typedef uint16_t player_error_t;

enum
{
    E_PLAYER_SUCCESS,
    E_PLAYER_INTERNAL,
    E_PLAYER_UNSUPPORTED_CPU,
    E_PLAYER_LOW_MEMORY,
    E_PLAYER_NO_BUFFERS,
    E_PLAYER_NO_SOUND_DEVICE,
    E_PLAYER_NO_SOUND_MODE,
    E_PLAYER_NO_MODULE,
    E_PLAYER_LOADER,
};

static const struct player_error_msg_t
{
    player_error_t code;
    char *msg;
}
PLAYER_ERRORS[] =
{
    {
        E_PLAYER_INTERNAL,
        "Internal failure"
    },
    {
        E_PLAYER_UNSUPPORTED_CPU,
        "Unsupported CPU"
    },
    {
        E_PLAYER_LOW_MEMORY,
        "Not enough memory"
    },
    {
        E_PLAYER_NO_BUFFERS,
        "No buffers were allocated"
    },
    {
        E_PLAYER_NO_SOUND_DEVICE,
        "No sound device was set"
    },
    {
        E_PLAYER_NO_SOUND_MODE,
        "No sound mode was set"
    },
    {
        E_PLAYER_NO_MODULE,
        "No music module was loaded"
    },
    {
        E_PLAYER_LOADER,
        NULL
    },
    {
        E_PLAYER_SUCCESS,
        "No error"
    }
};

static const char PLAYER_ERROR_UNKNOWN[] = "Unknown error";

/* Private variables */

static player_error_t player_error;
static const char    *player_error_msg;
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

/* Player */

void PUBLIC_CODE player_clear_error(void)
{
    player_error = E_PLAYER_SUCCESS;
    player_error_msg = NULL;
}

bool PUBLIC_CODE player_is_error(void)
{
    return player_error != E_PLAYER_SUCCESS;
}

const char *PUBLIC_CODE player_get_error(void)
{
    int i;

    if (player_error == E_PLAYER_LOADER)
        return player_error_msg;

    i = 0;
    while (PLAYER_ERRORS[i].code != E_PLAYER_SUCCESS)
    {
        if (PLAYER_ERRORS[i].code == player_error)
            return PLAYER_ERRORS[i].msg;
        i++;
    }

    return PLAYER_ERROR_UNKNOWN;
}

bool PUBLIC_CODE player_init(void)
{
    uint16_t len;

    DEBUG_BEGIN("player_init");

    player_clear_error();

    if (!player_flags_i386)
    {
        DEBUG_FAIL("player_init", "CPU is not supported.");
        player_error = E_PLAYER_UNSUPPORTED_CPU;
        return false;
    }

    if (UseEMS)
    {
        SavHandle = emsAlloc(1);    // is 1 page enough?
        if (emsEC != E_EMS_SUCCESS)
        {
            DEBUG_FAIL("player_init", "Failed to allocate EM handle for mapping.");
            player_error = E_PLAYER_INTERNAL;
            return false;
        }
        emsSetHandleName(SavHandle, &_EM_map_name);
    }

    if (!volumetableptr)
        if (!voltab_alloc())
        {
            DEBUG_FAIL("player_init", "Failed to initialize volume table.");
            player_error = E_PLAYER_LOW_MEMORY;
            return false;
        }

    if (!sndDMABuf.buf)
        if (!snddmabuf_alloc(&sndDMABuf, DMA_BUF_SIZE_MAX))
        {
            DEBUG_FAIL("player_init", "Failed to initialize sound buffer.");
            player_error = E_PLAYER_LOW_MEMORY;
            return false;
        }

    len = sndDMABuf.buf->size / 2;  // half DMA transfer buffer size (but in samples)

    if (!smpbuf_get(&smpbuf))
        if (!smpbuf_alloc(&smpbuf, len))
        {
            DEBUG_FAIL("player_init", "Failed to initialize sample buffer.");
            player_error = E_PLAYER_LOW_MEMORY;
            return false;
        }

    if (!mixbuf_get(&mixBuf))
        if (!mixbuf_alloc(&mixBuf, len))
        {
            DEBUG_FAIL("player_init", "Failed to initialize mixing buffer.");
            player_error = E_PLAYER_LOW_MEMORY;
            return false;
        }

    player_flags_bufalloc = true;

    DEBUG_SUCCESS("player_init");
    return true;
}

bool PUBLIC_CODE player_init_device(uint8_t type)
{
    DEBUG_BEGIN("player_init_device");

    if (type <= 3)
    {
        player_device = sb_new();
        if (!player_device)
        {
            DEBUG_FAIL("player_init_device", "Failed to create sound device object.");
            return false;
        }
        sb_init(player_device);
    }
    else
    {
        DEBUG_ERR("player_init_device", "Unknown method.");
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
        DEBUG_FAIL("player_init_device", NULL);
        return false;
    }
}

void PUBLIC_CODE player_device_dump_conf(void)
{
    if (player_flags_snddev)
        sb_conf_dump(player_device);
}

char *PUBLIC_CODE player_device_get_name(void)
{
    if (player_flags_snddev)
        return sb_get_name(player_device);
    else
        return NULL;
}

bool PUBLIC_CODE player_set_mode(bool f_16bits, bool f_stereo, uint16_t rate, bool LQ)
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

uint16_t PUBLIC_CODE player_get_output_rate(void)
{
    return player_mode_rate;
}

uint8_t PUBLIC_CODE player_get_output_channels(void)
{
    return player_mode_channels;
}

uint8_t PUBLIC_CODE player_get_output_bits(void)
{
    return player_mode_bits;
}

bool PUBLIC_CODE player_get_output_lq(void)
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
        DEBUG_FAIL("_player_setup_outbuf", "No play mode was set.");
        return false;
    }
}

void PUBLIC_CODE player_set_master_volume(uint8_t value)
{
    if (value > 127)
        value = 127;
    playState_mVolume = value;
    amptab_set_volume(value);
}

uint8_t PUBLIC_CODE player_get_master_volume(void)
{
    return playState_mVolume;
}

void __near _player_setup_patterns_order(void)
{
    int i;

    if (mod_isLoaded)
    {
        if (playOption_ST3Order)
        {
            /* search for first '--' */
            i = 0;
            while ((i < OrdNum - 1) && (Order[i] < 255))
                i++;
            i--;
        }
        else
        {
            /* it is not important, we can also do simply LastOrder = OrdNum - 1 */
            i = OrdNum - 1;
            while ((i > 0) && (Order[i] >= 254))
                i--;
        }
    }
    else
        i = 0;

    LastOrder = i;
}

void PUBLIC_CODE player_set_order(bool extended)
{
    if (playOption_ST3Order != extended)
    {
        playOption_ST3Order = extended;
    }
        _player_setup_patterns_order();
}

bool PUBLIC_CODE player_load_s3m(char *name)
{
    S3MLOADER *p;

    p = s3mloader_new();
    if (!p)
    {
        DEBUG_FAIL("player_load_s3m", "Failed to initialize S3M loader.");
        player_error = E_PLAYER_LOADER;
        player_error_msg = "Failed to initialize S3M loader.";
        return false;
    }
    s3mloader_init(p);

    if (!s3mloader_load(p, name))
    {
        DEBUG_FAIL("player_load_s3m", "Failed to load S3M file.");
        player_error = E_PLAYER_LOADER;
        player_error_msg = s3mloader_get_error(p);
        s3mloader_free(p);
        s3mloader_delete(&p);
        player_free_module();
        return false;
    }

    s3mloader_free(p);
    s3mloader_delete(&p);

    DEBUG_SUCCESS("player_load_s3m");
    return true;
}

void __near _player_reset_channels(void)
{
    int i;
    MIXCHN *chn;

    for (i = 0; i < mod_ChannelsCount; i++)
    {
        chn = &mod_Channels[i];
        mixchn_reset_wave_tables(chn);
    }
}

void __near _player_set_initial_state(void)
{
    playState_set_speed(initState_speed);
    playState_set_tempo(initState_tempo);
    _player_reset_channels();
}

void player_set_pos(uint8_t start_order, uint8_t start_row, bool keep)
{
    // Module
    playState_tick = 1;         // last tick (go to next row)
    playState_row = start_row;  // next row to read from
    playState_order = start_order;          // next order to read from
    playState_pattern = Order[start_order]; // next pattern to read from

    if (!keep)
    {
        // reset pattern effects:
        playState_patDelayCount = 0;
        playState_patLoopActive = false;
        playState_patLoopCount = 0;
        playState_patLoopStartRow = 0;
    }
}

bool PUBLIC_CODE player_play_start(void)
{
    SNDDMABUF *outbuf;
    uint16_t frame_size;

    DEBUG_BEGIN("player_play_start");

    if (!player_flags_bufalloc)
    {
        DEBUG_FAIL("player_init", "No sound buffers were allocated.");
        player_error = E_PLAYER_NO_BUFFERS;
        return false;
    }

    if (!player_flags_snddev)
    {
        DEBUG_FAIL("player_play_start", "No sound device was set.");
        player_error = E_PLAYER_NO_SOUND_DEVICE;
        return false;
    }

    if (!player_mode_set)
    {
        DEBUG_FAIL("player_play_start", "No play mode was set.");
        player_error = E_PLAYER_NO_SOUND_MODE;
        return false;
    }

    if (!mod_isLoaded)
    {
        DEBUG_FAIL("player_play_start", "No music module was loaded.");
        player_error = E_PLAYER_NO_MODULE;
        return false;
    }

    // 1. Setup output mode

    sb_set_transfer_mode(player_device, player_mode_rate, player_mode_channels, player_mode_bits, player_mode_signed);
    player_mode_rate     = sb_mode_get_rate(player_device);
    player_mode_channels = sb_mode_get_channels(player_device);
    player_mode_bits     = sb_mode_get_bits(player_device);
    player_mode_signed   = sb_mode_is_signed(player_device);

    playState_rate = player_mode_lq ? player_mode_rate / 2 : player_mode_rate;

    // 2. Setup mixer mode

    mixbuf_set_mode(
        &mixBuf,
        player_mode_channels,
        ((1000000L / (uint16_t)(1000000L / playState_rate)) / playOption_FPS) + 1
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
    amptab_set_volume(playState_mVolume);

    // 5. Setup playing state

    _player_setup_patterns_order();
    _player_set_initial_state();
    player_set_pos(initState_startOrder, 0, false);

    playState_tick_samples_per_channel_left = 0;    // emmidiately next tick
    playState_songEnded = false;    // resume playing

    // 6. Prefill output buffer

    outbuf->frameActive = outbuf->framesCount - 1;
    outbuf->frameLast = outbuf->framesCount;
    fill_DMAbuffer(&mixBuf, outbuf);

    // 7. Start sound

    if (!sb_transfer_start(player_device))
    {
        DEBUG_FAIL("player_play_start", "Failed to start transfer.");
        player_error = E_PLAYER_INTERNAL;
        return false;
    }

    DEBUG_SUCCESS("player_play_start");
    return true;
}

void PUBLIC_CODE player_play_pause(void)
{
    if (player_device)
        sb_transfer_pause(player_device);
}

void PUBLIC_CODE player_play_continue(void)
{
    if (player_device)
        sb_transfer_continue(player_device);
}

void PUBLIC_CODE player_play_stop(void)
{
    if (player_device)
        sb_transfer_stop(player_device);
}

uint16_t PUBLIC_CODE player_get_buffer_pos(void)
{
    return sb_get_buffer_pos(player_device);
}

uint8_t PUBLIC_CODE player_get_speed(void)
{
    return playState_speed;
}

uint8_t PUBLIC_CODE player_get_tempo(void)
{
    return playState_tempo;
}

uint8_t PUBLIC_CODE player_get_pattern_delay(void)
{
    return playState_patDelayCount;
}

void PUBLIC_CODE player_free_module(void)
{
    DEBUG_BEGIN("player_free_module");

    if (mod_Instruments)
    {
        musinsl_free(mod_Instruments);
        musinsl_delete(&mod_Instruments);
    }

    if (mod_Patterns)
    {
        muspatl_free(mod_Patterns);
        _delete(mod_Patterns);
    }

    mod_isLoaded = false;

    DEBUG_END("player_free_module");
}

void PUBLIC_CODE player_free_device(void)
{
    DEBUG_BEGIN("player_free_device");

    if (player_device)
    {
        sb_free(player_device);
        sb_delete(&player_device);
    }

    DEBUG_END("player_free_device");
}

void PUBLIC_CODE player_free(void)
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
    mod_Instruments = NULL;
    mod_Patterns = NULL;
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
