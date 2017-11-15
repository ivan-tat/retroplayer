/* s3mplay.c -- main library for playing music modules.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/stdio.h"
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

static bool     player_flags_i386;
static bool     player_flags_bufalloc;
static bool     player_flags_snddev;
static bool     player_mode_set;
static uint8_t  player_mode_bits;
static bool     player_mode_signed;
static uint8_t  player_mode_channels;
static uint16_t player_mode_rate;
static bool     player_mode_lq;
static bool     inside;

static EMSNAME _EM_map_name = "saveMAP";

void __far __pascal PlaySoundCallback(void)
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

    fill_DMAbuffer(mixBuf.buf, &sndDMABuf);

    if (UseEMS & !err)
        emsRestoreMap(SavHandle);
}

/* player */

bool PUBLIC_CODE player_init(void)
{
    DEBUG_BEGIN("player_init");

    if (!player_flags_i386)
    {
        DEBUG_FAIL("player_init", "CPU is not supported.");
        player_error = nota386orhigher;
        return false;
    }

    if (UseEMS)
    {
        SavHandle = emsAlloc(1);    // is 1 page enough?
        if (emsEC != E_EMS_SUCCESS)
        {
            DEBUG_FAIL("player_init", "Failed to allocate EM handle for mapping.");
            player_error = internal_failure;
            return false;
        }
        emsSetHandleName(SavHandle, &_EM_map_name);
    }

    if (player_flags_bufalloc)
    {
        DEBUG_FAIL("player_init", "Failed to initialize volume table.");
        player_error = allreadyallocbuffers;
        return true;
    }

    if (!volumetableptr)
        if (!allocVolumeTable())
        {
            DEBUG_FAIL("player_init", "Failed to initialize volume table.");
            player_error = notenoughmem;
            return false;
        }

    if (!sndDMABuf.buf)
        if (!snddmabuf_alloc(&sndDMABuf, DMA_BUF_SIZE_MAX))
        {
            DEBUG_FAIL("player_init", "Failed to initialize sound buffer.");
            player_error = notenoughmem;
            return false;
        }

    if (!mixBuf.buf)
        if (!mixbuf_alloc(&mixBuf, (sndDMABuf.buf->size * sizeof(int16_t)) / 2))
        {
            DEBUG_FAIL("player_init", "Failed to initialize mixing buffer.");
            player_error = notenoughmem;
            return false;
        }
    memset(mixBuf.buf, 0, mixBuf.size);

    player_flags_bufalloc = true;

    DEBUG_SUCCESS("player_init");
    return true;
}

bool PUBLIC_CODE player_init_device(uint8_t type)
{
    DEBUG_BEGIN("player_init_device");

    switch (type)
    {
    case 0:
        player_flags_snddev = true;
        break;
    case 1:
        player_flags_snddev = DetectSoundblaster();
        break;
    case 2:
        player_flags_snddev = UseBlasterEnv();
        break;
    case 3:
        player_flags_snddev = InputSoundblasterValues();
        break;
    default:
        DEBUG_ERR("player_init_device", "Unknown method.");
        player_flags_snddev = false;
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
        return true;
    }
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

uint16_t PUBLIC_CODE playGetSampleRate(void)
{
    return player_mode_rate;
}

bool PUBLIC_CODE player_is_lq_mode(void)
{
    return player_mode_lq;
}

bool __near _player_setup_mixer(void)
{
    uint16_t rate;

    DEBUG_BEGIN("_player_setup_mixer");

    if (player_mode_set)
    {
        rate = player_mode_rate;

        if (player_mode_lq)
            rate /= 2;

        setMixMode(
            player_mode_channels,
            rate,
            ((1000000L / (uint16_t)(1000000L / rate)) / playOption_FPS) + 1
        );

        DEBUG_SUCCESS("_player_setup_mixer");
        return true;
    }
    else
    {
        DEBUG_FAIL("_player_setup_mixer", "No play mode was set.");
        return false;
    }
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

void PUBLIC_CODE playSetMasterVolume(uint8_t value)
{
    if (value > 127)
        value = 127;
    playState_mVolume = value;
    calcPostTable(value);
}

uint8_t PUBLIC_CODE playGetMasterVolume(void)
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

void PUBLIC_CODE playSetOrder(bool extended)
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
        player_error = E_failed_to_load_file;
        player_error_msg = "Failed to initialize S3M loader.";
        return false;
    }
    s3mloader_init(p);

    if (!s3mloader_load(p, name))
    {
        DEBUG_FAIL("player_load_s3m", "Failed to load S3M file.");
        player_error = E_failed_to_load_file;
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

    for (i = 0; i < UsedChannels; i++)
    {
        chn = &Channel[i];
        mixchn_reset_wave_tables(chn);
    }
}

bool PUBLIC_CODE playStart(void)
{
    bool mode_stereo;
    bool mode_16bits;
    SNDDMABUF *outbuf;
    uint16_t count;

    DEBUG_BEGIN("playStart");

    player_error = 0;
    player_error_msg = NULL;

    if (!player_flags_snddev)
    {
        DEBUG_FAIL("playStart", "No sound device was set.");
        player_error = nosounddevice;
        return false;
    }

    if (!player_mode_set)
    {
        DEBUG_FAIL("playStart", "No play mode was set.");
        player_error = internal_failure;
        return false;
    }

    if (!mod_isLoaded)
    {
        DEBUG_FAIL("playStart", "No music module was loaded.");
        player_error = noS3Minmemory;
        return false;
    }

    mode_16bits = player_mode_bits == 16;
    mode_stereo = player_mode_channels == 2;

    set_ready_irq(&PlaySoundCallback);

    sbAdjustMode(&player_mode_rate, &mode_stereo, &mode_16bits);

    Initblaster(&mode_16bits, &mode_stereo, &player_mode_rate);

    if (mode_16bits)
        player_mode_bits = 16;
    else
        player_mode_bits = 8;

    if (mode_stereo)
        player_mode_channels = 2;
    else
        player_mode_channels = 1;

    player_set_mode(mode_16bits, mode_stereo, player_mode_rate, player_mode_lq);

    if (!_player_setup_mixer())
    {
        DEBUG_FAIL("playStart", "Failed to setup mixer.");
        return false;
    }

    outbuf = &sndDMABuf;

    if (!_player_setup_outbuf(outbuf, mixBufSamplesPerChannel))
    {
        DEBUG_FAIL("playStart", "Failed to setup output buffer.");
        return false;
    }

    // now after loading we know if signed data or not
    calcVolumeTable(modOption_SignedData);

    calcPostTable(playState_mVolume);

    _player_setup_patterns_order();

    playState_tick = 1; // last tick (go to next row)
    playState_row = 0;  // next row to read from
    playState_order = initState_startOrder;     // next order to read from
    playState_pattern = Order[playState_order]; // next pattern to read from

    // reset pattern effects:
    playState_patDelayCount = 0;
    playState_patLoopActive = false;
    playState_patLoopStartRow = 0;

    set_speed(initState_speed);
    set_tempo(initState_tempo);

    playState_songEnded = false;

    mixTickSamplesPerChannelLeft = 0;   // emmidiately next tick

    _player_reset_channels();

    count = outbuf->frameSize;
    if (player_mode_lq)
        count *= 2;

    outbuf->frameActive = outbuf->framesCount - 1;
    outbuf->frameLast = outbuf->framesCount;

    // calc all buffer parts
    fill_DMAbuffer(mixBuf.buf, outbuf);

    // loop through whole DMA buffer with double buffering
    sbSetupDMATransfer(outbuf->buf->data, count * (uint16_t)outbuf->framesCount, true);
    sbSetupDSPTransfer(count, true);

    // now everything works in background
    DEBUG_SUCCESS("playStart");
    return true;
}

uint8_t PUBLIC_CODE playGetSpeed(void)
{
    return playState_speed;
}

uint8_t PUBLIC_CODE playGetTempo(void)
{
    return playState_tempo;
}

uint8_t PUBLIC_CODE playGetPatternDelay(void)
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

void PUBLIC_CODE player_free(void)
{
    DEBUG_BEGIN("player_free");

    stop_play();
    player_free_module();
    restore_irq();
    freeVolumeTable();
    snddmabuf_free(&sndDMABuf);
    mixbuf_free(&mixBuf);

    player_flags_bufalloc = false;

    if (UseEMS)
        emsFree(SavHandle);

    DEBUG_END("player_free");
}

/*** Initialization ***/

void __near s3mplay_init(void)
{
    player_flags_i386 = isCPU_i386();
    player_flags_bufalloc = false;
    player_flags_snddev = false;
    UseEMS = emsInstalled;
    inside = false;
    player_mode_set = false;
    player_mode_bits = 0;
    player_mode_signed = false;
    player_mode_channels = 0;
    player_mode_rate = 0;
    player_mode_lq = false;
    playOption_LoopSong = false;
    playOption_ST3Order = false;
    playOption_FPS = 70;
    initVolumeTable();
    mixbuf_init(&mixBuf);
    snddmabuf_init(&sndDMABuf);
    SavHandle = EMSBADHDL;
    mod_Instruments = NULL;
    mod_Patterns = NULL;
}

void __near s3mplay_done(void)
{
    player_free();
}

DEFINE_REGISTRATION(s3mplay, s3mplay_init, s3mplay_done)
