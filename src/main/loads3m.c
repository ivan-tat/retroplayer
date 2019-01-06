/* loads3m.c -- library for loading Scream Tracker 3.x music modules.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

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
#include "main/s3mvars.h"
#include "main/s3mplay.h"
#include "main/loads3m.h"

/* Limits */
#define _S3M_MAX_INSTRUMENTS 99
#define _S3M_MAX_PATTERNS 100
#define _S3M_MAX_CHANNELS 32

#define _S3M_LOADER_BUF_SIZE (10 * 1024)

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
#define _S3M_TRACKER_ST3    1

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
    uint8_t volume;
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
    uint8_t volume;
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
#define _ins_get_sample_loop_start(o)   (o)->data.sample.loopbeg
#define _ins_get_sample_loop_end(o)     (o)->data.sample.loopend
#define _ins_get_sample_rate(o)         (o)->data.sample.rate
#define _ins_get_sample_volume(o)       (o)->data.sample.volume
#define _ins_get_file_name(o)           (o)->dosname
#define _ins_get_title(o)               (o)->title

/*** Errors ***/

typedef int16_t _s3m_error_t;
typedef _s3m_error_t _S3M_ERR;

#define E_S3M_SUCCESS           0
#define E_S3M_DOS_MEM_ALLOC     1
#define E_S3M_EM_ALLOC          2
#define E_S3M_EM_MAP            3
#define E_S3M_FILE_OPEN         4
#define E_S3M_FILE_SEEK         5
#define E_S3M_FILE_READ         6
#define E_S3M_FILE_TYPE         7
#define E_S3M_INSTRUMENT_TYPE   8
#define E_S3M_PACKED_SAMPLE     9
#define E_S3M_STEREO_SAMPLE     10
#define E_S3M_16BITS_SAMPLE     100
#define E_S3M_SAMPLE_SIZE       11
#define E_S3M_PATTERN_SIZE      12
#define E_S3M_PATTERNS_ORDER    13
#define E_S3M_UNKNOWN           -1

static const struct
{
    _S3M_ERR code;
    char *msg;
}

_S3M_ERRORS[] =
{
    {
        E_S3M_DOS_MEM_ALLOC,
        "DOS memory allocation failed"
    },
    {
        E_S3M_EM_ALLOC,
        "Expanded memory allocation failed"
    },
    {
        E_S3M_EM_MAP,
        "Failed to map expanded memory"
    },
    {
        E_S3M_FILE_OPEN,
        "File open failed"
    },
    {
        E_S3M_FILE_SEEK,
        "File seek failed"
    },
    {
        E_S3M_FILE_READ,
        "File read failed"
    },
    {
        E_S3M_FILE_TYPE,
        "Unknown file type"
    },
    {
        E_S3M_INSTRUMENT_TYPE,
        "Unknown instrument type"
    },
    {
        E_S3M_PACKED_SAMPLE,
        "Packed sample is not supported"
    },
    {
        E_S3M_STEREO_SAMPLE,
        "Stereo sample is not supported"
    },
    {
        E_S3M_16BITS_SAMPLE,
        "16-bits sample is not supported"
    },
    {
        E_S3M_SAMPLE_SIZE,
        "Sample is too big"
    },
    {
        E_S3M_PATTERN_SIZE,
        "Bad pattern size"
    },
    {
        E_S3M_PATTERNS_ORDER,
        "Patterns order is corrupted"
    },
    {
        E_S3M_UNKNOWN,
        "Unknown error"
    }
};

/*** Loader ***/

#pragma pack(push, 1);
typedef struct _s3m_instrument_info_t
{
    _S3M_INS header;
    int8_t smp_num;
};
#pragma pack(pop);
typedef struct _s3m_instrument_info_t _S3M_INSINFO;

typedef struct _s3m_loader_t
{
    _S3M_ERR err;
    MUSMOD *track;
    FILE *f;
    char *buffer;
    bool signed_data;
    uint8_t   ins_count;
    uint16_t *ins_offsets;
    _S3M_INSINFO *ins_info;
    uint8_t   smp_count;
    uint8_t   pat_count;
    uint16_t *pat_offsets;
    uint16_t pat_EM_pages;
    uint16_t pat_EM_page;
    uint16_t pat_EM_page_offset;
    uint16_t smp_EM_pages;
    uint16_t smp_EM_page;
};
typedef struct _s3m_loader_t _S3M_LOADER;

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

bool __near load_s3m_allocbuf (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;

    if (_Self)
    {
        if (!_Self->buffer)
            _Self->buffer = __new (_S3M_LOADER_BUF_SIZE);
        if (_Self->buffer)
            return true;
    }

    return false;
}

void __near load_s3m_alloc_patterns (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSPATLIST *patterns;
    uint16_t patsize;
    uint16_t patperpage;
    uint16_t freepages;
    EMSHDL handle;

    track = _Self->track;
    patsize = musmod_get_channels_count (track) * 64 * 5;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("load_s3m_alloc_patterns", "Pattern memory size: %u.", patsize);

    if (UseEMS)
    {
        patterns = musmod_get_patterns (track);
        patperpage = EM_PAGE_SIZE / patsize;
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("load_s3m_alloc_patterns", "Patterns per EM page: %u.", patperpage);
        _Self->pat_EM_pages = (muspatl_get_count (patterns) + patperpage - 1) / patperpage;

        freepages = emsGetFreePagesCount ();
        if (_Self->pat_EM_pages > freepages)
            _Self->pat_EM_pages = freepages;

        handle = emsAlloc (_Self->pat_EM_pages);
        if (emsEC != E_EMS_SUCCESS)
        {
            DEBUG_ERR_ ("load_s3m_alloc_patterns", "Failed to allocate EM for %s.", "patterns");
            muspatl_set_EM_data (patterns, false);
            return;
        }

        muspatl_set_EM_data (patterns, true);
        muspatl_set_own_EM_handle (patterns, true);
        muspatl_set_EM_handle (patterns, handle);

        _Self->pat_EM_page_offset = 0;
        _Self->pat_EM_page = 0;
    }
}

/**********************************************************************/

typedef unsigned char _s3m_pattern_event_flags_t;
typedef _s3m_pattern_event_flags_t _S3M_PATEVENTFLAGS;

#define _S3M_PATEVFL_INS    (1 << 0)
#define _S3M_PATEVFL_NOTE   (1 << 1)
#define _S3M_PATEVFL_VOL    (1 << 2)
#define _S3M_PATEVFL_CMD    (1 << 3)
#define _S3M_PATEVFL_ROWEND (1 << 4)

typedef struct _s3m_pattern_event_t
{
    _S3M_PATEVENTFLAGS flags;
    unsigned char channel;
    unsigned char instrument;
    unsigned char note;
    unsigned char volume;
    unsigned char command;
    unsigned char parameter;
};
typedef struct _s3m_pattern_event_t _S3M_PATEVENT;

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

bool __near _s3m_patio_read_event (_S3M_PATIO *self, _S3M_PATEVENT *event)
{
    unsigned char flags, v[2];

    if (!_s3m_patio_read (self, &flags, 1))
        return false;

    event->instrument = CHNINS_EMPTY;
    event->note = CHNNOTE_EMPTY;
    event->volume = CHNVOL_EMPTY;
    event->command = CHNCMD_EMPTY;
    event->parameter = 0;

    if (flags)
    {
        event->flags = 0;
        event->channel = flags & 0x1f;

        if (flags & 0x20)
        {
            // note, instrument
            if (!_s3m_patio_read (self, v, 2))
                return false;

            switch (v[0])
            {
            case 0xff:
                break;
            case 0xfe:
                event->note = CHNNOTE_OFF;
                event->flags |= _S3M_PATEVFL_NOTE;
                break;
            default:
                if (((v[0] & 0x0f) <= 11) && ((v[0] >> 4) <= 7))
                {
                    event->note = v[0];
                    event->flags |= _S3M_PATEVFL_NOTE;
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
                    event->instrument = v[1];
                    event->flags |= _S3M_PATEVFL_INS;
                }
                break;
            }
        }

        if (flags & 0x40)
        {
            // volume
            if (!_s3m_patio_read (self, v, 1))
                return false;

            switch (v[0])
            {
            case 0xff:
                break;
            default:
                if (v[0] <= 64)
                {
                    event->volume = v[0];
                    event->flags |= _S3M_PATEVFL_VOL;
                }
                break;
            }
        }

        if (flags & 0x80)
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
                    event->command = v[0];
                    event->parameter = v[1];
                    event->flags |= _S3M_PATEVFL_CMD;
                }
                break;
            }
        }
        return true;
    }
    else
    {
        event->flags = _S3M_PATEVFL_ROWEND;
        event->channel = 0;
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
    _S3M_PATEVENT ei;
    MUSPATIO f;
    MUSPATROWEVENT e;
    MUSPATCHNEVENT events[_S3M_MAX_CHANNELS];
    uint16_t maxrow, r;
    uint8_t maxchn, c;
    uint16_t offset;
    #if DEBUG_FILE_S3M_LOAD == 1
    char s[13], *e_str;
    #endif  /* DEBUG_FILE_S3M_LOAD */

    muspat_set_data_packed (pattern, pack);

    if (!muspatio_open (&f, pattern, MUSPATIOMD_WRITE))
    {
        DEBUG_ERR_ ("load_s3m_convert_pattern", "Failed to open pattern for writing (%s).", f.error);
        return false;
    }

    src = _Self->buffer;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_dump_mem (src, src_len, "data: ");

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
            if (ei.flags & _S3M_PATEVFL_ROWEND)
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

                if (ei.flags)
                {
                    if (ei.flags & _S3M_PATEVFL_INS)
                    {
                        e.event.instrument = ei.instrument;
                        e.event.flags |= MUSPATCHNEVFL_INS;
                    }

                    if (ei.flags & _S3M_PATEVFL_NOTE)
                    {
                        e.event.note = ei.note;
                        e.event.flags |= MUSPATCHNEVFL_NOTE;
                    }

                    if (ei.flags & _S3M_PATEVFL_VOL)
                    {
                        e.event.volume = ei.volume;
                        e.event.flags |= MUSPATCHNEVFL_VOL;
                    }

                    if (ei.flags & _S3M_PATEVFL_CMD)
                    {
                        e.event.command = ei.command;
                        e.event.parameter = ei.parameter;
                        e.event.flags |= MUSPATCHNEVFL_CMD;
                    }

                    if (DEBUG_FILE_S3M_LOAD)
                    {
                        if (e.event.flags)
                        {
                            DEBUG_get_pattern_channel_event_str (s, & (e.event));
                            e_str = s;
                        }
                        else
                            e_str = "empty";
                    }

                    memcpy (& (events [e.channel]), & (e.event), sizeof (MUSPATCHNEVENT));
                }
                else
                {
                    if (DEBUG_FILE_S3M_LOAD)
                        e_str = "empty";
                }

                if (DEBUG_FILE_S3M_LOAD)
                    _DEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL,
                        "row=%02hhu, offset=0x%04X, size=%hhu, type=event <%02hhu:%s>",
                        r,
                        offset,
                        fi.offset - offset,
                        e.channel,
                        e_str
                    );
            }
        }
        else
        {
            DEBUG_ERR_ ("load_s3m_convert_pattern", "Failed to read pattern (%s).", fi.error);
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
    uint16_t length;
    MUSPAT pat_static, *pat;
    void *data;
    bool em;
    char s[_BUF_SIZE];

    track = _Self->track;
    patterns = musmod_get_patterns (track);
    pos = _Self->pat_offsets[index] * 16;

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
        DEBUG_ERR_ ("load_s3m_load_pattern", "Failed to read %s.", "file");
        _Self->err = E_S3M_FILE_SEEK;
        return false;
    }
    if (!fread (&length, 2, 1, _Self->f))
    {
        DEBUG_ERR_ ("load_s3m_load_pattern", "Failed to read %s.", "pattern");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if ((length <= 2) || (length > _S3M_LOADER_BUF_SIZE))
    {
        DEBUG_ERR ("load_s3m_load_pattern", "Bad pattern size.");
        _Self->err = E_S3M_PATTERN_SIZE;
        return false;
    }

    length -= 2;
    if (!fread (_Self->buffer, length, 1, _Self->f))
    {
        DEBUG_ERR_ ("load_s3m_load_pattern", "Failed to read %s.", "pattern");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    pat = &pat_static;
    muspat_init (pat);
    muspat_set_channels (pat, musmod_get_channels_count (track));
    muspat_set_rows (pat, 64);
    muspat_set_size (pat, muspat_get_channels (pat) * muspat_get_rows (pat) * 5);

    // try to put in EM
    em = false;
    if (muspatl_is_EM_data (patterns) && _Self->pat_EM_pages)
    {
        em = true;
        if (muspat_is_data_packed (pat))
            length = muspat_get_packed_size (pat);
        else
            length = muspat_get_size (pat);
        length = (length + 15) & ~15;   // round size to 16 bytes
        if (_Self->pat_EM_page_offset + length > EM_PAGE_SIZE)
        {
            // EM page overflow, try next page
            _Self->pat_EM_page++;
            _Self->pat_EM_page_offset = 0;
            _Self->pat_EM_pages--;
            em = _Self->pat_EM_pages && (_Self->pat_EM_page_offset + length <= EM_PAGE_SIZE);
        }
    }

    if (em)
    {
        muspat_set_EM_data (pat, true);
        muspat_set_own_EM_handle (pat, false);
        muspat_set_EM_data_handle (pat, muspatl_get_EM_handle (patterns));
        muspat_set_EM_data_page (pat, _Self->pat_EM_page);
        muspat_set_EM_data_offset (pat, _Self->pat_EM_page_offset);
        _Self->pat_EM_page_offset += length;    // next available offset
    }
    else
    {
        data = __new (muspat_get_size (pat));
        if (!data)
        {
            DEBUG_ERR_ ("load_s3m_load_pattern", "Failed to allocate memory for %s.", "pattern");
            _Self->err = E_S3M_DOS_MEM_ALLOC;
            return false;
        }
        muspat_set_EM_data (pat, false);
        muspat_set_data (pat, data);
    }

    muspatl_set (patterns, index, pat);
    pat = muspatl_get (patterns, index);    // update pointer

    if (DEBUG_FILE_S3M_LOAD)
    {
        if (muspat_is_EM_data (pat))
            snprintf (s, 64,
                "EM, page=0x%04X, offset=0x%04X",
                muspat_get_EM_data_page (pat),
                muspat_get_EM_data_offset (pat)
            );
        else
        {
            data = muspat_get_data (pat);
            snprintf (s, 64,
                "DOS, address=0x%04X:0x%04X",
                FP_SEG (data),
                FP_OFF (data)
            );
        }

        DEBUG_MSG_ (
            "load_s3m_load_pattern",
            "index=%hu, file_size=%u, mem_size=%u, data=%s",
            index,
            length,
            muspat_get_size (pat),
            s
        );
    }

    // TODO: use extra buffer for unpacked pattern to determine real data size of final packed data
    if (load_s3m_convert_pattern (self, _Self->buffer, length, pat, true))
    {
        if (muspat_is_EM_data (pat) && muspat_is_data_packed (pat))
            muspat_set_size (pat, muspat_get_packed_size (pat));    // reduce memory usage

        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_dump_pattern (pat, s, musmod_get_channels_count (track));
        return true;
    }
    else
        return false;
    #undef _BUF_SIZE
}

bool __near load_s3m_load_ins_offsets (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    uint16_t size;

    size = _Self->ins_count * sizeof (uint16_t);

    _Self->ins_offsets = __new (size);
    if (! _Self->ins_offsets)
    {
        DEBUG_ERR_ ("load_s3m_load_ins_offsets", "Failed to allocate memory for %s.", "instruments offsets");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return false;
    }

    if (!fread (_Self->ins_offsets, size, 1, _Self->f))
    {
        DEBUG_ERR_ ("load_s3m_load_ins_offsets", "Failed to read %s.", "instruments offsets");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    return true;
}

bool __near load_s3m_load_pat_offsets (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    uint16_t size;

    size = sizeof (uint16_t) * _Self->pat_count;

    _Self->pat_offsets = __new (size);
    if (!_Self->pat_offsets)
    {
        DEBUG_ERR_ ("load_s3m_load_pat_offsets", "Failed to allocate memory for %s.", "pattern offsets");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return false;
    }

    if (!fread (_Self->pat_offsets, size, 1, _Self->f))
    {
        DEBUG_ERR_ ("load_s3m_load_pat_offsets", "Failed to read %s.", "patterns offsets");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    return true;
}

bool __near load_s3m_load_ins_headers (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    _S3M_INSINFO *info;
    uint8_t i;

    _Self->ins_info = __new (sizeof (_S3M_INSINFO) * _Self->ins_count);
    if (!_Self->ins_info)
    {
        DEBUG_ERR_ ("load_s3m_load_ins_headers", "Failed to allocate memory for %s.", "instruments headers");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return false;
    }

    _Self->smp_count = 0;
    for (i = 0; i < _Self->ins_count; i++)
    {
        if (fsetpos (_Self->f, _Self->ins_offsets[i] * 16))
        {
            DEBUG_ERR_ ("load_s3m_load_ins_headers", "Failed to read %s.", "file");
            _Self->err = E_S3M_FILE_SEEK;
            return false;
        }

        info = & (_Self->ins_info[i]);
        if (!fread (& (info->header), sizeof (_S3M_INS), 1, _Self->f))
        {
            DEBUG_ERR_ ("load_s3m_load_ins_headers", "Failed to read %s.", "instrument header");
            _Self->err = E_S3M_FILE_READ;
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
                DEBUG_ERR_ ("load_s3m_load_ins_headers", "%s is not supported.", "Packed sample");
                _Self->err = E_S3M_PACKED_SAMPLE;
                return false;
            }
            if (info->header.data.sample.flags & _S3M_SMPFL_STEREO)
            {
                DEBUG_ERR_ ("load_s3m_load_ins_headers", "%s is not supported.", "Stereo sample");
                _Self->err = E_S3M_STEREO_SAMPLE;
                return false;
            }
            if (info->header.data.sample.flags & _S3M_SMPFL_16BITS)
            {
                DEBUG_ERR_ ("load_s3m_load_ins_headers", "%s is not supported.", "16-bits sample");
                _Self->err = E_S3M_16BITS_SAMPLE;
                return false;
            }
            info->smp_num = _Self->smp_count;
            _Self->smp_count++;
            break;
        case _S3M_INST_AMEL:
        case _S3M_INST_ABD:
        case _S3M_INST_ASNARE:
        case _S3M_INST_ATOM:
        case _S3M_INST_ACYM:
        case _S3M_INST_AHIHAT:
            DEBUG_ERR_ ("load_s3m_load_ins_headers", "%s is not supported.", "Adlib instrument");
            _Self->err = E_S3M_INSTRUMENT_TYPE;
            return false;
        default:
            DEBUG_ERR ("load_s3m_load_ins_headers", "Unknown instrument type.");
            _Self->err = E_S3M_INSTRUMENT_TYPE;
            return false;
        }
        DEBUG_INFO_ ("load_s3m_load_ins_headers", "i=%d, s=%d", i, info->smp_num);
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
    char smp_title[_S3M_INS_FILENAME_LEN + 1];  /* including terminating zero */
    char ins_title[MUSINS_TITLE_LEN + 1];       /* including terminating zero */

    info = & (_Self->ins_info[index]);
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
        pcmsmp_set_size (smp, _ins_get_sample_length (_ins));

        if (_ins_is_sample_looped (_ins))
        {
            pcmsmp_set_loop (smp, PCMSMPLOOP_FORWARD);
            pcmsmp_set_loop_start (smp, _ins_get_sample_loop_start (_ins));
            pcmsmp_set_loop_end (smp, _ins_get_sample_loop_end (_ins));
        }
        else
            pcmsmp_set_loop (smp, PCMSMPLOOP_NONE);

        pcmsmp_set_rate (smp, _ins_get_sample_rate (_ins));
        pcmsmp_set_volume (smp, _ins_get_sample_volume (_ins));

        memcpy (smp_title, _ins_get_file_name (_ins), _S3M_INS_FILENAME_LEN);
        smp_title[_S3M_INS_FILENAME_LEN] = 0;
        pcmsmp_set_title (smp, smp_title);

        musins_set_type (ins, MUSINST_PCM);
        musins_set_volume (ins, _ins_get_sample_volume (_ins));    /* FIXME: split sample and instrument volumes */
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

uint32_t __near _calc_sample_mem_size (uint32_t size)
{
    return size + 1024;
}

void __near load_s3m_alloc_samples (LOADER_S3M *self)
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
    for (i = 0; i < _Self->ins_count; i++)
    {
        ins = musinsl_get (instruments, i);
        if (musins_get_type (ins) == MUSINST_PCM)
        {
            smp = musins_get_sample (ins);
            data_size = _calc_sample_mem_size (_calc_sample_load_size (smp));
            pages += (uint32_t) (data_size + EM_PAGE_SIZE - 1) / EM_PAGE_SIZE;
        }
    }

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("load_s3m_alloc_samples",
            "Samples to load: %u (EM pages: %u)",
            _Self->smp_count,
            pages
        );

    free_pages = emsGetFreePagesCount ();
    if (!free_pages)
    {
        DEBUG_WARN_ ("load_s3m_alloc_samples", "Not enough EM for %s.", "samples");
        pcmsmpl_set_EM_data (samples, false);
        return;
    }

    if (pages > free_pages)
        pages = free_pages;

    handle = emsAlloc (pages);
    if (emsEC != E_EMS_SUCCESS)
    {
        DEBUG_ERR_ ("load_s3m_alloc_samples", "Failed to allocate EM for %s.", "samples");
        pcmsmpl_set_EM_data (samples, false);
        return;
    }
    pcmsmpl_set_EM_data (samples, true);
    pcmsmpl_set_EM_handle (samples, handle);

    _Self->smp_EM_pages = pages;
    _Self->smp_EM_page = 0;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("load_s3m_alloc_samples",
            "EM pages allocated: %u.",
            pages
        );
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
    void *loopstart;
    uint32_t loopsize;

    info = & (_Self->ins_info[index]);
    if (fsetpos (_Self->f, _ins_get_sample_offset (& (info->header))))
    {
        DEBUG_ERR_ ("load_s3m_load_sample", "Failed to read %s.", "file");
        _Self->err = E_S3M_FILE_SEEK;
        return false;
    }

    track = _Self->track;
    samples = musmod_get_samples (track);
    instruments = musmod_get_instruments (track);
    ins = musinsl_get (instruments, index);
    smp = pcmsmpl_get (samples, info->smp_num);
    load_size = _calc_sample_load_size (smp);
    data_size = _calc_sample_mem_size (load_size);
    if (data_size > 0xffff)
    {
        DEBUG_ERR ("load_s3m_load_sample", "Sample is too large.");
        _Self->err = E_S3M_SAMPLE_SIZE;
        return false;
    }

    pages = (uint32_t) (data_size + EM_PAGE_SIZE - 1) / EM_PAGE_SIZE;

    if (UseEMS && pcmsmpl_is_EM_data (samples) && (_Self->smp_EM_pages >= pages))
    {
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("load_s3m_load_sample",
                "sample=%02u, place=EM, pages=%u-%u.",
                index,
                _Self->smp_EM_page,
                _Self->smp_EM_page + pages - 1
            );
        pcmsmp_set_EM_data (smp, true);
        pcmsmp_set_own_EM_handle (smp, false);
        pcmsmp_set_EM_data_handle (smp, pcmsmpl_get_EM_handle (samples));
        pcmsmp_set_EM_data_page (smp, _Self->smp_EM_page);
        pcmsmp_set_EM_data_offset (smp, 0);
        data = pcmsmp_map_EM_data (smp);
        if (!data)
        {
            DEBUG_ERR_ ("load_s3m_load_sample", "Failed to map EM for %s.", "sample");
            _Self->err = E_S3M_EM_MAP;
            return false;
        }
        _Self->smp_EM_page += pages;
    }
    else
    {
        data = __new (data_size);
        if (!data)
        {
            DEBUG_ERR_ ("load_s3m_load_sample", "Failed to allocate memory for %s.", "sample data");
            _Self->err = E_S3M_DOS_MEM_ALLOC;
            return false;
        }
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("load_s3m_load_sample",
                "sample=%02u, data=DOS, address=0x%04X:0x%04X.",
                index,
                FP_SEG (data),
                FP_OFF (data)
            );
        pcmsmp_set_EM_data (smp, false);
        pcmsmp_set_data (smp, data);
    }

    if (!fread (data, load_size, 1, _Self->f))
    {
        DEBUG_ERR_ ("load_s3m_load_sample", "Failed to read %s.", "sample data");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if (!_Self->signed_data)
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

uint8_t __near getchtyp(uint8_t b)
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

MUSMOD *load_s3m_load (LOADER_S3M *self, const char *name)
{
    _S3M_LOADER *_Self = self;
    _S3M_HEADER header;
    MUSMOD *track;
    MUSMODCHNPAN pan;
    PCMSMPLIST *samples;
    MUSINSLIST *instruments;
    MUSPATLIST *patterns;
    //PATORD *order;
    uint8_t maxused, i, smpnum;
    uint16_t count;

    if ((!_Self) || (!name))
    {
        DEBUG_ERR ("load_s3m_load", "Bad arguments.");
        return NULL;
    }

    track = _new (MUSMOD);
    if (!track)
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to allocate memory for %s.", "music module");
        return NULL;
    }
    musmod_init (track);
    _Self->track = track;
    samples = musmod_get_samples (track);
    instruments = musmod_get_instruments (track);
    patterns = musmod_get_patterns (track);
    //order = musmod_get_order (track);

    UseEMS = UseEMS && emsInstalled && emsGetFreePagesCount ();

    _Self->err = E_S3M_SUCCESS;
    _Self->f = fopen (name, "rb");
    if (!_Self->f)
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to open file '%s'.", name);
        _Self->err = E_S3M_FILE_OPEN;
        return NULL;
    }

    if (!fread (&header, sizeof (_S3M_HEADER), 1, _Self->f))
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to read %s.", "file''s header");
        _Self->err = E_S3M_FILE_READ;
        return NULL;
    }

    if ((header.type != _S3M_FILE_TYPE)
    || (header.magic != _S3M_FILE_MAGIC)
    || ((header.tracker & (_S3M_TRACKER_TYPE_MASK | _S3M_TRACKER_VER_MAJOR_MASK)) !=
        ((_S3M_TRACKER_ST3 << _S3M_TRACKER_TYPE_SHIFT) | (_S3M_TRACKER_VER_MAJOR_ST3 << _S3M_TRACKER_VER_MAJOR_SHIFT)))
    || ((header.format != _S3M_FILE_FORMAT_1) && (header.format != _S3M_FILE_FORMAT_2)))
    {
        DEBUG_ERR_ ("load_s3m_load", "%s is not supported.", "file format");
        _Self->err = E_S3M_FILE_TYPE;
        return NULL;
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
        DEBUG_ERR ("load_s3m_load", "All channels are disabled.");
        _Self->err = E_S3M_UNKNOWN;
        return NULL;
    }

    musmod_set_channels_count (track, maxused);

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("load_s3m_load", "Channels: %hu", musmod_get_channels_count (track));

    snprintf (
        musmod_get_format (track),
        MUSMOD_FORMAT_LEN,
        "Scream Tracker %hhx.%02hhx module",
        (header.tracker & _S3M_TRACKER_VER_MAJOR_MASK) >> _S3M_TRACKER_VER_MAJOR_SHIFT,
        (header.tracker & _S3M_TRACKER_VER_MINOR_MASK) >> _S3M_TRACKER_VER_MINOR_SHIFT
    );

    header.name[_S3M_TITLE_LEN - 1] = 0;
    musmod_set_title (track, header.name);

    _Self->ins_count = header.insnum;
    _Self->pat_count = header.patnum;
    musmod_set_order_length (track, header.ordnum);
    musmod_set_stereo (track, (header.mvolume & _S3M_MVOL_STEREO) != 0);
    musmod_set_amiga_limits (track, (header.flags & _S3M_FLAG_AMIGA_LIMITS) != 0);
    //musmod_set_global_volume (track, header.gvolume);
    //musmod_set_master_volume (track, (header.mvolume & _S3M_MVOL_MASK) >> _S3M_MVOL_SHIFT);
    playState_gVolume   = header.gvolume;
    playState_mVolume   = (header.mvolume & _S3M_MVOL_MASK) >> _S3M_MVOL_SHIFT;
    musmod_set_tempo (track, header.initialtempo);
    musmod_set_speed (track, header.initialspeed);
    _Self->signed_data = (header.format == _S3M_FILE_FORMAT_1);

    // TODO: allocate patterns order array (order)

    if (!musinsl_set_count (instruments, _Self->ins_count))
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to allocate memory for %s.", "instruments list");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return NULL;
    }

    if (!muspatl_set_count (patterns, _Self->pat_count))
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to allocate memory for %s.", "patterns list");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return NULL;
    }

    //if (!fread (order, musmod_get_order_length (track), 1, _Self->f))
    if (!fread (&Order, musmod_get_order_length (track), 1, _Self->f))
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to read %s.", "patterns order");
        _Self->err = E_S3M_FILE_READ;
        return NULL;
    }

    // check order if there's one 'real' (playable) entry ...
    i = 0;
    //while ((i < musmod_get_order_length (track)) && (*order[i] >= 254))
    while ((i < musmod_get_order_length (track)) && (Order[i] >= 254))
        i++;

    if (i == musmod_get_order_length (track))
    {
        DEBUG_ERR ("load_s3m_load", "Playable entry not found.");
        _Self->err = E_S3M_PATTERNS_ORDER;
        return NULL;
    }

    if (!load_s3m_load_ins_offsets (_Self))
        return NULL;

    if (!load_s3m_load_pat_offsets (_Self))
        return NULL;

    if (!load_s3m_allocbuf (_Self))
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to allocate memory for %s.", "file buffer");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return NULL;
    }

    load_s3m_alloc_patterns (_Self);

    count = muspatl_get_count (patterns);
    for (i = 0; i < count; i++)
        if (!load_s3m_load_pattern (_Self, i))
            return NULL;

    if (!load_s3m_load_ins_headers (_Self))
        return NULL;

    if (!pcmsmpl_set_count (samples, _Self->smp_count))
    {
        DEBUG_ERR_ ("load_s3m_load", "Failed to allocate memory for %s.", "samples list");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return NULL;
    }

    count = _Self->ins_count;
    for (i = 0; i < count; i++)
        if (!load_s3m_load_instrument (_Self, i))
            return NULL;

    if (UseEMS)
        load_s3m_alloc_samples (_Self);

    for (i = 0; i < count; i++)
        if (_Self->ins_info[i].smp_num >= 0)
            if (!load_s3m_load_sample (_Self, i))
                return NULL;

    if (pcmsmpl_is_EM_data (samples))
        pcmsmpl_set_EM_handle_name (samples, "smplist");

    if (muspatl_is_EM_data (patterns))
        muspatl_set_EM_handle_name (patterns);

    musmod_set_loaded (track, true);

    _Self->track = NULL;
    return track;
}

const char *load_s3m_get_error (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;
    uint16_t i;

    if (_Self)
    {
        if (_Self->err == E_S3M_SUCCESS)
            return NULL;

        i = 0;
        while (_S3M_ERRORS[i].code != E_S3M_UNKNOWN)
        {
            if (_S3M_ERRORS[i].code == _Self->err)
                return _S3M_ERRORS[i].msg;
            i++;
        }

        return _S3M_ERRORS[i].msg;
    }
    else
        return NULL;
}

void load_s3m_free (LOADER_S3M *self)
{
    _S3M_LOADER *_Self = self;

    if (_Self)
    {
        _delete (_Self->buffer);
        _delete (_Self->ins_offsets);
        _delete (_Self->pat_offsets);
        _delete (_Self->ins_info);
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
