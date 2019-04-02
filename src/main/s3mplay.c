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
#include "main/musins.h"
#include "main/muspat.h"
#include "main/musmod.h"
#include "main/loads3m.h"
#include "main/musmodps.h"
#include "main/mixchn.h"
#include "main/effects.h"
#include "main/mixer.h"
#include "main/voltab.h"
#include "main/posttab.h"
#include "main/fillvars.h"
#include "main/filldma.h"

#include "main/s3mplay.h"

#define _PLAYER_ERROR_LEN 128

/* EM handle to save mapping while playing */
static EMSHDL  _EM_map_handle = EMSBADHDL;
static EMSNAME _EM_map_name = "saveMAP";

#pragma pack(push, 1);
typedef struct play_isr_param_t {
    void *player;
    bool busy;
    SNDDMABUF *dmabuf;
};
#pragma pack(pop);
typedef struct play_isr_param_t PLAYISRPARAM;

static char     player_error[_PLAYER_ERROR_LEN];
static bool     player_flags_use_EM;
static bool     player_flags_bufalloc;
static bool     player_flags_snddev;
static SBDEV   *player_device;
static uint8_t  player_sound_buffer_fps;
static bool     player_mode_set;
static uint8_t  player_mode_bits;
static bool     player_mode_signed;
static uint8_t  player_mode_channels;
static uint16_t player_mode_rate;
static bool     player_mode_lq;
static MUSMOD  *_player_track;
static MIXCHNLIST *_player_mixing_channels;
static PLAYSTATE _player_play_state;
static SMPBUF   _player_smpbuf;
static MIXBUF   _player_mixbuf;
static MIXER   *_player_mixer;
static PLAYISRPARAM player_play_isr_param;

/* Each element is a pointer */

typedef struct pointers_list_t
{
    DYNARR list;
};
typedef struct pointers_list_t PTRLIST;

#define _ptrlist_get_list(o)        & ((o)->list)
#define _ptrlist_set_count(o, v)    dynarr_set_size (_ptrlist_get_list (o), v)
#define _ptrlist_get_count(o)       dynarr_get_size (_ptrlist_get_list (o))
#define _ptrlist_set(o, i, v)       dynarr_set_item (_ptrlist_get_list (o), i, v)
#define _ptrlist_get(o, i)          dynarr_get_item (_ptrlist_get_list (o), i)

void __near ptrlist_init (PTRLIST *self);
#define     ptrlist_set_count(o, v) _ptrlist_set_count (o, v)
#define     ptrlist_get_count(o)    _ptrlist_get_count (o)
#define     ptrlist_set(o, i, v)    _ptrlist_set (o, i, v)
#define     ptrlist_get(o, i)       _ptrlist_get (o, i)
void __near ptrlist_free (PTRLIST *self);

void __far _ptrlist_init_item (void *self, void *item)
{
    *(void **) item = NULL;
}

void __far _ptrlist_free_item (void *self, void *item)
{
    *((void **) item) = NULL;
}

void __near ptrlist_init (PTRLIST *self)
{
    if (self)
        dynarr_init (_ptrlist_get_list (self), self, sizeof (MUSMOD *), _ptrlist_init_item, _ptrlist_free_item);
}

void __near ptrlist_free (PTRLIST *self)
{
    if (self)
        dynarr_free (_ptrlist_get_list (self));
}

/*** Musical modules list ***/

typedef PTRLIST MUSMODLIST;

#define _musmodlist_init(o)         ptrlist_init (o)
#define _musmodlist_set_count(o, v) ptrlist_set_count (o, v)
#define _musmodlist_get_count(o)    ptrlist_get_count (o)
#define _musmodlist_set(o, i, v)    ptrlist_set (o, i, v)
#define _musmodlist_get(o, i)       (MUSMOD **) ptrlist_get (o, i)
#define _musmodlist_free(o)         ptrlist_free (o)

static MUSMODLIST _musmodlist;

void __near _init_modules (void)
{
    _musmodlist_init (&_musmodlist);
}

bool __near _add_module (MUSMOD *track)
{
    int count;

    count = _musmodlist_get_count (&_musmodlist);

    if (!_musmodlist_set_count (&_musmodlist, count + 1))
        return false;

    _musmodlist_set (&_musmodlist, count, &track);
    return true;
}

void __near _free_module (MUSMOD *track)
{
    musmod_free (track);
    _delete (track);
}

void __near _free_modules (void)
{
    int count, i;
    MUSMOD **track;

    count = _musmodlist_get_count (&_musmodlist);
    for (i = 0; i < count; i++)
    {
        track = _musmodlist_get (&_musmodlist, i);
        if (track && *track)
        {
            _free_module (*track);
            _musmodlist_set (&_musmodlist, i, NULL);
        }
    }

    _musmodlist_free (&_musmodlist);
}

/*** Sound ***/

/* IRQ routines */

void __far ISR_play (void *param)
{
    PLAYISRPARAM *self;
    void *player;
    SNDDMABUF *dmabuf;
    bool err;

    self = (PLAYISRPARAM *) param;
    player = self->player;
    dmabuf = self->dmabuf;

    if (self->busy)
    {
        dmabuf->flags |= SNDDMABUFFL_SLOW;
        dmabuf->frameActive = (dmabuf->frameActive + 1) & (dmabuf->framesCount - 1);
    }
    else
    {
        self->busy = true;

        dmabuf->frameActive = (dmabuf->frameActive + 1) & (dmabuf->framesCount - 1);

        err = false;

        if (player_flags_use_EM)
        {
            err = true;
            if (emsSaveMap (_EM_map_handle))
                err = false;
        }

        fill_DMAbuffer (_player_track, &_player_play_state, _player_mixing_channels, _player_mixer, dmabuf);

        if (player_flags_use_EM & !err)
            emsRestoreMap (_EM_map_handle);

        self->busy = false;
    }
}

/*** Player ***/

/* Error handling */

void __near _player_clear_error (void)
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

/* Initialization */

bool __far player_init (void)
{
    SMPBUF *_smpbuf;
    MIXBUF *_mixbuf;
    uint16_t len;

    DEBUG_BEGIN ("player_init");

    _player_clear_error ();

    player_flags_use_EM = emsInstalled;
    player_flags_bufalloc = false;
    player_flags_snddev = false;

    // Sound
    _EM_map_handle = EMSBADHDL;
    snddmabuf_init (&sndDMABuf);

    // Mixer
    smpbuf_init (&_player_smpbuf);
    mixbuf_init (&_player_mixbuf);
    voltab_init();
    _player_mixer = NULL;

    // Player
    player_device = NULL;
    player_sound_buffer_fps = 70;
    player_mode_set = false;
    player_mode_bits = 0;
    player_mode_signed = false;
    player_mode_channels = 0;
    player_mode_rate = 0;
    player_mode_lq = false;

    // Music modules
    _init_modules ();
    _player_track = NULL;
    _player_mixing_channels = NULL;

    if (!isCPU_i386 ())
    {
        ERROR ("player_init", "%s", "CPU is not supported.");
        return false;
    }

    if (player_flags_use_EM)
    {
        _EM_map_handle = emsAlloc (1);  // is 1 page enough?
        if (emsEC != E_EMS_SUCCESS)
        {
            ERROR ("player_init", "%s", "Failed to allocate EM handle for mapping.");
            return false;
        }
        emsSetHandleName (_EM_map_handle, &_EM_map_name);
    }

    // Sound

    if (!sndDMABuf.buf)
        if (!snddmabuf_alloc(&sndDMABuf, DMA_BUF_SIZE_MAX))
        {
            ERROR ("player_init", "Failed to initialize %s.", "DMA buffer");
            return false;
        }

    // Mixer

    if (!_player_mixer)
    {
        _player_mixer = _new (MIXER);
        if (!_player_mixer)
        {
            ERROR ("player_init", "Failed to initialize %s.", "sound mixer");
            return false;
        }
        mixer_init (_player_mixer);
        mixer_set_smpbuf (_player_mixer, &_player_smpbuf);
        mixer_set_mixbuf (_player_mixer, &_player_mixbuf);
    }

    len = sndDMABuf.buf->size / 2;  // half DMA transfer buffer size (but in samples)

    _smpbuf = mixer_get_smpbuf (_player_mixer);
    if (!smpbuf_get (_smpbuf))
        if (!smpbuf_alloc (_smpbuf, len))
        {
            ERROR ("player_init", "Failed to initialize %s.", "sample buffer");
            return false;
        }

    _mixbuf = mixer_get_mixbuf (_player_mixer);
    if (!mixbuf_get (_mixbuf))
        if (!mixbuf_alloc (_mixbuf, len))
        {
            ERROR ("player_init", "Failed to initialize %s.", "mixing buffer");
            return false;
        }

    if (!volumetableptr)
        if (!voltab_alloc())
        {
            ERROR ("player_init", "Failed to initialize %s.", "volume table");
            return false;
        }

    player_flags_bufalloc = true;

    DEBUG_SUCCESS ("player_init");
    return true;
}

/* Private and public methods */

void __far player_set_EM_usage (bool value)
{
    player_flags_use_EM = value && emsInstalled;
}

bool __far player_is_EM_in_use (void)
{
    return player_flags_use_EM;
}

bool __far player_init_device (SNDDEVTYPE type, SNDDEVSETMET method)
{
    bool result;

    DEBUG_BEGIN("player_init_device");

    switch (type)
    {
    case SNDDEVTYPE_SB:
        player_device = sb_new();
        if (!player_device)
        {
            ERROR ("player_init_device", "%s", "Failed to create sound device object.");
            return false;
        }
        sb_init(player_device);
        break;
    default:
        ERROR ("player_init_device", "%s", "Unknown device type.");
        return false;
    }

    switch (method)
    {
    case SNDDEVSETMET_MANUAL:
        result = true;
        break;
    case SNDDEVSETMET_DETECT:
        result = sb_conf_detect(player_device);
        break;
    case SNDDEVSETMET_ENV:
        result = sb_conf_env(player_device);
        break;
    case SNDDEVSETMET_INPUT:
        result = sb_conf_input(player_device);
        break;
    default:
        ERROR ("player_init_device", "%s", "Unknown method.");
        sb_free (player_device);
        return false;
    }

    if (result)
    {
        DEBUG_SUCCESS("player_init_device");
        player_flags_snddev = true;
        return true;
    }
    else
    {
        ERROR ("player_init_device", "%s", "No sound device.");
        sb_free (player_device);
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

void __far player_set_sound_buffer_fps (uint8_t value)
{
    player_sound_buffer_fps = value;
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
    PLAYSTATE *ps;

    ps = &_player_play_state;
    if (value > MUSMOD_MASTER_VOLUME_MAX)
        value = MUSMOD_MASTER_VOLUME_MAX;
    ps->master_volume = value;
    amptab_set_volume(value);
}

uint8_t __far player_get_master_volume (void)
{
    PLAYSTATE *ps;

    ps = &_player_play_state;
    return ps->master_volume;
}

MIXER *__far player_get_mixer (void)
{
    return _player_mixer;
}

void __near _player_setup_patterns_order (MUSMOD *track, PLAYSTATE *ps)
{
    MUSPATORDER *order;
    int i;

    if (track && musmod_is_loaded (track))
        i = muspatorder_find_last (musmod_get_order (track), ps->flags & PLAYSTATEFL_SKIPENDMARK);
    else
        i = 0;

    ps->order_last = i;
}

void __far player_set_order (bool skipend)
{
    MUSMOD *track;
    PLAYSTATE *ps;

    track = _player_track;
    ps = &_player_play_state;

    if (skipend)
        ps->flags |= PLAYSTATEFL_SKIPENDMARK;
    else
        ps->flags &= ~PLAYSTATEFL_SKIPENDMARK;

    _player_setup_patterns_order (track, ps);
}

void __far player_set_order_start (uint8_t value)
{
    PLAYSTATE *ps;

    ps = &_player_play_state;

    ps->order_start = value;
}

int __far player_find_next_pattern (MUSMOD *track, PLAYSTATE *ps, int index, int step)
{
    MUSPATORDER *order;
    int start, last, pos;
    bool skipend;

    order = musmod_get_order (track);
    start = ps->order_start;
    last = ps->order_last;
    pos = index;
    skipend =  ps->flags & PLAYSTATEFL_SKIPENDMARK;

    // Check bounds

    if ((step < 0) && (pos <= start))
        // Rewind
        return muspatorder_find_next_pattern (order, start, last, start, 1, skipend);

    if ((step > 0) && (pos >= ps->order_last))
    {
        if (ps->flags & PLAYSTATEFL_SONGLOOP)
            // Rewind
            return muspatorder_find_next_pattern (order, start, last, start, 1, skipend);
        else
            // Stop
            return -1;
    }

    pos = muspatorder_find_next_pattern (order, start, last, pos + step, step, skipend);

    if (pos < 0)
    {
        if ((step < 0) || (ps->flags & PLAYSTATEFL_SONGLOOP))
            // Rewind
            return muspatorder_find_next_pattern (order, start, last, start, 1, skipend);
    }

    return pos;
}

void __far player_set_song_loop (bool value)
{
    PLAYSTATE *ps;

    ps = &_player_play_state;

    if (value)
        ps->flags |= PLAYSTATEFL_SONGLOOP;
    else
        ps->flags &= ~PLAYSTATEFL_SONGLOOP;
}

bool __far player_load_s3m (char *name, MUSMOD **_track)
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

    track = load_s3m_load (p, name, player_flags_use_EM);
    if ((!track) || (!musmod_is_loaded (track)))
    {
        ERROR ("player_load_s3m", "Failed to load S3M file (%s).", load_s3m_get_error (p));
        // free partially loaded track
        if (track)
            _free_module (track);
        load_s3m_free(p);
        load_s3m_delete(&p);
        return false;
    }

    load_s3m_free(p);
    load_s3m_delete(&p);

    if (!_add_module (track))
    {
        ERROR ("player_load_s3m", "%s", "Failed to register loaded music module.");
        _free_module (track);
        return false;
    }

    _player_track = track;  // set active track
    *_track = track;

    DEBUG_SUCCESS("player_load_s3m");
    return true;
}

MIXCHNLIST *__far player_get_mixing_channels (void)
{
    return _player_mixing_channels;
}

PLAYSTATE *__far player_get_play_state (void)
{
    return &_player_play_state;
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

void __near _player_set_initial_state (MUSMOD *track, PLAYSTATE *ps)
{
    playState_set_tempo (ps, musmod_get_tempo (track)); // first priority (is output mixer-dependant)
    playState_set_speed (ps, musmod_get_speed (track)); // second priority (is song's internal value)
    ps->global_volume = musmod_get_global_volume (track); // is song's internal value
    ps->master_volume = musmod_get_master_volume (track); // is song's output
}

void __far player_set_pos (MUSMOD *track, PLAYSTATE *ps, uint8_t start_order, uint8_t start_row, bool keep)
{
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;

    order = musmod_get_order (track);

    // Module
    ps->order = start_order;                // next order to read from
    order_entry = muspatorder_get (order, start_order);
    ps->pattern = *order_entry;             // next pattern to read from
    ps->row = start_row;                    // next row to read from
    ps->tick = 1;                           // last tick (go to next row)
    ps->tick_samples_per_channel_left = 0;  // immediately next tick

    if (!keep)
    {
        // reset pattern effects:
        ps->patdelay_count = 0;
        ps->flags &= ~PLAYSTATEFL_PATLOOP;
        ps->patloop_count = 0;
        ps->patloop_start_row = 0;
    }
}

void __far player_song_stop (MUSMOD *track, PLAYSTATE *ps)
{
    ps->flags |= PLAYSTATEFL_END;
}

bool __far player_play_start (void)
{
    MUSMOD *track;
    PLAYSTATE *ps;
    MIXCHNLIST *channels;
    SNDDMABUF *outbuf;
    uint16_t frame_size;

    DEBUG_BEGIN("player_play_start");

    ps = &_player_play_state;

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

    track = _player_track;
    if ((!track) || (!musmod_is_loaded (track)))
    {
        ERROR ("player_play_start", "%s", "No music module was loaded.");
        return false;
    }

    // Allocate mixing channels

    channels = _new (MIXCHNLIST);
    if (!channels)
    {
        ERROR ("player_play_start", "Failed to allocate memory for %s.", "mixing channels");
        return false;
    }
    mixchnl_init (channels);
    _player_mixing_channels = channels;

    if (!_player_alloc_channels (channels, track))
        return false;

    // 1. Setup output mode

    sb_set_transfer_mode(player_device, player_mode_rate, player_mode_channels, player_mode_bits, player_mode_signed);
    player_mode_rate     = sb_mode_get_rate(player_device);
    player_mode_channels = sb_mode_get_channels(player_device);
    player_mode_bits     = sb_mode_get_bits(player_device);
    player_mode_signed   = sb_mode_is_signed(player_device);

    ps->rate = player_mode_lq ? player_mode_rate / 2 : player_mode_rate;

    // 2. Setup mixer mode

    mixbuf_set_mode(
        &_player_mixbuf,
        player_mode_channels,
        ((1000000L / (uint16_t)(1000000L / ps->rate)) / player_sound_buffer_fps) + 1
    );

    // 3. Setup output buffer

    outbuf = &sndDMABuf;

    if (!_player_setup_outbuf(outbuf, mixbuf_get_samples_per_channel(&_player_mixbuf)))
    {
        DEBUG_FAIL("player_play_start", "Failed to setup output buffer.");
        return false;
    }

    frame_size = outbuf->frameSize;
    if (player_mode_lq)
        frame_size *= 2;

    player_play_isr_param.busy = false;
    player_play_isr_param.player = NULL;
    player_play_isr_param.dmabuf = &sndDMABuf;
    sb_set_transfer_buffer(player_device, outbuf->buf->data, frame_size, outbuf->framesCount, true, &ISR_play, &player_play_isr_param);

    // 4. Setup mixer tables

    voltab_calc();

    // 5. Setup playing state

    _player_setup_patterns_order (track, ps);
    _player_set_initial_state (track, ps);  // master volume affects mixer tables

    // mixer
    amptab_set_volume (ps->master_volume);

    _player_reset_channels (channels);
    player_set_pos (track, ps, ps->order_start, 0, false);
    ps->flags = 0;    // resume playing

    // 6. Prefill output buffer

    outbuf->frameLast = -1;
    outbuf->frameActive = outbuf->framesCount - 1;
    fill_DMAbuffer (track, ps, channels, _player_mixer, outbuf);

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
    PLAYSTATE *ps;

    ps = &_player_play_state;
    return ps->speed;
}

uint8_t __far player_get_tempo (void)
{
    PLAYSTATE *ps;

    ps = &_player_play_state;
    return ps->tempo;
}

uint8_t __far player_get_pattern_delay (void)
{
    PLAYSTATE *ps;

    ps = &_player_play_state;
    return ps->patdelay_count;
}

void __far player_free_module (MUSMOD *track)
{
    DEBUG_BEGIN ("player_free_module");
    if (track)
    {
        if (_player_track == track)
            _player_track = NULL;
        _free_module (track);
    }
    DEBUG_END ("player_free_module");
}

void __far player_free_modules (void)
{
    MIXCHNLIST *channels;

    DEBUG_BEGIN ("player_free_modules");

    _free_modules ();

    channels = _player_mixing_channels;
    if (channels)
    {
        mixchnl_free (channels);
        _delete (channels);
    }
    _player_mixing_channels = NULL;

    DEBUG_END ("player_free_modules");
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

/* Finalization */

void __far player_free (void)
{
    DEBUG_BEGIN("player_free");

    player_play_stop();
    player_free_modules ();
    player_free_device();
    voltab_free();
    snddmabuf_free(&sndDMABuf);

    if (_player_mixer)
    {
        mixer_free (_player_mixer);
        _delete (_player_mixer);
    }
    smpbuf_free (&_player_smpbuf);
    mixbuf_free (&_player_mixbuf);

    player_flags_bufalloc = false;

    if (player_flags_use_EM)
        emsFree (_EM_map_handle);

    DEBUG_END("player_free");
}

/*** Initialization ***/

void __near s3mplay_init(void)
{
#if DEBUG_WRITE_OUTPUT_STREAM == 1
    DEBUG_open_output_streams ();
#endif  /* DEBUG_WRITE_OUTPUT_STREAM */
}

void __near s3mplay_done(void)
{
    player_free();

#if DEBUG_WRITE_OUTPUT_STREAM == 1
    DEBUG_close_output_streams ();
#endif  /* DEBUG_WRITE_OUTPUT_STREAM */
}

DEFINE_REGISTRATION(s3mplay, s3mplay_init, s3mplay_done)
