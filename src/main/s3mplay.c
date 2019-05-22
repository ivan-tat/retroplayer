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

typedef uint8_t _music_player_flags_t;
typedef _music_player_flags_t _MUSPLAYERFLAGS;

#define _MUSPLAYERFL_USE_EM     (1 << 0)
#define _MUSPLAYERFL_DEVICE     (1 << 1)

#pragma pack(push, 1);
typedef struct _music_player_t
{
    char error[_PLAYER_ERROR_LEN];
    _MUSPLAYERFLAGS flags;
    SBDEV *device;
    SNDDMABUF *sndbuf;
    uint8_t sound_buffer_fps;
    bool     mode_set;
    uint8_t  mode_bits;
    bool     mode_signed;
    uint8_t  mode_channels;
    uint16_t mode_rate;
    bool     mode_lq;
    PLAYSTATE *play_state;
    SMPBUF *smpbuf;
    MIXBUF *mixbuf;
    MIXER *mixer;
    EMSHDL EM_map_handle;
    EMSNAME EM_map_name;
    PLAYISRPARAM play_isr_param;
};
#pragma pack(pop);

_MUSPLAYERFLAGS __near __player_set_flags (_MUSPLAYERFLAGS _flags, _MUSPLAYERFLAGS _mask, _MUSPLAYERFLAGS _set, bool _raise)
{
    if (_raise)
        return (_flags & _mask) | _set;
    else
        return _flags & _mask;
}

#define _player_get_flags(o)                (o)->flags
#define _player_set_flags(o, v)             _player_get_flags (o) = v
#define _player_is_EM_in_use(o)             ((_player_get_flags (o) & _MUSPLAYERFL_USE_EM) != 0)
#define _player_set_EM_usage(o, v)          _player_set_flags (o, __player_set_flags (_player_get_flags (o), ~_MUSPLAYERFL_USE_EM, _MUSPLAYERFL_USE_EM, v))
#define _player_is_device_set(o)            ((_player_get_flags (o) & _MUSPLAYERFL_DEVICE) != 0)
#define _player_set_device_set(o, v)        _player_set_flags (o, __player_set_flags (_player_get_flags (o), ~_MUSPLAYERFL_DEVICE, _MUSPLAYERFL_DEVICE, v))

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

        if (_player_is_EM_in_use (player))
        {
            err = true;
            if (emsSaveMap (player->EM_map_handle))
                err = false;
        }

        fill_DMAbuffer (player->play_state, player->mixer, dmabuf);

        if (_player_is_EM_in_use (player) && !err)
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

void __far player_clear (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self)
    {
        memset (_Self, 0, sizeof (_MUSPLAYER));

        /* Device */
        //_Self->device = NULL;
        //_Self->sndbuf = NULL;

        /* Player */
        _Self->sound_buffer_fps = 70;
        //_Self->mode_set = false;
        //_Self->mode_bits = 0;
        //_Self->mode_signed = false;
        //_Self->mode_channels = 0;
        //_Self->mode_rate = 0;
        //_Self->mode_lq = false;

        /* Mixer */
        //_Self->smpbuf = NULL;
        //_Self->mixbuf = NULL;
        //_Self->mixer = NULL;

        /* ISR */
        _Self->EM_map_handle = EMSBADHDL;

        /* Active track */
        //_Self->play_state = NULL;
    }
}

bool __far player_init (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN ("player_init");

    if (_Self)
    {
        player_clear (_Self);

        _player_set_EM_usage (_Self, emsInstalled);

        _Self->sndbuf = _new (SNDDMABUF);
        if (!_Self->sndbuf)
        {
            ERROR ("player_init", "Failed to allocate %s.", "sound buffer object");
            return false;
        }
        snddmabuf_init (_Self->sndbuf);

        if (!_Self->sndbuf->buf)
            if (!snddmabuf_alloc (_Self->sndbuf, DMA_BUF_SIZE_MAX))
            {
                ERROR ("player_init", "Failed to initialize %s.", "DMA buffer");
                return false;
            }

        // Music modules
        _init_modules ();

        // ISR
        if (_player_is_EM_in_use (_Self))
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

        voltab_init ();

        if (!volumetableptr)
        {
            if (!voltab_alloc())
            {
                ERROR ("player_init", "Failed to initialize %s.", "volume table");
                return false;
            }
            voltab_calc ();
        }

        DEBUG_SUCCESS ("player_init");
        return true;
    }
    else
    {
        DEBUG_FAIL ("player_init", "Empty object.");
        return false;
    }
}

void __far player_set_EM_usage (MUSPLAYER *self, bool value)
{
    _MUSPLAYER *_Self = self;

    _player_set_EM_usage (_Self, value && emsInstalled);
}

bool __far player_is_EM_in_use (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _player_is_EM_in_use (_Self);
}

/* Output device */

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
        _player_set_device_set (_Self, true);
        return true;
    }
    else
    {
        ERROR ("player_init_device", "%s", "No sound device.");
        sb_free (_Self->device);
        return false;
    }
}

char *__far player_device_get_name (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_player_is_device_set (_Self))
        return sb_get_name (_Self->device);
    else
        return NULL;
}

void __far player_device_dump_conf (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_player_is_device_set (_Self))
        sb_conf_dump (_Self->device);
}

SNDDMABUF *__far player_get_sound_buffer (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->sndbuf;
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

bool __far player_play_start (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    MUSMOD *track;
    PLAYSTATE *ps;
    MIXCHNLIST *channels;
    SNDDMABUF *outbuf;
    uint16_t frame_size;

    DEBUG_BEGIN("player_play_start");

    if (!_player_is_device_set (_Self))
    {
        ERROR ("player_play_start", "No %s was set.", "sound device");
        return false;
    }

    if (!_Self->mode_set)
    {
        ERROR ("player_play_start", "No %s was set.", "play mode");
        return false;
    }

    if (!_Self->mixer)
    {
        ERROR ("player_play_start", "No %s was set.", "mixer");
        return false;
    }

    ps = _Self->play_state;
    if (!ps)
    {
        ERROR ("player_play_start", "No %s was set.", "active track");
        return false;
    }

    ps->flags = PLAYSTATEFL_END;    // stop for setup

    track = ps->track;
    channels = ps->channels;

    // 1. Setup output mode

    sb_set_transfer_mode (_Self->device, _Self->mode_rate, _Self->mode_channels, _Self->mode_bits, _Self->mode_signed);
    _Self->mode_rate     = sb_mode_get_rate (_Self->device);
    _Self->mode_channels = sb_mode_get_channels (_Self->device);
    _Self->mode_bits     = sb_mode_get_bits (_Self->device);
    _Self->mode_signed   = sb_mode_is_signed (_Self->device);

    ps->rate = _Self->mode_lq ? _Self->mode_rate / 2 : _Self->mode_rate;

    // Setup playing state
    playstate_setup_patterns_order (ps);
    playstate_set_initial_state (ps);   // depends on rate, master volume affects mixer tables
    playstate_set_pos (ps, ps->order_start, 0, false);
    playstate_reset_channels (ps);

    // 2. Setup mixer mode

    mixbuf_set_mode(
        _Self->mixbuf,
        _Self->mode_channels,
        ((1000000L / (uint16_t)(1000000L / ps->rate)) / _Self->sound_buffer_fps) + 1
    );

    // 3. Setup output buffer

    outbuf = _Self->sndbuf;

    if (!_player_setup_outbuf (self, outbuf, mixbuf_get_samples_per_channel (_Self->mixbuf)))
    {
        DEBUG_FAIL("player_play_start", "Failed to setup output buffer.");
        return false;
    }

    frame_size = outbuf->frameSize;
    if (_Self->mode_lq)
        frame_size *= 2;

    _Self->play_isr_param.busy = false;
    _Self->play_isr_param.player = _Self;
    _Self->play_isr_param.dmabuf = _Self->sndbuf;
    sb_set_transfer_buffer (_Self->device, outbuf->buf->data, frame_size, outbuf->framesCount, true, &ISR_play, &_Self->play_isr_param);

    // 4. Setup mixer tables

    amptab_set_volume (ps->master_volume);

    ps->flags = 0;    // resume playing

    // 5. Prefill output buffer

    outbuf->frameLast = -1;
    outbuf->frameActive = outbuf->framesCount - 1;
    fill_DMAbuffer (ps, _Self->mixer, outbuf);

    // 6. Start sound

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

    if (_Self->device)
        return sb_get_buffer_pos (_Self->device);
    else
        return 0;
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

    _player_set_device_set (_Self, false);

    DEBUG_END("player_free_device");
}

/* Mixer */

bool __far player_init_mixer (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    SMPBUF *_smpbuf;
    MIXBUF *_mixbuf;
    uint16_t len;

    DEBUG_BEGIN ("player_init_mixer");

    if (!isCPU_i386 ())
    {
        ERROR ("player_init_mixer", "%s", "CPU is not supported.");
        return false;
    }

    if (!_Self->mixer)
    {
        _Self->mixer = _new (MIXER);
        if (!_Self->mixer)
        {
            ERROR ("player_init_mixer", "Failed to allocate %s.", "sound mixer object");
            return false;
        }
        mixer_init (_Self->mixer);
    }

    if (!_Self->smpbuf)
    {
        _Self->smpbuf = _new (SMPBUF);
        if (!_Self->smpbuf)
        {
            ERROR ("player_init_mixer", "Failed to allocate %s.", "sample buffer object");
            return false;
        }
        smpbuf_init (_Self->smpbuf);
    }
    mixer_set_smpbuf (_Self->mixer, _Self->smpbuf);

    if (!_Self->mixbuf)
    {
        _Self->mixbuf = _new (MIXBUF);
        if (!_Self->mixbuf)
        {
            ERROR ("player_init_mixer", "Failed to allocate %s.", "mixing buffer object");
            return false;
        }
        mixbuf_init (_Self->mixbuf);
    }
    mixer_set_mixbuf (_Self->mixer, _Self->mixbuf);

    // allocate internal buffers

    if (_Self->sndbuf)
    {
        len = _Self->sndbuf->buf->size / 2;  // half DMA transfer buffer size (but in samples)

        _smpbuf = mixer_get_smpbuf (_Self->mixer);
        if (!smpbuf_get (_smpbuf))
            if (!smpbuf_alloc (_smpbuf, len))
            {
                ERROR ("player_init_mixer", "Failed to initialize %s.", "sample buffer");
                return false;
            }

        _mixbuf = mixer_get_mixbuf (_Self->mixer);
        if (!mixbuf_get (_mixbuf))
            if (!mixbuf_alloc (_mixbuf, len))
            {
                ERROR ("player_init_mixer", "Failed to initialize %s.", "mixing buffer");
                return false;
            }
    }
    else
    {
        ERROR ("player_init_mixer", "No %s was set.", "sound buffer");
        return false;
    }

    DEBUG_SUCCESS ("player_init_mixer");
    return true;
}

MIXER *__far player_get_mixer (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->mixer;
}

void __far player_free_mixer (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN ("player_free_mixer");

    if (_Self->mixer)
    {
        mixer_free (_Self->mixer);
        _delete (_Self->mixer);
    }

    if (_Self->smpbuf)
    {
        smpbuf_free (_Self->smpbuf);
        _delete (_Self->smpbuf);
    }

    if (_Self->mixbuf)
    {
        mixbuf_free (_Self->mixbuf);
        _delete (_Self->mixbuf);
    }

    DEBUG_END ("player_free_mixer");
}

/* Song */

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

    track = load_s3m_load (p, name, _player_is_EM_in_use (_Self));
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

    *_track = track;

    DEBUG_SUCCESS("player_load_s3m");
    return true;
}

bool __far player_set_active_track (MUSPLAYER *self, MUSMOD *track)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = _Self->play_state;
    if (!ps)
    {
        ps = _new (PLAYSTATE);
        if (!ps)
        {
            ERROR ("player_set_active_track", "Failed to allocate memory for %s.", "play state object");
            return false;
        }
        playstate_init (ps);
        _Self->play_state = ps;
    }

    ps->track = track;

    if (!playstate_alloc_channels (ps))
    {
        ERROR ("player_set_active_track", "Failed to allocate memory for %s.", "mixing channels");
        return false;
    }

    return true;
}

PLAYSTATE *__far player_get_play_state (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    return _Self->play_state;
}

uint8_t __far player_get_master_volume (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = _Self->play_state;
    if (ps)
        return ps->master_volume;
    else
    {
        ERROR ("player_set_master_volume", "%s", "Active track is not set.");
        return 0;
    }
}

void __far player_set_master_volume (MUSPLAYER *self, uint8_t value)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    ps = _Self->play_state;
    if (ps)
    {
        if (value > MUSMOD_MASTER_VOLUME_MAX)
            value = MUSMOD_MASTER_VOLUME_MAX;
        ps->master_volume = value;
        amptab_set_volume(value);
    }
    else
        ERROR ("player_set_master_volume", "%s", "Active track is not set.");
}

void __near player_free_play_state (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    if (_Self->play_state)
    {
        playstate_free (_Self->play_state);
        _delete (_Self->play_state);
    }
}

void __far player_free_module (MUSPLAYER *self, MUSMOD *track)
{
    _MUSPLAYER *_Self = self;
    PLAYSTATE *ps;

    DEBUG_BEGIN ("player_free_module");
    if (track)
    {
        ps = _Self->play_state;
        if (ps)
            if (ps->track == track)
                player_free_play_state (self);
        _free_module (track);
    }
    DEBUG_END ("player_free_module");
}

void __far player_free_modules (MUSPLAYER *self)
{
    _MUSPLAYER *_Self = self;

    DEBUG_BEGIN ("player_free_modules");

    player_free_play_state (self);
    _free_modules ();

    DEBUG_END ("player_free_modules");
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

    if (_Self)
    {
        if (_Self->sndbuf)
        {
            snddmabuf_free (_Self->sndbuf);
            _delete (_Self->sndbuf);
        }

        player_free_mixer (_Self);

        if (_player_is_EM_in_use (_Self))
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
