/* loads3m.c -- library for loading Scream Tracker 3.x music modules.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$loads3m$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "debug.h"
#include "cc/dos.h"
#include "dos/ems.h"
#include "common.h"
#include "main/pcmsmp.h"
#include "main/musins.h"
#include "main/muspat.h"
#include "main/musmod.h"
#include "main/effects.h"
#include "main/s3mtypes.h"
#include "main/s3mplay.h"
#include "main/loads3m.h"

/* Limits */
#define _S3M_MAX_INSTRUMENTS 99
#define _S3M_MAX_PATTERNS 100
#define _S3M_MAX_CHANNELS 32
#define _S3M_PATTERN_ROWS 64

#define _calc_max_file_pattern_size(rows, channels)     ((rows) * ((channels) * 5 + 1))
#define _calc_max_raw_pattern_size(rows, channels)      ((rows) * (channels) * sizeof (MUSPATCHNEVDATA))
#define _calc_max_packed_pattern_size(rows, channels)   ((rows) * ((channels) * sizeof (MUSPATCHNEVENT) + sizeof (uint16_t)))

#define _S3M_IN_PATTERN_SIZE_MAX    _calc_max_file_pattern_size (_S3M_PATTERN_ROWS, _S3M_MAX_CHANNELS)
#define _S3M_OUT_PATTERN_SIZE_MAX   _calc_max_packed_pattern_size (_S3M_PATTERN_ROWS, _S3M_MAX_CHANNELS)

/* Macros */
#define mk_u16(a, b)        ((((a) & 0xffU) << 0) | (((b) & 0xffU) << 8))
#define mk_u32(a, b, c, d)  ((((a) & 0xffUL) << 0) | (((b) & 0xffUL) << 8) | (((c) & 0xffUL) << 16) | (((d) & 0xffUL) << 24))

/*** File header ***/

#define _S3M_TITLE_LEN 28

/* Format */
#define _S3M_FILE_TYPE      mk_u16 (0x1a, 16)
#define _S3M_FILE_FORMAT_1  1
#define _S3M_FILE_FORMAT_2  2
#define _S3M_FILE_MAGIC     mk_u32 ('S', 'C', 'R', 'M')

/* Header flags */
#define _S3M_FLAG_ST2_VIBRATO   0x01    // not supported
#define _S3M_FLAG_ST2_TEMPO     0x02    // not supported
#define _S3M_FLAG_AMIGA_SLIDES  0x04    // not supported
#define _S3M_FLAG_VOL_0_OPTI    0x08    // not supported
#define _S3M_FLAG_AMIGA_LIMITS  0x10
#define _S3M_FLAG_SB_FILTER     0x20    // not supported
#define _S3M_FLAG_CUSTOM        0x80    // not supported

/* Master volume bits */
#define _S3M_MVOL_MASK      0x7f
#define _S3M_MVOL_SHIFT     0
#define _S3M_MVOL_STEREO    0x80

/* Tracker version bits */
#define _S3M_TRACKER_VER_MINOR_MASK     0x00ff
#define _S3M_TRACKER_VER_MINOR_SHIFT    0
#define _S3M_TRACKER_VER_MAJOR_MASK     0x0f00
#define _S3M_TRACKER_VER_MAJOR_SHIFT    8
#define _S3M_TRACKER_TYPE_MASK          0xf000
#define _S3M_TRACKER_TYPE_SHIFT         12

/* Tracker type */
#define _S3M_TRACKER_ST3        1
#define _S3M_TRACKER_IMPULSE    3
#define _S3M_TRACKER_SCHISM     4

/* Tracker version */
#define _S3M_TRACKER_VER_MAJOR_ST3  3

/* Channel bits */
#define _S3M_CHN_TYPE_MASK      0x1f
#define _S3M_CHN_DISABLED       0x80

/* Channel type */
#define _S3M_CHN_OFF            0
#define _S3M_CHN_LEFT           1
#define _S3M_CHN_RIGHT          2
#define _S3M_CHN_ADLIB_MELODY   3
#define _S3M_CHN_ADLIB_DRUMS    4

#pragma pack(push, 1);
typedef struct _s3m_header_t
{
    char name[_S3M_TITLE_LEN];
    uint16_t type;
    uint8_t unused1[2];
    uint16_t ordnum;
    uint16_t insnum;
    uint16_t patnum;
    uint16_t flags;
    uint16_t tracker;
    uint16_t format;
    uint32_t magic;
    uint8_t gvolume;    // global volume
    uint8_t initialspeed;
    uint8_t initialtempo;
    uint8_t mvolume;    // master volume
    uint8_t unused2[12];
    uint8_t channelset[_S3M_MAX_CHANNELS];
};
#pragma pack(pop);
typedef struct _s3m_header_t _S3M_HEADER;

/*** Packed pattern lead byte flags ***/

typedef uint8_t _s3m_event_flags_t;
typedef _s3m_event_flags_t _S3M_EVENTFLAGS;

#define _S3M_EVENTFL_CHNMASK  0x1f
#define _S3M_EVENTFL_NOTE_INS 0x20
#define _S3M_EVENTFL_VOL      0x40
#define _S3M_EVENTFL_CMD_PARM 0x80

/*** PCM sample ***/

typedef uint8_t _s3m_sample_flags_t;
typedef _s3m_sample_flags_t _S3M_SMPFLAGS;

#define _S3M_SMPFL_LOOP   0x01
#define _S3M_SMPFL_STEREO 0x02
#define _S3M_SMPFL_16BITS 0x04

#pragma pack(push, 1);
typedef struct _s3m_sample_t
{
    uint8_t filepos_hi;
    uint16_t filepos;
    uint32_t length;
    uint32_t loopbeg;
    uint32_t loopend;
    uint8_t note_volume;
    uint8_t unused1;
    uint8_t packinfo;
    _S3M_SMPFLAGS flags;
    uint32_t rate;
    uint8_t unused2[12];
};
#pragma pack(pop);
typedef struct _s3m_sample_t _S3M_SMP;

/*** AdLib instrument ***/

#pragma pack(push, 1);
typedef struct _s3m_adlib_instrument_t
{
    uint8_t unused1[3];
    uint8_t data[12];
    uint8_t note_volume;
    uint8_t unused2[3];
    uint32_t rate;
    uint8_t unused3[12];
};
#pragma pack(pop);
typedef struct _s3m_adlib_instrument_t _S3M_ADL;

/*** Instrument ***/

typedef uint8_t _s3m_instrument_type_t;
typedef _s3m_instrument_type_t _S3M_INSTYPE;

#define _S3M_INST_EMPTY  0x00
#define _S3M_INST_PCM    0x01
#define _S3M_INST_AMEL   0x02
#define _S3M_INST_ABD    0x03
#define _S3M_INST_ASNARE 0x04
#define _S3M_INST_ATOM   0x05
#define _S3M_INST_ACYM   0x06
#define _S3M_INST_AHIHAT 0x07

#define _S3M_INS_FILENAME_LEN 12
#define _S3M_INS_TITLE_LEN 28

#pragma pack(push, 1);
typedef struct _s3m_instrument_t
{
    _S3M_INSTYPE type;
    char dosname[_S3M_INS_FILENAME_LEN];
    union
    {
        _S3M_SMP sample;
        _S3M_ADL adlib;
    } data;
    char title[_S3M_INS_TITLE_LEN];
    uint32_t magic;
};
#pragma pack(pop);
typedef struct _s3m_instrument_t _S3M_INS;

#define _ins_get_type(o)                (o)->type
#define _ins_get_sample_offset(o)       (((o)->data.sample.filepos + ((uint32_t)(o)->data.sample.filepos_hi << 16)) << 4)
#define _ins_get_sample_length(o)       (o)->data.sample.length
#define _ins_is_sample_looped(o)        (((o)->data.sample.flags & _S3M_SMPFL_LOOP) != 0)
#define _ins_is_sample_16_bits(o)       (((o)->data.sample.flags & _S3M_SMPFL_16BITS) != 0)
#define _ins_get_sample_loop_start(o)   (o)->data.sample.loopbeg
#define _ins_get_sample_loop_end(o)     (o)->data.sample.loopend
#define _ins_get_sample_rate(o)         (o)->data.sample.rate
#define _ins_get_sample_note_volume(o)  (o)->data.sample.note_volume
#define _ins_get_file_name(o)           (o)->dosname
#define _ins_get_title(o)               (o)->title

/*** Patterns order entry ***/

typedef uint8_t _s3m_patterns_order_entry_t;
typedef _s3m_patterns_order_entry_t _S3M_PATORDENT;

#define _S3M_PATORDENT_SKIP 0xfe
#define _S3M_PATORDENT_END  0xff

/*** Loader ***/

#pragma pack(push, 1);
typedef struct _s3m_instrument_info_t
{
    _S3M_INS header;
    int8_t smp_num;
};
#pragma pack(pop);
typedef struct _s3m_instrument_info_t _S3M_INSINFO;

#define _S3M_ERROR_LEN 128

typedef uint8_t _s3m_loader_flags_t;
typedef _s3m_loader_flags_t _S3M_LOADERFLAGS;

#define _S3M_LOADERFL_USE_EM        (1 << 0)
#define _S3M_LOADERFL_SIGNED_DATA   (1 << 1)

typedef struct _s3m_loader_t
{
    _S3M_LOADERFLAGS flags;
    char err[_S3M_ERROR_LEN];
    MUSMOD *track;
    FILE *f;
    char *in_buf;
    char *pat_buf;
    uint8_t   ins_count;
    uint8_t   smp_count;
    uint8_t   pat_count;
    uint8_t   ord_length;
    uint16_t *ins_offsets;
    _S3M_INSINFO *ins_info;
    uint16_t *pat_offsets;
    uint16_t pat_EM_pages;
    uint16_t pat_EM_page;
    uint16_t pat_EM_page_offset;
    uint16_t smp_EM_pages;
    uint16_t smp_EM_page;
    uint16_t smp_EM_page_offset;
};
typedef struct _s3m_loader_t _S3M_LOADER;

#define _get_instruments_count(o)       (o)->ins_count
#define _set_instruments_count(o, v)    _get_instruments_count (o) = (v)
#define _get_instruments_offsets(o)     (o)->ins_offsets
#define _set_instruments_offsets(o, v)  _get_instruments_offsets (o) = (v)
#define _get_instrument_offset(o, i)    (_get_instruments_offsets (o)[i] * 16)
#define _get_instruments_info(o)        (o)->ins_info
#define _set_instruments_info(o, v)     _get_instruments_info (o) = (v)
#define _get_instrument_info(o, i)      (_get_instruments_info (o)[i])
#define _get_samples_count(o)           (o)->smp_count
#define _set_samples_count(o, v)        _get_samples_count (o) = (v)
#define _get_patterns_count(o)          (o)->pat_count
#define _set_patterns_count(o, v)       _get_patterns_count (o) = (v)
#define _get_patterns_offsets(o)        (o)->pat_offsets
#define _set_patterns_offsets(o, v)     _get_patterns_offsets (o) = (v)
#define _get_pattern_offset(o, i)       (_get_patterns_offsets (o)[i] * 16)
#define _get_order_length(o)            (o)->ord_length
#define _set_order_length(o, v)         _get_order_length (o) = (v)

/**********************************************************************/

/*** Initialization ***/

LOADER_S3M *load_s3m_new (void)
{
    return _new (_S3M_LOADER);
}

void load_s3m_init (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;

    if (_Self)
        memset (_Self, 0, sizeof (_S3M_LOADER));
}

/*** Errors ***/

void __near load_s3m_clear_error (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;

    memset (_Self->err, 0, _S3M_ERROR_LEN);
}

#if DEBUG == 1

void __near load_s3m_set_error (LOADER_S3M *self, const char *method, int line, const char *format, ...)
{
    va_list ap;
    _S3M_LOADER *_Self = self;

    va_start(ap, format);
    vsnprintf (_Self->err, _S3M_ERROR_LEN, format, ap);
    _DEBUG_LOG (DBGLOG_ERR, __FILE__, line, method, "%s", _Self->err);
}

#define ERROR(o, f, ...) load_s3m_set_error (o, __func__, __LINE__, f, __VA_ARGS__)

#else   /* DEBUG != 1 */

void __near load_s3m_set_error (LOADER_S3M *self, const char *format, ...)
{
    va_list ap;
    _S3M_LOADER *_Self = self;

    va_start(ap, format);
    vsnprintf (_Self->err, _S3M_ERROR_LEN, format, ap);
}

#define ERROR(o, f, ...) load_s3m_set_error (o, f, __VA_ARGS__)

#endif  /* DEBUG != 1 */

const char *load_s3m_get_error (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;

    if (_Self)
    {
        if (_Self->err[0] == 0)
            return NULL;

        return _Self->err;
    }
    else
        return NULL;
}

/*** Header ***/

uint8_t __near getchtyp (uint8_t b)
{
    if (b <= 7)
        return _S3M_CHN_LEFT;

    if (b <= 15)
        return _S3M_CHN_RIGHT;

    if (b <= 23)
        return _S3M_CHN_ADLIB_MELODY;

    if (b <= 31)
        return _S3M_CHN_ADLIB_DRUMS;

    return _S3M_CHN_OFF;
}

bool __near load_s3m_read_header (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    _S3M_HEADER header;
    MUSMOD *track;
    MUSMODCHNPAN pan;
    uint8_t maxused, i, smpnum;

    track = _Self->track;

    if (!fread (&header, sizeof (_S3M_HEADER), 1, _Self->f))
    {
        ERROR (self, "Failed to read %s.", "file's header");
        return false;
    }

    if ((header.type != _S3M_FILE_TYPE)
    || (header.magic != _S3M_FILE_MAGIC)
    || ((header.format != _S3M_FILE_FORMAT_1) && (header.format != _S3M_FILE_FORMAT_2)))
    {
        DEBUG_INFO_ ("type=%04X (%s)",
            header.type,
            (header.type != _S3M_FILE_TYPE)
                ? "wrong" : "ok"
        );
        DEBUG_INFO_ ("magic=%08lX (%s)",
            (long) header.magic,
            (header.magic != _S3M_FILE_MAGIC)
                ? "wrong" : "ok"
        );
        DEBUG_INFO_ ("format=%04X (%s)",
            header.format,
            ((header.format != _S3M_FILE_FORMAT_1) && (header.format != _S3M_FILE_FORMAT_2))
                ? "wrong" : "ok"
        );
        ERROR (self, "%s is not supported.", "file format");
        return false;
    }

    switch ((header.tracker & _S3M_TRACKER_TYPE_MASK) >> _S3M_TRACKER_TYPE_SHIFT)
    {
    case _S3M_TRACKER_ST3:
        if ((header.tracker & _S3M_TRACKER_VER_MAJOR_MASK) != (_S3M_TRACKER_VER_MAJOR_ST3 << _S3M_TRACKER_VER_MAJOR_SHIFT))
        {
            DEBUG_INFO_ ("tracker=%04X (%s)",
                header.tracker,
                ((header.tracker & _S3M_TRACKER_VER_MAJOR_MASK) != (_S3M_TRACKER_VER_MAJOR_ST3 << _S3M_TRACKER_VER_MAJOR_SHIFT))
                    ? "wrong" : "ok"
            );
            ERROR (self, "%s", "Unknown Scream Tracker version.");
            return false;
        }
        snprintf (
            musmod_get_format (track),
            MUSMOD_FORMAT_LEN,
            "Scream Tracker %hhx.%02hhx module",
            (header.tracker & _S3M_TRACKER_VER_MAJOR_MASK) >> _S3M_TRACKER_VER_MAJOR_SHIFT,
            (header.tracker & _S3M_TRACKER_VER_MINOR_MASK) >> _S3M_TRACKER_VER_MINOR_SHIFT
        );
        break;
    case _S3M_TRACKER_IMPULSE:
        if ((header.tracker & (_S3M_TRACKER_VER_MAJOR_MASK | _S3M_TRACKER_VER_MINOR_MASK))
        <=  ((2 << _S3M_TRACKER_VER_MAJOR_SHIFT) | (0x14 << _S3M_TRACKER_VER_MINOR_SHIFT)))
            snprintf (
                musmod_get_format (track),
                MUSMOD_FORMAT_LEN,
                "Impulse Tracker %hhd.%02hhx module",
                (header.tracker & _S3M_TRACKER_VER_MAJOR_MASK) >> _S3M_TRACKER_VER_MAJOR_SHIFT,
                (header.tracker & _S3M_TRACKER_VER_MINOR_MASK) >> _S3M_TRACKER_VER_MINOR_SHIFT
            );
        else
            snprintf (
                musmod_get_format (track),
                MUSMOD_FORMAT_LEN,
                "Impulse Tracker 2.14p%d module",
                (header.tracker & (_S3M_TRACKER_VER_MAJOR_MASK | _S3M_TRACKER_VER_MINOR_MASK)) -
                ((2 << _S3M_TRACKER_VER_MAJOR_SHIFT) | (0x14 << _S3M_TRACKER_VER_MINOR_SHIFT))
            );
        break;
    case _S3M_TRACKER_SCHISM:
        snprintf (
            musmod_get_format (track),
            MUSMOD_FORMAT_LEN,
            "%s",
            "Schism Tracker module"
        );
        break;
    default:
        DEBUG_INFO_ ("tracker=%04X, format=%04X", header.tracker, header.format);
        ERROR (self, "%s", "Unknown tracker.");
        return false;
    }

    maxused = 0;
    for (i = 0; i < _S3M_MAX_CHANNELS; i++)
        if ((header.channelset[i] & _S3M_CHN_DISABLED) == 0)
        {
            switch (getchtyp (header.channelset[i] & _S3M_CHN_TYPE_MASK))
            {
            case _S3M_CHN_OFF:
                pan = MUSMODCHNPAN_CENTER;
                break;
            case _S3M_CHN_LEFT:
                pan = MUSMODCHNPAN_LEFT | MUSMODCHNPANFL_ENABLED;
                maxused = i + 1;
                break;
            case _S3M_CHN_RIGHT:
                pan = MUSMODCHNPAN_RIGHT | MUSMODCHNPANFL_ENABLED;
                maxused = i + 1;
                break;
            case _S3M_CHN_ADLIB_MELODY:
                pan = MUSMODCHNPAN_CENTER;
                break;
            case _S3M_CHN_ADLIB_DRUMS:
                pan = MUSMODCHNPAN_CENTER;
                break;
            default:
                pan = MUSMODCHNPAN_CENTER;
                break;
            }
            musmod_get_channels (track)[i].pan = pan;
        }

    if (!maxused)
    {
        ERROR (self, "%s", "All channels are disabled.");
        return false;
    }

    musmod_set_channels_count (track, maxused);

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("Channels: %hu", musmod_get_channels_count (track));

    header.name[_S3M_TITLE_LEN - 1] = 0;
    musmod_set_title (track, header.name);

    _set_instruments_count (_Self, header.insnum);
    _set_patterns_count (_Self, header.patnum);
    _set_order_length (_Self, header.ordnum);
    musmod_set_stereo (track, (header.mvolume & _S3M_MVOL_STEREO) != 0);
    musmod_set_amiga_limits (track, (header.flags & _S3M_FLAG_AMIGA_LIMITS) != 0);
    musmod_set_global_volume (track, header.gvolume);
    musmod_set_master_volume (track, (header.mvolume & _S3M_MVOL_MASK) >> _S3M_MVOL_SHIFT);
    musmod_set_tempo (track, header.initialtempo);
    musmod_set_speed (track, header.initialspeed);
    if (header.format == _S3M_FILE_FORMAT_1)
        _Self->flags |= _S3M_LOADERFL_SIGNED_DATA;

    return true;
}

/*** Samples ***/

// minus 1 to fit in 16 bits
#define SAMPLE_SIZE_MAX (0x10000 - 1)

uint32_t __near _calc_sample_load_size (PCMSMP *smp)
{
    uint32_t value;

    if (pcmsmp_get_loop (smp) != PCMSMPLOOP_NONE)
        value = pcmsmp_get_loop_end (smp);
    else
        value = pcmsmp_get_length (smp);

    if (pcmsmp_get_bits (smp) == 16)
        value <<= 1;

    return value;
}

uint32_t __near _calc_sample_mem_size (PCMSMP *smp, uint32_t size)
{
    return size + PCMSMP_EXTRA_DATA_SIZE;
}

void __near convert_sign_8 (void *data, uint32_t size)
{
    char *p;
    uint32_t count;

    p = data;
    count = size;
    while (count)
    {
        *p ^= 0x80;
        p++;
        count--;
    }
}

void __near load_s3m_alloc_EM_samples_pages (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    PCMSMPLIST *samples;
    MUSINSLIST *instruments;
    uint16_t pages, free_pages;
    uint32_t data_size;
    int16_t i;
    MUSINS *ins;
    PCMSMP *smp;
    EMSHDL handle;

    track = _Self->track;
    samples = musmod_get_samples (track);
    instruments = musmod_get_instruments (track);

    pages = 0;
    for (i = 0; i < _get_instruments_count (_Self); i++)
    {
        ins = musinsl_get (instruments, i);
        if (musins_get_type (ins) == MUSINST_PCM)
        {
            smp = musins_get_sample (ins);
            if (emsEC != E_EMS_SUCCESS)
            {
                ERROR (self, "Failed to allocate EM for %s.", "samples");
                pcmsmpl_set_EM_data (samples, false);
                return;
            }
            data_size = _calc_sample_mem_size (smp, _calc_sample_load_size (smp));
            pages += (uint32_t) (data_size + EM_PAGE_SIZE - 1) / EM_PAGE_SIZE;
        }
    }

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("Samples to load: %u (EM pages: %u)", _get_samples_count (_Self), pages);

    free_pages = emsGetFreePagesCount ();
    if (!free_pages)
    {
        DEBUG_WARN_ ("Not enough EM for %s.", "samples");
        pcmsmpl_set_EM_data (samples, false);
        return;
    }

    if (pages > free_pages)
        pages = free_pages;

    handle = emsAlloc (pages);
    if (emsEC != E_EMS_SUCCESS)
    {
        ERROR (self, "Failed to allocate EM for %s.", "samples");
        pcmsmpl_set_EM_data (samples, false);
        return;
    }
    pcmsmpl_set_EM_data (samples, true);
    pcmsmpl_set_EM_handle (samples, handle);
    pcmsmpl_set_EM_handle_name (samples, "smplist");

    _Self->smp_EM_pages = pages;
    _Self->smp_EM_page = 0;
    _Self->smp_EM_page_offset = 0;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("EM pages allocated: %u.", pages);
}

bool __near load_s3m_allocate_EM_for_sample (LOADER_S3M *self, PCMSMP *smp, uint8_t index, uint32_t size, uint16_t offset, char **data)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    PCMSMPLIST *samples;
    uint16_t start_page, start_offset, end_page, end_offset;
    uint32_t s;

    if (size > SAMPLE_SIZE_MAX - PCMSMP_EXTRA_DATA_SIZE)
    {
        ERROR (self, "%s", "Sample is too large.");
        return false;
    }

    track = _Self->track;
    samples = musmod_get_samples (track);

    start_page = _Self->smp_EM_page + (offset / EM_PAGE_SIZE);
    start_offset = offset % EM_PAGE_SIZE;
    s = size + offset - 1;  // last byte
    end_page = start_page + (s / EM_PAGE_SIZE);
    end_offset = s % EM_PAGE_SIZE;

    if (end_page < _Self->smp_EM_pages)
    {
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("sample=%02hhu, size=0x%08lX, handle=0x%04hX, pages=0x%04hX:0x%04hX-0x%04hX:0x%04hX",
                (uint8_t) index,
                (uint32_t) size,
                (uint16_t) pcmsmpl_get_EM_handle (samples),
                (uint16_t) start_page,
                (uint16_t) start_offset,
                (uint16_t) end_page,
                (uint16_t) end_offset
            );
        pcmsmp_set_EM_data (smp, true);
        pcmsmp_set_own_EM_handle (smp, false);
        pcmsmp_set_EM_data_handle (smp, pcmsmpl_get_EM_handle (samples));
        pcmsmp_set_EM_data_page (smp, start_page);
        pcmsmp_set_EM_data_offset (smp, start_offset);
        *data = pcmsmp_map_EM_data (smp);
        if (!*data)
        {
            ERROR (self, "Failed to map EM for %s.", "sample");
            return false;
        }
        // next EM position
        end_offset++;
        end_page += end_offset / EM_PAGE_SIZE;
        end_offset %= EM_PAGE_SIZE;
        _Self->smp_EM_page = end_page;
        _Self->smp_EM_page_offset = end_offset;
    }

    return true;
}

bool __near load_s3m_load_sample (LOADER_S3M *self, uint8_t index)
{
    _S3M_LOADER *_Self = self;
    _S3M_INSINFO *info;
    MUSMOD *track;
    PCMSMPLIST *samples;
    MUSINSLIST *instruments;
    MUSINS *ins;
    PCMSMP *smp;
    char *data;
    uint16_t pages, h, dh;
    uint8_t i;
    uint32_t load_size, data_size;
    uint16_t start_offset;
    uint32_t end_offset;
    uint8_t end_page;
    void *loopstart;
    uint32_t loopsize;

    info = & (_get_instrument_info (_Self, index));
    if (fsetpos (_Self->f, _ins_get_sample_offset (& (info->header))))
    {
        ERROR (self, "Failed to read %s.", "file");
        return false;
    }

    track = _Self->track;
    samples = musmod_get_samples (track);
    instruments = musmod_get_instruments (track);
    ins = musinsl_get (instruments, index);
    smp = pcmsmpl_get (samples, info->smp_num);
    load_size = _calc_sample_load_size (smp);
    data_size = _calc_sample_mem_size (smp, load_size);
    if (data_size > SAMPLE_SIZE_MAX)
    {
        ERROR (self, "%s", "Sample is too large.");
        return false;
    }

    data = NULL;

    if ((_Self->flags & _S3M_LOADERFL_USE_EM) && pcmsmpl_is_EM_data (samples))
    {
        start_offset = _Self->smp_EM_page_offset;
        if (start_offset)
        {
            /* try to use current offset */
            end_offset = start_offset + data_size - 1;  // end offset points to last byte
            if (end_offset > 0xffff)
                start_offset = EM_PAGE_SIZE;    // use start of next page
        }
        if (!load_s3m_allocate_EM_for_sample (self, smp, index, data_size, start_offset, &data))
            return false;
    }

    if (!data)
    {
        data = __new (data_size);
        if (!data)
        {
            ERROR (self, "Failed to allocate memory for %s.", "sample data");
            return false;
        }
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("sample=%02u, data=DOS, address=0x%04X:0x%04X.",
                index,
                FP_SEG (data),
                FP_OFF (data)
            );
        pcmsmp_set_EM_data (smp, false);
        pcmsmp_set_data (smp, data);
    }

    if (!fread (data, load_size, 1, _Self->f))
    {
        ERROR (self, "Failed to read %s.", "sample data");
        return false;
    }

    if (!(_Self->flags & _S3M_LOADERFL_SIGNED_DATA))
        convert_sign_8 (data, load_size);

    switch (pcmsmp_get_loop (smp))
    {
    case PCMSMPLOOP_NONE:
        memset (& (data [load_size]), 0, data_size - load_size);
        break;
    case PCMSMPLOOP_FORWARD:
        h = data_size - load_size;
        loopstart = & (data [pcmsmp_get_loop_start (smp)]);
        loopsize = pcmsmp_get_loop_end (smp) - pcmsmp_get_loop_start (smp);
        while (h)
        {
            if (h > loopsize)
                dh = loopsize;
            else
                dh = h;
            memcpy (& (data [data_size - h]), loopstart, dh);
            h -= dh;
        }
        break;
    /* TODO: implement all loop types */
    case PCMSMPLOOP_BACKWARD:
    case PCMSMPLOOP_PINGPONG:
    default:
        break;
    }

    pcmsmp_set_available (smp, true);

    return true;
}

bool __near load_s3m_load_samples (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    int count, i;

    count = _get_instruments_count (_Self);
    for (i = 0; i < count; i++)
        if (_get_instrument_info (_Self, i).smp_num >= 0)
            if (!load_s3m_load_sample (_Self, i))
                return false;

    return true;
}

void __near load_s3m_free_unused_samples_pages (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    PCMSMPLIST *samples;
    uint16_t pages, pages_free;
    EMSHDL handle;

    track = _Self->track;
    samples = musmod_get_samples (track);
    if (pcmsmpl_is_EM_data (samples))
    {
        pages = _Self->smp_EM_page;
        if (_Self->smp_EM_page_offset)
            pages++;
        pages_free = _Self->smp_EM_pages - pages;

        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("EM pages: %u used, %u free", pages, pages_free);

        // try to free unused pages
        if (pages_free)
        {
            handle = pcmsmpl_get_EM_handle (samples);
            if (pages)
            {
                if (!emsResize (handle, pages))
                    DEBUG_WARN_ ("Failed to free unused EM pages for %s.", "samples");
            }
            else
            {
                if (emsFree (handle))
                {
                    pcmsmpl_set_EM_data (samples, false);
                    pcmsmpl_set_EM_handle (samples, EMSBADHDL);
                }
                else
                    DEBUG_WARN_ ("Failed to free unused EM pages for %s.", "samples");
            }
        }
    }
}

/*** Instruments ***/

bool __near load_s3m_load_ins_offsets (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    uint16_t size;

    size = _get_instruments_count (_Self) * sizeof (uint16_t);

    _set_instruments_offsets (_Self, __new (size));
    if (!_get_instruments_offsets (_Self))
    {
        ERROR (self, "Failed to allocate memory for %s.", "instruments offsets");
        return false;
    }

    if (!fread (_get_instruments_offsets (_Self), size, 1, _Self->f))
    {
        ERROR (self, "Failed to read %s.", "instruments offsets");
        return false;
    }

    return true;
}

bool __near load_s3m_load_ins_headers (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    _S3M_INSINFO *info;
    uint8_t i;

    _set_instruments_info (_Self, __new (sizeof (_S3M_INSINFO) * _get_instruments_count (_Self)));
    if (!_get_instruments_info (_Self))
    {
        ERROR (self, "Failed to allocate memory for %s.", "instruments headers");
        return false;
    }

    _set_samples_count (_Self, 0);
    for (i = 0; i < _get_instruments_count (_Self); i++)
    {
        if (fsetpos (_Self->f, _get_instrument_offset (_Self, i)))
        {
            ERROR (self, "Failed to read %s.", "file");
            return false;
        }

        info = & (_get_instrument_info (_Self, i));
        if (!fread (& (info->header), sizeof (_S3M_INS), 1, _Self->f))
        {
            ERROR (self, "Failed to read %s.", "instrument header");
            return false;
        }

        switch (_ins_get_type (& (info->header)))
        {
        case _S3M_INST_EMPTY:
            info->smp_num = -1;
            break;
        case _S3M_INST_PCM:
            if (info->header.data.sample.packinfo)
            {
                ERROR (self, "%s is not supported.", "Packed sample");
                return false;
            }
            if (info->header.data.sample.flags & _S3M_SMPFL_STEREO)
            {
                ERROR (self, "%s is not supported.", "Stereo sample");
                return false;
            }
            info->smp_num = _get_samples_count (_Self);
            _get_samples_count (_Self)++;
            break;
        case _S3M_INST_AMEL:
        case _S3M_INST_ABD:
        case _S3M_INST_ASNARE:
        case _S3M_INST_ATOM:
        case _S3M_INST_ACYM:
        case _S3M_INST_AHIHAT:
            ERROR (self, "%s is not supported.", "Adlib instrument");
            return false;
        default:
            ERROR (self, "%s", "Unknown instrument type.");
            return false;
        }

        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("index=%hd, sample=%hd", i, info->smp_num);
    }

    return true;
}

bool __near load_s3m_load_instrument (LOADER_S3M *self, uint8_t index)
{
    _S3M_LOADER *_Self = self;
    _S3M_INSINFO *info;
    _S3M_INS *_ins;
    MUSMOD *track;
    PCMSMPLIST *samples;
    MUSINSLIST *instruments;
    MUSINS *ins;
    PCMSMP *smp;
    uint32_t size;
    char smp_title[_S3M_INS_FILENAME_LEN + 1];  /* including terminating zero */
    char ins_title[MUSINS_TITLE_LEN + 1];       /* including terminating zero */

    info = & (_get_instrument_info (_Self, index));
    _ins = & (info->header);

    track = _Self->track;
    samples = musmod_get_samples (track);
    instruments = musmod_get_instruments (track);
    ins = musinsl_get (instruments, index);
    musins_init (ins);

    if (_ins_get_type (_ins) == _S3M_INST_PCM)
    {
        smp = pcmsmpl_get (samples, info->smp_num);
        pcmsmp_set_available (smp, true);

        size = _ins_get_sample_length (_ins);
        if (_ins_is_sample_16_bits (_ins))
        {
            pcmsmp_set_bits (smp, 16);
            size <<= 1;
        }
        else
            pcmsmp_set_bits (smp, 8);
        pcmsmp_set_size (smp, size);

        if (_ins_is_sample_looped (_ins))
        {
            pcmsmp_set_loop (smp, PCMSMPLOOP_FORWARD);
            pcmsmp_set_loop_start (smp, _ins_get_sample_loop_start (_ins));
            pcmsmp_set_loop_end (smp, _ins_get_sample_loop_end (_ins));
        }
        else
            pcmsmp_set_loop (smp, PCMSMPLOOP_NONE);

        pcmsmp_set_rate (smp, _ins_get_sample_rate (_ins));
        pcmsmp_set_volume (smp, PCMSMP_VOLUME_MAX);

        memcpy (smp_title, _ins_get_file_name (_ins), _S3M_INS_FILENAME_LEN);
        smp_title[_S3M_INS_FILENAME_LEN] = 0;
        pcmsmp_set_title (smp, smp_title);

        musins_set_type (ins, MUSINST_PCM);
        musins_set_volume (ins, MUSINS_VOLUME_MAX);
        musins_set_note_volume (ins, _ins_get_sample_note_volume (_ins));
        musins_set_sample (ins, smp);
    }
    else
    {
        musins_set_type (ins, MUSINST_EMPTY);
    }

    memcpy (ins_title, _ins_get_title (_ins), _S3M_INS_TITLE_LEN);
    ins_title[_S3M_INS_TITLE_LEN] = 0;
    musins_set_title (ins, ins_title);
    musins_get_title (ins)[MUSINS_TITLE_LEN - 1] = 0;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_dump_instrument_info (ins, index, samples);

    return true;
}

bool __near load_s3m_load_instruments (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    int count, i;

    count = _get_instruments_count (_Self);
    for (i = 0; i < count; i++)
        if (!load_s3m_load_instrument (_Self, i))
            return false;

    return true;
}

/*** Patterns ***/

bool __near load_s3m_load_pat_offsets (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    uint16_t size;

    size = sizeof (uint16_t) * _get_patterns_count (_Self);

    _set_patterns_offsets (_Self, __new (size));
    if (!_get_patterns_offsets (_Self))
    {
        ERROR (self, "Failed to allocate memory for %s.", "pattern offsets");
        return false;
    }

    if (!fread (_get_patterns_offsets (_Self), size, 1, _Self->f))
    {
        ERROR (self, "Failed to read %s.", "patterns offsets");
        return false;
    }

    return true;
}

void __near load_s3m_alloc_EM_patterns_pages (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSPATLIST *patterns;
    uint16_t num_patterns;
    uint16_t patsize;
    uint16_t patperpage;
    uint16_t patpages;
    uint16_t freepages;
    EMSHDL handle;

    track = _Self->track;
    patterns = musmod_get_patterns (track);
    num_patterns = muspatl_get_count (patterns);
    patsize = _calc_max_raw_pattern_size (_S3M_PATTERN_ROWS, musmod_get_channels_count (track));
    patperpage = EM_PAGE_SIZE / patsize;
    patpages = (num_patterns + patperpage - 1) / patperpage;
    freepages = emsGetFreePagesCount ();

    if (patpages <= freepages)
        _Self->pat_EM_pages = patpages;
    else
        _Self->pat_EM_pages = freepages;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("Memory info:" CRLF
            "All patterns size:       %5lu KiB (%u patterns, each %u bytes size)" CRLF
            "All patterns size in EM: %5lu KiB (%u patterns, %u pages, %u per page)" CRLF
            "Free EM:                 %5lu KiB (%u patterns, %u pages)" CRLF
            "Requested EM:            %5lu KiB (%u patterns, %u pages)",
            ((uint32_t) patsize * num_patterns + 1024) / 1024, num_patterns, patsize,
            (uint32_t) patpages * EM_PAGE_SIZE / 1024, patpages * patperpage, patpages, patperpage,
            (uint32_t) freepages * EM_PAGE_SIZE / 1024, freepages * patperpage, freepages,
            (uint32_t) _Self->pat_EM_pages * EM_PAGE_SIZE / 1024, _Self->pat_EM_pages * patperpage, _Self->pat_EM_pages
        );

    handle = emsAlloc (_Self->pat_EM_pages);
    if (emsEC != E_EMS_SUCCESS)
    {
        ERROR (self, "Failed to allocate EM for %s.", "patterns");
        muspatl_set_EM_data (patterns, false);
        return;
    }

    muspatl_set_EM_data (patterns, true);
    muspatl_set_own_EM_handle (patterns, true);
    muspatl_set_EM_handle (patterns, handle);
    muspatl_set_EM_handle_name (patterns, "patlist");

    _Self->pat_EM_page_offset = 0;
    _Self->pat_EM_page = 0;
}

/* Packed pattern event */

typedef struct _s3m_pattern_channel_event_data_t
{
    unsigned char instrument;
    unsigned char note;
    unsigned char note_volume;
    unsigned char command;
    unsigned char parameter;
};
typedef struct _s3m_pattern_channel_event_data_t _S3M_PATCHNEVDATA;

typedef unsigned char _s3m_pattern_channel_event_flags_t;
typedef _s3m_pattern_channel_event_flags_t _S3M_PATCHNEVFLAGS;

#define _S3M_PATEVFL_INS    (1 << 0)
#define _S3M_PATEVFL_NOTE   (1 << 1)
#define _S3M_PATEVFL_VOL    (1 << 2)
#define _S3M_PATEVFL_CMD    (1 << 3)
#define _S3M_PATEVFL_ROWEND (1 << 4)

typedef struct _s3m_pattern_channel_event_t
{
    _S3M_PATCHNEVFLAGS flags;
    _S3M_PATCHNEVDATA data;
};
typedef struct _s3m_pattern_channel_event_t _S3M_PATCHNEVENT;

typedef struct _s3m_pattern_row_event_t
{
    unsigned char channel;
    _S3M_PATCHNEVENT event;
};
typedef struct _s3m_pattern_row_event_t _S3M_PATROWEVENT;

/* Packed pattern I/O */

typedef struct _s3m_pattern_io_t
{
    unsigned char *data;
    unsigned int size;
    unsigned int offset;
    char *error;
};
typedef struct _s3m_pattern_io_t _S3M_PATIO;

void __near _s3m_patio_open (_S3M_PATIO *self, char *data, unsigned int size)
{
    self->data = data;
    self->size = size;
    self->offset = 0;
    self->error = NULL;
}

bool __near _s3m_patio_read (_S3M_PATIO *self, char *buf, unsigned int size)
{
    if (self->offset + size - 1 < self->size)
    {
        memcpy (buf, self->data + self->offset, size);
        self->offset += size;
        return true;
    }
    else
    {
        self->error = "Out of data";
        return false;
    }
}

bool __near _s3m_patio_eof (_S3M_PATIO *self)
{
    return self->size <= self->offset;
}

bool __near _s3m_patio_read_event (_S3M_PATIO *self, _S3M_PATROWEVENT *event)
{
    unsigned char flags, v[2];

    if (!_s3m_patio_read (self, &flags, 1))
        return false;

    event->event.data.instrument  = CHN_INS_NONE;
    event->event.data.note        = CHN_NOTE_NONE;
    event->event.data.note_volume = CHN_NOTEVOL_NONE;
    event->event.data.command     = CHN_CMD_NONE;
    event->event.data.parameter   = 0;

    if (flags)
    {
        event->channel = flags & _S3M_EVENTFL_CHNMASK;
        event->event.flags = 0;

        if (flags & _S3M_EVENTFL_NOTE_INS)
        {
            // note, instrument
            if (!_s3m_patio_read (self, v, 2))
                return false;

            switch (v[0])
            {
            case 0xff:
                break;
            case 0xfe:
                event->event.data.note = CHN_NOTE_OFF;
                event->event.flags |= _S3M_PATEVFL_NOTE;
                break;
            default:
                if (((v[0] & 0x0f) <= 11) && ((v[0] >> 4) <= 7))
                {
                    event->event.data.note = v[0];
                    event->event.flags |= _S3M_PATEVFL_NOTE;
                }
                break;
            }

            switch (v[1])
            {
            case 0:
                break;
            default:
                if (v[1] <= 99)
                {
                    event->event.data.instrument = v[1];
                    event->event.flags |= _S3M_PATEVFL_INS;
                }
                break;
            }
        }

        if (flags & _S3M_EVENTFL_VOL)
        {
            // note volume
            if (!_s3m_patio_read (self, v, 1))
                return false;

            switch (v[0])
            {
            case 0xff:
                break;
            default:
                if (v[0] <= 64)
                {
                    event->event.data.note_volume = v[0];
                    event->event.flags |= _S3M_PATEVFL_VOL;
                }
                break;
            }
        }

        if (flags & _S3M_EVENTFL_CMD_PARM)
        {
            // command, parameter
            if (!_s3m_patio_read (self, v, 2))
                return false;

            switch (v[0])
            {
            case 0:
                break;
            default:
                if (v[0] <= 26)
                {
                    event->event.data.command = v[0];
                    event->event.data.parameter = v[1];
                    event->event.flags |= _S3M_PATEVFL_CMD;
                }
                break;
            }
        }
        return true;
    }
    else
    {
        event->channel = 0;
        event->event.flags = _S3M_PATEVFL_ROWEND;
        return true;
    }
}

void __near _s3m_patio_close (_S3M_PATIO *self)
{
}

void __near _clear_events (MUSPATCHNEVENT *events)
{
    unsigned char i;

    for (i = 0; i < _S3M_MAX_CHANNELS; i++)
    {
        muspatchnevent_clear (events);
        events++;
    }
}

bool __near load_s3m_convert_pattern (LOADER_S3M *self, uint8_t *src, uint16_t src_len, MUSPAT *pattern, bool pack)
{
    _S3M_LOADER *_Self = self;
    _S3M_PATIO fi;
    _S3M_PATROWEVENT ei;
    MUSPATIO f;
    MUSPATROWEVENT e;
    MUSPATCHNEVENT events[_S3M_MAX_CHANNELS];
    uint16_t maxrow, r;
    uint8_t maxchn, c;
    uint16_t offset;
    #if DEBUG_FILE_S3M_LOAD == 1
    char s[13], *e_str;
    #endif  /* DEBUG_FILE_S3M_LOAD == 1 */

    muspat_set_data_packed (pattern, pack);

    if (!muspatio_open (&f, pattern, MUSPATIOMD_WRITE))
    {
        ERROR (self, "Failed to open pattern for writing (%s).", f.error);
        return false;
    }

    src = _Self->in_buf;

    _s3m_patio_open (&fi, src, src_len);

    memset (muspat_get_data (pattern), 0xaa, muspat_get_size (pattern));

    maxrow = muspat_get_rows (pattern);
    maxchn = muspat_get_channels (pattern);

    if (!muspat_is_data_packed (pattern))
    {
        /* clear pattern */

        muspatrowevent_clear (&e);
        for (r = 0; r < maxrow; r++)
        {
            muspatio_seek (&f, r, 0);
            for (c = 0; c < maxchn; c++)
            {
                e.channel = c;
                muspatio_write (&f, &e);
            }
        }
        muspatio_seek (&f, 0, 0);
    }

    _clear_events (events);

    r = 0;
    while (r < maxrow)
    {
        offset = fi.offset;
        if (_s3m_patio_read_event (&fi, &ei))
        {
            if (ei.event.flags & _S3M_PATEVFL_ROWEND)
            {
                if (DEBUG_FILE_S3M_LOAD)
                    _DEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL,
                        "row=%02hhu, offset=0x%04X, size=%hhu, type=end",
                        r,
                        offset,
                        fi.offset - offset
                    );
                for (c = 0; c < maxchn; c++)
                    if (events [c].flags)
                    {
                        e.channel = c;
                        memcpy (& (e.event), & (events [c]), sizeof (MUSPATCHNEVENT));
                        muspatio_write (&f, &e);
                    }
                muspatio_end_row (&f);
                r++;

                if (r < maxrow)
                    _clear_events (events);
            }
            else
            {
                muspatrowevent_clear (&e);
                e.channel = ei.channel;

                if (ei.event.flags)
                {
                    if (ei.event.flags & _S3M_PATEVFL_INS)
                    {
                        e.event.data.instrument = ei.event.data.instrument;
                        e.event.flags |= MUSPATCHNEVFL_INS;
                    }

                    if (ei.event.flags & _S3M_PATEVFL_NOTE)
                    {
                        e.event.data.note = ei.event.data.note;
                        e.event.flags |= MUSPATCHNEVFL_NOTE;
                    }

                    if (ei.event.flags & _S3M_PATEVFL_VOL)
                    {
                        e.event.data.note_volume = ei.event.data.note_volume;
                        e.event.flags |= MUSPATCHNEVFL_VOL;
                    }

                    if (ei.event.flags & _S3M_PATEVFL_CMD)
                    {
                        e.event.data.command = ei.event.data.command;
                        e.event.data.parameter = ei.event.data.parameter;
                        e.event.flags |= MUSPATCHNEVFL_CMD;
                    }

                    #if DEBUG_FILE_S3M_LOAD == 1
                    if (e.event.flags)
                    {
                        DEBUG_get_pattern_channel_event_str (s, & (e.event));
                        e_str = s;
                    }
                    else
                        e_str = "empty";
                    #endif  /* DEBUG_FILE_S3M_LOAD == 1 */

                    memcpy (& (events [e.channel]), & (e.event), sizeof (MUSPATCHNEVENT));
                }
                else
                {
                    #if DEBUG_FILE_S3M_LOAD == 1
                    e_str = "empty";
                    #endif  /* DEBUG_FILE_S3M_LOAD == 1 */
                }

                #if DEBUG_FILE_S3M_LOAD == 1
                _DEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL,
                    "row=%02hhu, offset=0x%04X, size=%hhu, type=event <%02hhu:%s>",
                    r,
                    offset,
                    fi.offset - offset,
                    e.channel,
                    e_str
                );
                #endif  /* DEBUG_FILE_S3M_LOAD == 1 */
            }
        }
        else
        {
            ERROR (self, "Failed to read pattern (%s).", fi.error);
            muspatio_close (&f);
            _s3m_patio_close (&fi);
            return false;
        }
    }

    muspatio_close (&f);
    _s3m_patio_close (&fi);

    return true;
}

bool __near load_s3m_load_pattern (LOADER_S3M *self, uint8_t index)
{
    #define _BUF_SIZE (_S3M_MAX_CHANNELS * 13)
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSPATLIST *patterns;
    uint32_t pos;
    uint16_t in_size, mem_size, em_size;
    MUSPAT pat_static, *pat;
    void *data;
    bool em;
    char s[_BUF_SIZE];

    pos = _get_pattern_offset (_Self, index);

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_MSG_ ("pattern_index=%u, file_offset=0x%08lX", index, (uint32_t) pos);

    if (!pos)
    {
        /*
        pat = muspatl_get (patterns, index);
        muspat_init (pat);  // is not necessary because it was already initialized by DYNARR's set_size().
        */
        return true;
    }

    if (fsetpos (_Self->f, pos))
    {
        ERROR (self, "Failed to read %s.", "file");
        return false;
    }

    if (!fread (&in_size, 2, 1, _Self->f))
    {
        ERROR (self, "Failed to read %s.", "pattern");
        return false;
    }

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_MSG_ ("file_size=0x%04X", in_size);

    if ((in_size <= 2) || (in_size > _S3M_IN_PATTERN_SIZE_MAX))
    {
        ERROR (self, "%s", "Bad pattern size.");
        return false;
    }

    in_size -= 2;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_MSG_ ("load_size=0x%04X", in_size);

    if (!fread (_Self->in_buf, in_size, 1, _Self->f))
    {
        ERROR (self, "Failed to read %s.", "pattern");
        return false;
    }

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_dump_mem (_Self->in_buf, in_size, "file: ");

    // target is pat_buf
    track = _Self->track;
    patterns = musmod_get_patterns (track);
    pat = &pat_static;
    muspat_init (pat);
    muspat_set_EM_data (pat, false);
    muspat_set_channels (pat, musmod_get_channels_count (track));
    muspat_set_rows (pat, _S3M_PATTERN_ROWS);
    muspat_set_size (pat, _S3M_OUT_PATTERN_SIZE_MAX);
    muspat_set_data (pat, _Self->pat_buf);

    if (!load_s3m_convert_pattern (self, _Self->in_buf, in_size, pat, true))
        return false;

    mem_size = muspat_get_packed_size (pat);    // get final size
    muspat_set_size (pat, mem_size);            // reduce (in most cases) memory usage

    // try to put in EM
    #define fit_in_EM_page(offset, size)    (offset + size <= EM_PAGE_SIZE)
    em = false;
    if (muspatl_is_EM_data (patterns) && _Self->pat_EM_pages)
    {
        // calculate em_size
        em = true;
        em_size = (mem_size + 15) & ~15;    // round size to 16 bytes
        if (!fit_in_EM_page (_Self->pat_EM_page_offset, em_size))
        {
            // EM page overflow, try next page
            _Self->pat_EM_page++;
            _Self->pat_EM_page_offset = 0;
            _Self->pat_EM_pages--;
            em = _Self->pat_EM_pages && fit_in_EM_page (_Self->pat_EM_page_offset, em_size);
        }
    }
    #undef fit_in_EM_page

    if (em)
    {
        // use em_size
        muspat_set_EM_data (pat, true);
        muspat_set_own_EM_handle (pat, false);
        muspat_set_EM_data_handle (pat, muspatl_get_EM_handle (patterns));
        muspat_set_EM_data_page (pat, _Self->pat_EM_page);
        muspat_set_EM_data_offset (pat, _Self->pat_EM_page_offset);
        data = muspat_map_EM_data (pat);
        if (!data)
        {
            ERROR (self, "Failed to map EM for %s.", "pattern");
            return false;
        }
        _Self->pat_EM_page_offset += em_size;   // calculate next available offset
    }
    else
    {
        data = __new (mem_size);
        if (!data)
        {
            ERROR (self, "Failed to allocate memory for %s.", "pattern");
            return false;
        }
        muspat_set_EM_data (pat, false);
        muspat_set_data (pat, data);
    }

    memcpy (data, _Self->pat_buf, mem_size);    // save data

    muspatl_set (patterns, index, pat);     // save final pattern
    pat = muspatl_get (patterns, index);    // update pointer

    if (DEBUG_FILE_S3M_LOAD)
    {
        DEBUG_dump_pattern_info (pat, index);
        DEBUG_dump_pattern (pat, s, musmod_get_channels_count (track));
    }

    return true;
    #undef _BUF_SIZE
}

bool __near load_s3m_load_patterns (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSPATLIST *patterns;
    int count, i;

    track = _Self->track;
    patterns = musmod_get_patterns (track);
    count = muspatl_get_count (patterns);
    for (i = 0; i < count; i++)
        if (!load_s3m_load_pattern (_Self, i))
            return false;

    return true;
}

void __near load_s3m_free_unused_patterns_pages (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSPATLIST *patterns;
    uint16_t pages, pages_free;
    EMSHDL handle;

    track = _Self->track;
    patterns = musmod_get_patterns (track);
    if (muspatl_is_EM_data (patterns))
    {
        pages = _Self->pat_EM_page;
        if (_Self->pat_EM_page_offset)
            pages++;
        pages_free = _Self->pat_EM_pages - pages;

        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("EM pages: %u used, %u free", pages, pages_free);

        // try to free unused pages
        if (pages_free)
        {
            handle = muspatl_get_EM_handle (patterns);
            if (pages)
            {
                if (!emsResize (handle, pages))
                    DEBUG_WARN_ ("Failed to free unused EM pages for %s.", "patterns");
            }
            else
            {
                if (emsFree (handle))
                {
                    muspatl_set_EM_data (patterns, false);
                    muspatl_set_EM_handle (patterns, EMSBADHDL);
                }
                else
                    DEBUG_WARN_ ("Failed to free unused EM pages for %s.", "patterns");
            }
        }
    }
}

/*** Patterns order ***/

bool __near load_s3m_load_patterns_order (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    _S3M_PATORDENT *buf;
    MUSMOD *track;
    MUSPATORDER *order;
    MUSPATORDENT index, *p;
    uint16_t count, size, i;
    bool found;

    track = _Self->track;
    order = musmod_get_order (track);
    count = _get_order_length (_Self);
    size = count * sizeof (_S3M_PATORDENT);

    buf = __new (size);
    if (!buf)
    {
        ERROR (self, "Failed to allocate memory for %s.", "patterns order");
        return false;
    }

    if (!fread (buf, size, 1, _Self->f))
    {
        ERROR (self, "Failed to read %s.", "patterns order");
        _delete (buf);
        return false;
    }

    for (i = 0; i < count; i++)
    {
        switch (buf[i])
        {
        case _S3M_PATORDENT_SKIP:
            index = MUSPATORDENT_SKIP;
            break;
        case _S3M_PATORDENT_END:
            index = MUSPATORDENT_END;
            break;
        default:
            if (buf[i] < _S3M_MAX_PATTERNS)
                index = buf[i];
            else
                index = MUSPATORDENT_END;
            break;
        }
        muspatorder_set (order, i, &index);
    }

    _delete (buf);

    // check order if there's one 'real' (playable) entry ...
    found = false;
    i = 0;
    while ((i < count) && (!found))
    {
        p = muspatorder_get (order, i);
        found = *p < _S3M_MAX_PATTERNS;
        i++;
    }

    if (!found)
    {
        ERROR (self, "%s", "Playable entry not found.");
        return false;
    }

    return true;
}

/**********************************************************************/

MUSMOD *load_s3m_load (LOADER_S3M *self, const char *name, bool use_EM)
{
    _S3M_LOADER *_Self = self;
    _S3M_HEADER header;
    MUSMOD *track;
    PCMSMPLIST *samples;
    MUSINSLIST *instruments;
    MUSPATLIST *patterns;
    MUSPATORDER *order;

    if ((!_Self) || (!name))
    {
        ERROR (self, "%s", "Bad arguments.");
        return NULL;
    }

    if (use_EM && emsInstalled && emsGetFreePagesCount ())
        _Self->flags |= _S3M_LOADERFL_USE_EM;
    else
        _Self->flags &= ~_S3M_LOADERFL_USE_EM;

    track = _new (MUSMOD);
    if (!track)
    {
        ERROR (self, "Failed to allocate memory for %s.", "music module");
        return NULL;
    }
    musmod_init (track);
    _Self->track = track;
    samples = musmod_get_samples (track);
    instruments = musmod_get_instruments (track);
    patterns = musmod_get_patterns (track);
    order = musmod_get_order (track);

    load_s3m_clear_error (self);
    _Self->f = fopen (name, "rb");
    if (!_Self->f)
    {
        ERROR (self, "Failed to open file '%s'.", name);
        return NULL;
    }

    if (!load_s3m_read_header (_Self))
        return NULL;

    if (!musinsl_set_count (instruments, _get_instruments_count (_Self)))
    {
        ERROR (self, "Failed to allocate memory for %s.", "instruments list");
        return NULL;
    }

    if (!muspatl_set_count (patterns, _get_patterns_count (_Self)))
    {
        ERROR (self, "Failed to allocate memory for %s.", "patterns list");
        return NULL;
    }

    if (!muspatorder_set_count (order, _get_order_length (_Self)))
    {
        ERROR (self, "Failed to allocate memory for %s.", "patterns order");
        return NULL;
    }

    if (!load_s3m_load_patterns_order (_Self))
        return NULL;

    if (!load_s3m_load_ins_offsets (_Self))
        return NULL;

    if (!load_s3m_load_pat_offsets (_Self))
        return NULL;

    _Self->in_buf = __new (_S3M_IN_PATTERN_SIZE_MAX);
    if (!_Self->in_buf)
    {
        ERROR (self, "Failed to allocate memory for %s.", "input pattern buffer");
        return NULL;
    }

    _Self->pat_buf = __new (_S3M_OUT_PATTERN_SIZE_MAX);
    if (!_Self->pat_buf)
    {
        ERROR (self, "Failed to allocate memory for %s.", "output pattern buffer");
        return NULL;
    }

    if (_Self->flags & _S3M_LOADERFL_USE_EM)
        load_s3m_alloc_EM_patterns_pages (_Self);

    if (!load_s3m_load_patterns (_Self))
        return NULL;

    if (_Self->flags & _S3M_LOADERFL_USE_EM)
        load_s3m_free_unused_patterns_pages (_Self);

    if (!load_s3m_load_ins_headers (_Self))
        return NULL;

    if (!pcmsmpl_set_count (samples, _get_samples_count (_Self)))
    {
        ERROR (self, "Failed to allocate memory for %s.", "samples list");
        return NULL;
    }

    if (!load_s3m_load_instruments (_Self))
        return NULL;

    if (_Self->flags & _S3M_LOADERFL_USE_EM)
        load_s3m_alloc_EM_samples_pages (_Self);

    if (!load_s3m_load_samples (_Self))
        return NULL;

    if (_Self->flags & _S3M_LOADERFL_USE_EM)
        load_s3m_free_unused_samples_pages (_Self);

    musmod_set_loaded (track, true);

    _Self->track = NULL;
    return track;
}

/*** Free ***/

void load_s3m_free (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;

    if (_Self)
    {
        _delete (_Self->in_buf);
        _delete (_Self->pat_buf);
        _delete (_get_instruments_offsets (_Self));
        _delete (_get_instruments_info (_Self));
        _delete (_get_patterns_offsets (_Self));
        if (_Self->f)
        {
            fclose (_Self->f);
            _Self->f = NULL;
        }
        if (_Self->track)
        {
            musmod_free (_Self->track);
            _delete (_Self->track);
        }
    }
}

void load_s3m_delete (LOADER_S3M **self)
{
    _delete (self);
}
