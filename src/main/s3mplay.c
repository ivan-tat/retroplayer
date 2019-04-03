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

// Current index to generate unique EM handle name to save EM mapping while in ISR
static uint16_t _isr_index;

typedef struct _music_player_t _MUSPLAYER;

#pragma pack(push, 1);
typedef struct play_isr_param_t {
    _MUSPLAYER *player;
    bool busy;
    SNDDMABUF *dmabuf;
};
#pragma pack(pop);
typedef struct play_isr_param_t PLAYISRPARAM;

#pragma pack(push, 1);
typedef struct _music_player_t
{
    char error[_PLAYER_ERROR_LEN];
    bool flags_use_EM;
    bool flags_bufalloc;
    bool flags_snddev;
    SBDEV *device;
    uint8_t sound_buffer_fps;
    bool     mode_set;
    uint8_t  mode_bits;
    bool     mode_signed;
    uint8_t  mode_channels;
    uint16_t mode_rate;
    bool     mode_lq;
    MUSMOD     *track;
    MIXCHNLIST *channels;
    PLAYSTATE   play_state;
    SMPBUF smpbuf;
    MIXBUF mixbuf;
    MIXER *mixer;
    EMSHDL EM_map_handle;
    EMSNAME EM_map_name;
    PLAYISRPARAM play_isr_param;
};
#pragma pack(pop);

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
    _MUSPLAYER *player;
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

        if (player->flags_use_EM)
        {
            err = true;
            if (emsSaveMap (player->EM_map_handle))
                err = false;
        }

        fill_DMAbuffer (
            player->track,
            &player->play_state,
            player->channels,
            player->mixer,
            dmabuf
        );

        if (player->flags_use_EM && !err)
            emsRestoreMap (player->EM_map_handle);

        self->busy = false;
    }
}

/*** Player ***/

/* Error handling */

void __near _player_clear_error (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    _Self->error[0] = 0;
}

bool __far player_is_error (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->error[0] != 0;
}

const char *__far player_get_error (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self->error[0] != 0)
        return _Self->error;
    else
        return NULL;
}

#if DEBUG == 1

void __near player_set_error (MUSPLAYER *self, const char *method, int line, const char *format, ...)
{
    va_list ap;
    _MUSPLAYER *_Self = self;

    va_start(ap, format);
    vsnprintf (_Self->error, _PLAYER_ERROR_LEN, format, ap);
    _DEBUG_LOG (DBGLOG_ERR, __FILE__, line, method, "%s", _Self->error);
}

#define ERROR(m, f, ...) player_set_error (self, m, __LINE__, f, __VA_ARGS__)

#else

void __near player_set_error (MUSPLAYER *self, const char *format, ...)
{
    va_list ap;
    _MUSPLAYER *_Self = self;

    va_start(ap, format);
    vsnprintf (_Self->error, _PLAYER_ERROR_LEN, format, ap);
}

#define ERROR(m, f, ...) player_set_error (self, f, __VA_ARGS__)

#endif  /* DEBUG */

/* Initialization */

MUSPLAYER *__far player_new (void)
{
    return _new (_MUSPLAYER);
}

bool __far player_init (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    SMPBUF *_smpbuf;
    MIXBUF *_mixbuf;
    uint16_t len;

    DEBUG_BEGIN ("player_init");

    if (_Self)
    {
        memset (_Self, 0, sizeof (_MUSPLAYER));
        _Self->flags_use_EM = emsInstalled;
    }
    else
    {
        DEBUG_FAIL ("player_init", "Empty object.");
        return false;
    }

    // Sound
    _Self->EM_map_handle = EMSBADHDL;
    snddmabuf_init (&sndDMABuf);

    // Mixer
    smpbuf_init (&_Self->smpbuf);
    mixbuf_init (&_Self->mixbuf);
    voltab_init();
    _Self->mixer = NULL;

    // Player
    _Self->device = NULL;
    _Self->sound_buffer_fps = 70;
    _Self->mode_set = false;
    _Self->mode_bits = 0;
    _Self->mode_signed = false;
    _Self->mode_channels = 0;
    _Self->mode_rate = 0;
    _Self->mode_lq = false;

    // Music modules
    _init_modules ();
    _Self->track = NULL;
    _Self->channels = NULL;

    if (!isCPU_i386 ())
    {
        ERROR ("player_init", "%s", "CPU is not supported.");
        return false;
    }

    if (_Self->flags_use_EM)
    {
        _Self->EM_map_handle = emsAlloc (1);  // is 1 page enough?
        if (emsEC != E_EMS_SUCCESS)
        {
            ERROR ("player_init", "%s", "Failed to allocate EM handle for mapping.");
            return false;
        }
        snprintf (&_Self->EM_map_name, sizeof (EMSNAME), "map%04x", _isr_index);
        _isr_index++;
        emsSetHandleName (_Self->EM_map_handle, &_Self->EM_map_name);
    }

    // Sound

    if (!sndDMABuf.buf)
        if (!snddmabuf_alloc(&sndDMABuf, DMA_BUF_SIZE_MAX))
        {
            ERROR ("player_init", "Failed to initialize %s.", "DMA buffer");
            return false;
        }

    // Mixer

    if (!_Self->mixer)
    {
        _Self->mixer = _new (MIXER);
        if (!_Self->mixer)
        {
            ERROR ("player_init", "Failed to initialize %s.", "sound mixer");
            return false;
        }
        mixer_init (_Self->mixer);
        mixer_set_smpbuf (_Self->mixer, &_Self->smpbuf);
        mixer_set_mixbuf (_Self->mixer, &_Self->mixbuf);
    }

    len = sndDMABuf.buf->size / 2;  // half DMA transfer buffer size (but in samples)

    _smpbuf = mixer_get_smpbuf (_Self->mixer);
    if (!smpbuf_get (_smpbuf))
        if (!smpbuf_alloc (_smpbuf, len))
        {
            ERROR ("player_init", "Failed to initialize %s.", "sample buffer");
            return false;
        }

    _mixbuf = mixer_get_mixbuf (_Self->mixer);
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

    _Self->flags_bufalloc = true;

    DEBUG_SUCCESS ("player_init");
    return true;
}

/* Private and public methods */

void __far player_set_EM_usage (MUSPLAYER *self, bool value)
{
    _MUSPLAYER *_Self = self;

    _Self->flags_use_EM = value && emsInstalled;
}

bool __far player_is_EM_in_use (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->flags_use_EM;
}

bool __far player_init_device (MUSPLAYER *self, SNDDEVTYPE type, SNDDEVSETMET method)
{
    _MUSPLAYER *_Self = self;
    bool result;

    DEBUG_BEGIN("player_init_device");

    switch (type)
    {
    case SNDDEVTYPE_SB:
        _Self->device = sb_new();
        if (!_Self->device)
        {
            ERROR ("player_init_device", "%s", "Failed to create sound device object.");
            return false;
        }
        sb_init (_Self->device);
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
        result = sb_conf_detect (_Self->device);
        break;
    case SNDDEVSETMET_ENV:
        result = sb_conf_env (_Self->device);
        break;
    case SNDDEVSETMET_INPUT:
        result = sb_conf_input (_Self->device);
        break;
    default:
        ERROR ("player_init_device", "%s", "Unknown method.");
        sb_free (_Self->device);
        return false;
    }

    if (result)
    {
        DEBUG_SUCCESS("player_init_device");
        _Self->flags_snddev = true;
        return true;
    }
    else
    {
        ERROR ("player_init_device", "%s", "No sound device.");
        sb_free (_Self->device);
        return false;
    }
}

void __far player_device_dump_conf (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self->flags_snddev)
        sb_conf_dump (_Self->device);
}

char *__far player_device_get_name (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self->flags_snddev)
        return sb_get_name (_Self->device);
    else
        return NULL;
}

void __far player_set_sound_buffer_fps (MUSPLAYER *self, uint8_t value)
{
    _MUSPLAYER *_Self = self;

    _Self->sound_buffer_fps = value;
}

bool __far player_set_mode (MUSPLAYER *self, bool f_16bits, bool f_stereo, uint16_t rate, bool LQ)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN("player_set_mode");

    if (f_16bits)
    {
        _Self->mode_bits = 16;
        _Self->mode_signed = true;
    }
    else
    {
        _Self->mode_bits = 8;
        _Self->mode_signed = false;
    }

    if (f_stereo)
        _Self->mode_channels = 2;
    else
        _Self->mode_channels = 1;

    _Self->mode_rate = rate;
    _Self->mode_lq = LQ;
    _Self->mode_set = true;

    DEBUG_SUCCESS("player_set_mode");
    return true;
}

uint16_t __far player_get_output_rate (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->mode_rate;
}

uint8_t __far player_get_output_channels (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->mode_channels;
}

uint8_t __far player_get_output_bits (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->mode_bits;
}

bool __far player_get_output_lq (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->mode_lq;
}

bool __near _player_setup_outbuf(MUSPLAYER *self, SNDDMABUF *outbuf, uint16_t spc)
{
    _MUSPLAYER *_Self = self;
    uint16_t size;
    uint16_t i, count;

    DEBUG_BEGIN("_player_setup_outbuf");

    if (_Self->mode_set)
    {
        outbuf->flags = 0;

        if (_Self->mode_lq)
            outbuf->flags |= SNDDMABUFFL_LQ;

        set_sample_format(&(outbuf->format),
            _Self->mode_bits, _Self->mode_signed, _Self->mode_channels);

        size = spc;
        if (_Self->mode_bits == 16)
            size *= 2;

        if (_Self->mode_channels == 2)
            size *= 2;

        outbuf->frameSize = size;

        i = outbuf->buf->size / size;
        count = 1;
        while (count < i)
            count *= 2;

        if (_Self->mode_lq)
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

void __far player_set_master_volume (MUSPLAYER *self, uint8_t value)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = &_Self->play_state;
    if (value > MUSMOD_MASTER_VOLUME_MAX)
        value = MUSMOD_MASTER_VOLUME_MAX;
    ps->master_volume = value;
    amptab_set_volume(value);
}

uint8_t __far player_get_master_volume (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = &_Self->play_state;
    return ps->master_volume;
}

MIXER *__far player_get_mixer (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->mixer;
}

void __near _player_setup_patterns_order (MUSPLAYER *self, MUSMOD *track, PLAYSTATE *ps)
{
    _MUSPLAYER *_Self = self;
    MUSPATORDER *order;
    int i;

    if (track && musmod_is_loaded (track))
        i = muspatorder_find_last (musmod_get_order (track), ps->flags & PLAYSTATEFL_SKIPENDMARK);
    else
        i = 0;

    ps->order_last = i;
}

void __far player_set_order (MUSPLAYER *self, bool skipend)
{
    _MUSPLAYER *_Self = self;
    MUSMOD *track;
    PLAYSTATE *ps;

    track = _Self->track;
    ps = &_Self->play_state;

    if (skipend)
        ps->flags |= PLAYSTATEFL_SKIPENDMARK;
    else
        ps->flags &= ~PLAYSTATEFL_SKIPENDMARK;

    _player_setup_patterns_order (self, track, ps);
}

void __far player_set_order_start (MUSPLAYER *self, uint8_t value)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = &_Self->play_state;

    ps->order_start = value;
}

int __far player_find_next_pattern (MUSPLAYER *self, MUSMOD *track, PLAYSTATE *ps, int index, int step)
{
    _MUSPLAYER *_Self = self;
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

void __far player_set_song_loop (MUSPLAYER *self, bool value)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = &_Self->play_state;

    if (value)
        ps->flags |= PLAYSTATEFL_SONGLOOP;
    else
        ps->flags &= ~PLAYSTATEFL_SONGLOOP;
}

bool __far player_load_s3m (MUSPLAYER *self, char *name, MUSMOD **_track)
{
    _MUSPLAYER *_Self = self;
    LOADER_S3M *p;
    MUSMOD *track;

    p = load_s3m_new();
    if (!p)
    {
        ERROR ("player_load_s3m", "Failed to initialize %s.", "S3M loader");
        return false;
    }
    load_s3m_init(p);

    track = load_s3m_load (p, name, _Self->flags_use_EM);
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

    _Self->track = track;  // set active track
    *_track = track;

    DEBUG_SUCCESS("player_load_s3m");
    return true;
}

MIXCHNLIST *__far player_get_mixing_channels (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->channels;
}

PLAYSTATE *__far player_get_play_state (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return &_Self->play_state;
}

bool __near _player_alloc_channels (MUSPLAYER *self, MIXCHNLIST *channels, MUSMOD *track)
{
    _MUSPLAYER *_Self = self;
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

void __near _player_reset_channels (MUSPLAYER *self, MIXCHNLIST *channels)
{
    _MUSPLAYER *_Self = self;
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

void __near _player_set_initial_state (MUSPLAYER *self, MUSMOD *track, PLAYSTATE *ps)
{
    _MUSPLAYER *_Self = self;

    playState_set_tempo (ps, musmod_get_tempo (track)); // first priority (is output mixer-dependant)
    playState_set_speed (ps, musmod_get_speed (track)); // second priority (is song's internal value)
    ps->global_volume = musmod_get_global_volume (track); // is song's internal value
    ps->master_volume = musmod_get_master_volume (track); // is song's output
}

void __far player_set_pos (MUSPLAYER *self, MUSMOD *track, PLAYSTATE *ps, uint8_t start_order, uint8_t start_row, bool keep)
{
    _MUSPLAYER *_Self = self;
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

void __far player_song_stop (MUSPLAYER *self, MUSMOD *track, PLAYSTATE *ps)
{
    _MUSPLAYER *_Self = self;

    ps->flags |= PLAYSTATEFL_END;
}

bool __far player_play_start (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    MUSMOD *track;
    PLAYSTATE *ps;
    MIXCHNLIST *channels;
    SNDDMABUF *outbuf;
    uint16_t frame_size;

    DEBUG_BEGIN("player_play_start");

    ps = &_Self->play_state;

    if (!(_Self->flags_bufalloc))
    {
        ERROR ("player_play_start", "%s", "No sound buffers were allocated.");
        return false;
    }

    if (!_Self->flags_snddev)
    {
        ERROR ("player_play_start", "%s", "No sound device was set.");
        return false;
    }

    if (!_Self->mode_set)
    {
        ERROR ("player_play_start", "%s", "No play mode was set.");
        return false;
    }

    track = _Self->track;
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
    _Self->channels = channels;

    if (!_player_alloc_channels (self, channels, track))
        return false;

    // 1. Setup output mode

    sb_set_transfer_mode (_Self->device, _Self->mode_rate, _Self->mode_channels, _Self->mode_bits, _Self->mode_signed);
    _Self->mode_rate     = sb_mode_get_rate (_Self->device);
    _Self->mode_channels = sb_mode_get_channels (_Self->device);
    _Self->mode_bits     = sb_mode_get_bits (_Self->device);
    _Self->mode_signed   = sb_mode_is_signed (_Self->device);

    ps->rate = _Self->mode_lq ? _Self->mode_rate / 2 : _Self->mode_rate;

    // 2. Setup mixer mode

    mixbuf_set_mode(
        &_Self->mixbuf,
        _Self->mode_channels,
        ((1000000L / (uint16_t)(1000000L / ps->rate)) / _Self->sound_buffer_fps) + 1
    );

    // 3. Setup output buffer

    outbuf = &sndDMABuf;

    if (!_player_setup_outbuf (self, outbuf, mixbuf_get_samples_per_channel (&_Self->mixbuf)))
    {
        DEBUG_FAIL("player_play_start", "Failed to setup output buffer.");
        return false;
    }

    frame_size = outbuf->frameSize;
    if (_Self->mode_lq)
        frame_size *= 2;

    _Self->play_isr_param.busy = false;
    _Self->play_isr_param.player = _Self;
    _Self->play_isr_param.dmabuf = &sndDMABuf;
    sb_set_transfer_buffer (_Self->device, outbuf->buf->data, frame_size, outbuf->framesCount, true, &ISR_play, &_Self->play_isr_param);

    // 4. Setup mixer tables

    voltab_calc();

    // 5. Setup playing state

    _player_setup_patterns_order (self, track, ps);
    _player_set_initial_state (self, track, ps);  // master volume affects mixer tables

    // mixer
    amptab_set_volume (ps->master_volume);

    _player_reset_channels (self, channels);
    player_set_pos (self, track, ps, ps->order_start, 0, false);
    ps->flags = 0;    // resume playing

    // 6. Prefill output buffer

    outbuf->frameLast = -1;
    outbuf->frameActive = outbuf->framesCount - 1;
    fill_DMAbuffer (track, ps, channels, _Self->mixer, outbuf);

    // 7. Start sound

    if (!sb_transfer_start (_Self->device))
    {
        ERROR ("player_play_start", "%s", "Failed to start transfer.");
        return false;
    }

    DEBUG_SUCCESS("player_play_start");
    return true;
}

void __far player_play_pause (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self->device)
        sb_transfer_pause (_Self->device);
}

void __far player_play_continue (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self->device)
        sb_transfer_continue (_Self->device);
}

void __far player_play_stop (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self->device)
        sb_transfer_stop (_Self->device);
}

uint16_t __far player_get_buffer_pos (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return sb_get_buffer_pos (_Self->device);
}

uint8_t __far player_get_speed (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = &_Self->play_state;
    return ps->speed;
}

uint8_t __far player_get_tempo (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = &_Self->play_state;
    return ps->tempo;
}

uint8_t __far player_get_pattern_delay (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = &_Self->play_state;
    return ps->patdelay_count;
}

void __far player_free_module (MUSPLAYER *self, MUSMOD *track)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN ("player_free_module");
    if (track)
    {
        if (_Self->track == track)
            _Self->track = NULL;
        _free_module (track);
    }
    DEBUG_END ("player_free_module");
}

void __far player_free_modules (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN ("player_free_modules");

    _free_modules ();

    if (_Self->channels)
    {
        mixchnl_free (_Self->channels);
        _delete (_Self->channels);
    }

    DEBUG_END ("player_free_modules");
}

void __far player_free_device (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN("player_free_device");

    if (_Self->device)
    {
        sb_free (_Self->device);
        sb_delete (&_Self->device);
    }

    DEBUG_END("player_free_device");
}

/* Finalization */

void __far player_free (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN("player_free");

    player_play_stop (self);
    player_free_modules (self);
    player_free_device (self);
    voltab_free();
    snddmabuf_free(&sndDMABuf);

    if (_Self->mixer)
    {
        mixer_free (_Self->mixer);
        _delete (_Self->mixer);
    }
    smpbuf_free (&_Self->smpbuf);
    mixbuf_free (&_Self->mixbuf);

    if (_Self)
    {
        _Self->flags_bufalloc = false;

        if (_Self->flags_use_EM)
            emsFree (_Self->EM_map_handle);
    }

    DEBUG_END("player_free");
}

void __far player_delete (MUSPLAYER **self)
{
    _delete (self);
}

/*** Initialization ***/

void __near s3mplay_init(void)
{
#if DEBUG_WRITE_OUTPUT_STREAM == 1
    DEBUG_open_output_streams ();
#endif  /* DEBUG_WRITE_OUTPUT_STREAM */

    _isr_index = 0;
}

void __near s3mplay_done(void)
{
#if DEBUG_WRITE_OUTPUT_STREAM == 1
    DEBUG_close_output_streams ();
#endif  /* DEBUG_WRITE_OUTPUT_STREAM */
}

DEFINE_REGISTRATION(s3mplay, s3mplay_init, s3mplay_done)
