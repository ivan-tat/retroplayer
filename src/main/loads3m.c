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

#define S3M_PAT_MIN    0
#define S3M_PAT_MAX    99
#define S3M_PAT_SKIP   0xfe
#define S3M_PAT_EMPTY  0xff

#define LOADER_BUF_SIZE (10 * 1024)

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

#pragma pack(push, 1);
typedef struct S3M_header
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
typedef struct S3M_header S3MHEADER;

/*** Packed pattern lead byte flags ***/

#define S3MPATFL_CHNMASK  0x1f
#define S3MPATFL_NOTE_INS 0x20
#define S3MPATFL_VOL      0x40
#define S3MPATFL_CMD_PARM 0x80

/*** PCM sample ***/

typedef uint8_t S3MSMPFLAGS;
#define S3MSMPFL_LOOP   0x01
#define S3MSMPFL_STEREO 0x02
#define S3MSMPFL_16BITS 0x04

#pragma pack(push, 1);
typedef struct S3M_PCM_sample
{
    uint8_t filepos_hi;
    uint16_t filepos;
    uint32_t length;
    uint32_t loopbeg;
    uint32_t loopend;
    uint8_t volume;
    uint8_t unused1;
    uint8_t packinfo;
    S3MSMPFLAGS flags;
    uint32_t rate;
    uint8_t unused2[12];
};
#pragma pack(pop);
typedef struct S3M_PCM_sample S3MPCMSMP;

/*** AdLib instrument ***/

#pragma pack(push, 1);
typedef struct S3M_adlib_instrument
{
    uint8_t unused1[3];
    uint8_t data[12];
    uint8_t volume;
    uint8_t unused2[3];
    uint32_t rate;
    uint8_t unused3[12];
};
#pragma pack(pop);
typedef struct S3M_adlib_instrument S3MADLIBINS;

/*** Instrument ***/

typedef uint8_t S3MINSTYPE;
#define S3MINST_EMPTY  0x00
#define S3MINST_PCM    0x01
#define S3MINST_AMEL   0x02
#define S3MINST_ABD    0x03
#define S3MINST_ASNARE 0x04
#define S3MINST_ATOM   0x05
#define S3MINST_ACYM   0x06
#define S3MINST_AHIHAT 0x07

#define S3MINS_TITLE_LENGTH_MAX 28

#pragma pack(push, 1);
typedef struct S3M_instrument
{
    S3MINSTYPE type;
    char dosname[12];
    union
    {
        S3MPCMSMP sample;
        S3MADLIBINS adlib;
    } data;
    char title[S3MINS_TITLE_LENGTH_MAX];
    uint32_t magic;
};
#pragma pack(pop);
typedef struct S3M_instrument S3MINS;

/*** Errors ***/

typedef int16_t S3M_error_t;
typedef S3M_error_t S3MERR;

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
    S3MERR code;
    char *msg;
}

S3M_ERRORS[] =
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

typedef struct _s3m_loader_t
{
    S3MERR err;
    MUSMOD *track;
    FILE *f;
    char *buffer;
    bool signed_data;
    uint16_t inspara[_S3M_MAX_INSTRUMENTS];
    uint16_t patpara[_S3M_MAX_PATTERNS];
    uint32_t smppara[_S3M_MAX_INSTRUMENTS];
    uint16_t pat_EM_pages;
    uint16_t pat_EM_page;
    uint16_t pat_EM_page_offset;
    uint16_t smp_EM_pages;
    uint16_t smp_EM_page;
};
typedef struct _s3m_loader_t _S3M_LOADER;

S3MLOADER *s3mloader_new(void)
{
    return _new (_S3M_LOADER);
}

void s3mloader_init(S3MLOADER *self)
{
    _S3M_LOADER *_Self = self;

    if (_Self)
        memset(_Self, 0, sizeof(_S3M_LOADER));
}

bool __near s3mloader_allocbuf(S3MLOADER *self)
{
    _S3M_LOADER *_Self = self;
    uint16_t seg;

    if (_Self)
        if (!_Self->buffer)
        {
            if (!_dos_allocmem(_dos_para(LOADER_BUF_SIZE), &seg))
            {
                _Self->buffer = MK_FP(seg, 0);
                return true;
            }
        }
        else
            return true;

    return false;
}

void __near s3mloader_alloc_patterns(S3MLOADER *self)
{
    _S3M_LOADER *_Self = self;
    MUSPATLIST *patterns;
    uint16_t patsize;
    uint16_t patperpage;
    uint16_t freepages;
    EMSHDL handle;

    patterns = mod_Patterns;
    patsize = mod_ChannelsCount * 64 * 5;
    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ (NULL, "Pattern memory size: %u.", patsize);

    if (UseEMS)
    {
        patperpage = 16 * 1024 / patsize;
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ (NULL, "Patterns per EM page: %u.", patperpage);
        _Self->pat_EM_pages = (muspatl_get_count (patterns) + patperpage - 1) / patperpage;

        freepages = emsGetFreePagesCount();
        if (_Self->pat_EM_pages > freepages)
            _Self->pat_EM_pages = freepages;

        handle = emsAlloc(_Self->pat_EM_pages);
        if (emsEC != E_EMS_SUCCESS)
        {
            DEBUG_ERR("s3mloader_load", "Failed to allocate EM for patterns.");
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

bool __near s3mloader_convert_pattern (S3MLOADER *self, uint8_t *src, uint16_t src_len, MUSPAT *pattern, bool pack)
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
        DEBUG_ERR_ ("s3mloader_convert_pattern", "Failed to open pattern for writing (%s).", f.error);
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
            DEBUG_ERR_ (
                "s3mloader_convert_pattern",
                "Failed to read pattern (%s).",
                fi.error
            );
            muspatio_close (&f);
            _s3m_patio_close (&fi);
            return false;
        }
    }

    muspatio_close (&f);
    _s3m_patio_close (&fi);

    return true;
}

void __near s3mloader_dump_patterns (S3MLOADER *self)
{
    _S3M_LOADER *_Self = self;
    MUSPATLIST *patterns;
    MUSPAT *pattern;
    unsigned int count, i;
    char s[_S3M_MAX_CHANNELS][13];

    patterns = mod_Patterns;
    count = muspatl_get_count (patterns);
    for (i = 0; i < count; i++)
    {
        pattern = muspatl_get (patterns, i);
        DEBUG_dump_pattern (pattern, s, mod_ChannelsCount);
    }
}

bool __near s3mloader_load_pattern(S3MLOADER *self, uint8_t index)
{
    _S3M_LOADER *_Self = self;
    MUSPATLIST *patterns;
    uint32_t pos;
    uint16_t length;
    MUSPAT pat_static;
    MUSPAT *pat;
    void *data;
    uint16_t seg;
    bool em;
    char s[64];

    patterns = mod_Patterns;
    pos = _Self->patpara[index] * 16;

    if (!pos)
    {
        /*
        pat = muspatl_get (patterns, index);
        muspat_init(pat);   // is not necessary because it was already initialized by DYNARR's set_size().
        */
        return true;
    }

    if (fsetpos(_Self->f, pos))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Failed to read file.");
        _Self->err = E_S3M_FILE_SEEK;
        return false;
    }
    if (!fread(&length, 2, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Failed to read pattern.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if ((length <= 2) || (length > LOADER_BUF_SIZE))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Bad pattern size.");
        _Self->err = E_S3M_PATTERN_SIZE;
        return false;
    }
    length -= 2;
    if (!fread(_Self->buffer, length, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Failed to read pattern.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    pat = &pat_static;
    muspat_init(pat);
    muspat_set_channels(pat, mod_ChannelsCount);
    muspat_set_rows(pat, 64);
    muspat_set_size(pat, muspat_get_channels(pat) * muspat_get_rows(pat) * 5);

    // try to put in EM
    em = false;
    if (muspatl_is_EM_data (patterns) && _Self->pat_EM_pages)
    {
        em = true;
        if (_Self->pat_EM_page_offset + muspat_get_size(pat) > 16 * 1024)
        {
            _Self->pat_EM_page++;
            _Self->pat_EM_page_offset = 0;
            _Self->pat_EM_pages--;
            if (!_Self->pat_EM_pages || (_Self->pat_EM_page_offset + muspat_get_size(pat) > 16 * 1024))
                em = false;
        }
    }

    if (em)
    {
        muspat_set_EM_data(pat, true);
        muspat_set_own_EM_handle(pat, false);
        muspat_set_EM_data_handle (pat, muspatl_get_EM_handle (patterns));
        muspat_set_EM_data_page(pat, _Self->pat_EM_page);
        muspat_set_EM_data_offset(pat, _Self->pat_EM_page_offset);
    }
    else
    {
        if (_dos_allocmem(_dos_para(muspat_get_size(pat)), &seg))
        {
            DEBUG_ERR("s3mloader_load_pattern", "Failed to allocate DOS memory for pattern.");
            _Self->err = E_S3M_DOS_MEM_ALLOC;
            return false;
        }
        muspat_set_EM_data(pat, false);
        muspat_set_data (pat, MK_FP (seg, 0));
    }

    muspatl_set (patterns, index, pat);
    pat = muspatl_get (patterns, index);

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
            "s3mloader_load_pattern",
            "index=%hu, file_size=%u, mem_size=%u, data=%s",
            index,
            length,
            muspat_get_size (pat),
            s
        );
    }

    return s3mloader_convert_pattern (self, _Self->buffer, length, pat, true);
}

bool __near s3mloader_load_instrument(S3MLOADER *self, uint8_t index)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSINSLIST *instruments;
    uint16_t length;
    uint8_t typ;
    MUSINS *ins;
    S3MINS s3mins;

    if (fsetpos(_Self->f, _Self->inspara[index] * 16))
    {
        DEBUG_ERR("s3mloader_load_instrument", "Failed to read file.");
        _Self->err = E_S3M_FILE_SEEK;
        return false;
    }

    track = _Self->track;
    instruments = mod_Instruments;
    ins = musinsl_get (instruments, index);
    musins_init(ins);

    if (!fread(&s3mins, sizeof(S3MINS), 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load_instrument", "Failed to read instrument header.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if (s3mins.type == S3MINST_PCM)
    {
        if (s3mins.data.sample.packinfo)
        {
            DEBUG_ERR("s3mloader_load_instrument", "Packed sample is not supported.");
            _Self->err = E_S3M_PACKED_SAMPLE;
            return false;
        }
        if (s3mins.data.sample.flags & S3MSMPFL_STEREO)
        {
            DEBUG_ERR("s3mloader_load_instrument", "Stereo sample is not supported.");
            _Self->err = E_S3M_STEREO_SAMPLE;
            return false;
        }
        if (s3mins.data.sample.flags & S3MSMPFL_16BITS)
        {
            DEBUG_ERR("s3mloader_load_instrument", "16-bits sample is not supported.");
            _Self->err = E_S3M_16BITS_SAMPLE;
            return false;
        }
        musins_set_type(ins, MUSINST_PCM);
        musins_set_length(ins, s3mins.data.sample.length);
        if (s3mins.data.sample.flags & S3MSMPFL_LOOP)
        {
            musins_set_looped(ins, true);
            musins_set_loop_start(ins, s3mins.data.sample.loopbeg);
            musins_set_loop_end(ins, s3mins.data.sample.loopend);
        }
        musins_set_volume(ins, s3mins.data.sample.volume);
        musins_set_rate(ins, s3mins.data.sample.rate);
        _Self->smppara[index] = s3mins.data.sample.filepos + ((uint32_t)s3mins.data.sample.filepos_hi << 16);
    }
    else
    {
        musins_set_type(ins, MUSINST_EMPTY);
        _Self->smppara[index] = 0;
    }
    s3mins.title[S3MINS_TITLE_LENGTH_MAX - 1] = 0;  // for weird case
    musins_set_title(ins, s3mins.title);

    return true;
}

uint32_t __near _calc_sample_mem_size(uint32_t size)
{
    return size + 1024;
}

void __near s3mloader_alloc_samples(S3MLOADER *self)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSINSLIST *instruments;
    uint16_t pages;
    uint32_t memsize;
    int16_t i;
    MUSINS *ins;
    EMSHDL handle;

    track = _Self->track;
    instruments = mod_Instruments;

    if (!emsGetFreePagesCount())
    {
        DEBUG_WARN("s3mloader_alloc_samples", "Not enough EM for samples.");
        musinsl_set_EM_data (instruments, false);
        return;
    }

    pages = 0;
    for (i = 0; i < _S3M_MAX_INSTRUMENTS; i++)
    {
        ins = musinsl_get (instruments, i);
        if (musins_get_type(ins) == MUSINST_PCM)
        {
            if (musins_is_looped(ins))
                memsize = musins_get_loop_end(ins);
            else
                memsize = musins_get_length(ins);
            memsize = _calc_sample_mem_size(memsize);
            pages += (memsize + 16 * 1024 - 1) / (16 * 1024);
        }
    }

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("s3mloader_alloc_samples",
            "Instruments: %u, EM pages requested for samples: %u.",
            musinsl_get_count (instruments),
            pages
        );

    if (pages)
    {
        if (pages > emsGetFreePagesCount())
            pages = emsGetFreePagesCount();

        handle = emsAlloc (pages);
        if (emsEC != E_EMS_SUCCESS)
        {
            DEBUG_ERR ("s3mloader_alloc_samples", "Failed to allocate EM for samples.");
            musinsl_set_EM_data (instruments, false);
            return;
        }
        musinsl_set_EM_data (instruments, true);
        musinsl_set_EM_data_handle (instruments, handle);
    }

    _Self->smp_EM_pages = pages;
    _Self->smp_EM_page = 0;

    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ ("s3mloader_alloc_samples", "EM pages allocated for samples: %u.", pages);
}

void __near convert_sign_8(void *data, uint32_t size)
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

bool __near s3mloader_load_sample(S3MLOADER *self, uint8_t index)
{
    _S3M_LOADER *_Self = self;
    MUSMOD *track;
    MUSINSLIST *instruments;
    char *data;
    MUSINS *ins;
    uint16_t pages, h, dh;
    uint8_t i;
    uint32_t smpsize, memsize;
    uint16_t seg;
    void *loopstart;
    uint32_t loopsize;

    track = _Self->track;
    instruments = mod_Instruments;

    if (fsetpos(_Self->f, _Self->smppara[index] * 16))
    {
        DEBUG_ERR("s3mloader_load_sample", "Failed to read file.");
        _Self->err = E_S3M_FILE_SEEK;
        return false;
    }

    ins = musinsl_get (instruments, index);

    if (musins_is_looped(ins))
        smpsize = musins_get_loop_end(ins);
    else
        smpsize = musins_get_length(ins);

    memsize = _calc_sample_mem_size(smpsize);
    if (memsize > 0xffff)
    {
        DEBUG_ERR("s3mloader_load_sample", "Sample too large.");
        _Self->err = E_S3M_SAMPLE_SIZE;
        return false;
    }

    pages = (uint32_t)(memsize + 16 * 1024 - 1) / (16 * 1024);

    if (UseEMS && musinsl_is_EM_data (instruments) && (_Self->smp_EM_pages >= pages))
    {
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ (NULL, "sample=%02u, data=EM:%u-%u.", index, _Self->smp_EM_page, _Self->smp_EM_page + pages - 1);
        musins_set_EM_data(ins, true);
        musins_set_EM_data_page(ins, _Self->smp_EM_page);
        data = musins_map_EM_data(ins);
        if (!data)
        {
            DEBUG_ERR("s3mloader_load_sample", "Failed to map EM for sample.");
            _Self->err = E_S3M_EM_MAP;
            return false;
        }
        _Self->smp_EM_page += pages;
    }
    else
    {
        if (_dos_allocmem(_dos_para(memsize), &seg))
        {
            DEBUG_ERR("s3mloader_load_sample", "Failed to allocate DOS memory for sample data.");
            _Self->err = E_S3M_DOS_MEM_ALLOC;
            return false;
        }
        data = MK_FP(seg, 0);
        musins_set_EM_data(ins, false);
        musins_set_data(ins, data);
        if (DEBUG_FILE_S3M_LOAD)
            DEBUG_INFO_ ("s3mloader_load_sample", "sample=%02u, data=DOS:0x%04X:0x%04X.", index, FP_SEG (data), FP_OFF (data));
    }
    if (!fread(data, smpsize, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load_sample", "Failed to read sample data.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if (!_Self->signed_data)
        convert_sign_8(data, smpsize);

    if (musins_is_looped(ins))
    {
        h = memsize - smpsize;
        loopstart = &(data[musins_get_loop_start(ins)]);
        loopsize = musins_get_loop_end(ins) - musins_get_loop_start(ins);
        while (h)
        {
            if (h > loopsize)
                dh = loopsize;
            else
                dh = h;
            memcpy(&(data[memsize - h]), loopstart, dh);
            h -= dh;
        }
    }
    else
        memset(&(data[smpsize]), 128, memsize - smpsize);

    return true;
}

uint8_t __near getchtyp(uint8_t b)
{
    if (b <= 7)
        return 1;   // left
    if (b <= 15)
        return 2;   // right
    if (b <= 23)
        return 3;   // adlib melody
    if (b <= 31)
        return 4;   // adlib drums

    return 0;
}

MUSMOD *s3mloader_load (S3MLOADER *self, const char *name)
{
    _S3M_LOADER *_Self = self;
    S3MHEADER header;
    MUSMOD *track;
    MUSINSLIST *instruments;
    MUSPATLIST *patterns;
    uint8_t maxused;
    uint8_t i, smpnum;
    uint8_t chtype;
    void *p;
    char *par;
    MIXCHN *chn;
    uint16_t count;

    if ((!_Self) || (!name))
    {
        DEBUG_ERR ("s3mloader_load", "Bad arguments.");
        return NULL;
    }

    track = _new (MUSMOD);
    if (!track)
    {
        DEBUG_ERR ("s3mloader_load", "Failed to allocate memory for music module.");
        return NULL;
    }
    musmod_init (track);

    _Self->track = track;

    mod_Instruments = musinsl_new();
    if (!mod_Instruments)
    {
        DEBUG_ERR("s3mloader_load", "Failed to initialize instruments.");
        return NULL;
    }
    musinsl_init(mod_Instruments);

    mod_Patterns = _new(MUSPATLIST);
    if (!mod_Patterns)
    {
        DEBUG_ERR("s3mloader_load", "Failed to initialize patterns.");
        return NULL;
    }
    muspatl_init(mod_Patterns);

    instruments = mod_Instruments;
    patterns = mod_Patterns;

    UseEMS = UseEMS && emsInstalled && emsGetFreePagesCount();

    _Self->err = E_S3M_SUCCESS;
    _Self->f = fopen(name, "rb");
    if (!_Self->f)
    {
        DEBUG_ERR("s3mloader_load", "Failed to open file.");
        _Self->err = E_S3M_FILE_OPEN;
        return NULL;
    }

    if (!fread(&header, sizeof(S3MHEADER), 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read file''s header.");
        _Self->err = E_S3M_FILE_READ;
        return NULL;
    }

    if ((header.type != _S3M_FILE_TYPE)
    || (header.magic != _S3M_FILE_MAGIC)
    || ((header.tracker & (_S3M_TRACKER_TYPE_MASK | _S3M_TRACKER_VER_MAJOR_MASK)) !=
        ((_S3M_TRACKER_ST3 << _S3M_TRACKER_TYPE_SHIFT) | (_S3M_TRACKER_VER_MAJOR_ST3 << _S3M_TRACKER_VER_MAJOR_SHIFT)))
    || ((header.format != _S3M_FILE_FORMAT_1) && (header.format != _S3M_FILE_FORMAT_2)))
    {
        DEBUG_ERR("s3mloader_load", "Unsupported file format.");
        _Self->err = E_S3M_FILE_TYPE;
        return NULL;
    }

    snprintf (
        musmod_get_format (_Self->track),
        MOD_FORMAT_LEN,
        "Scream Tracker %hhx.%02hhx module",
        (header.tracker & _S3M_TRACKER_VER_MAJOR_MASK) >> _S3M_TRACKER_VER_MAJOR_SHIFT,
        (header.tracker & _S3M_TRACKER_VER_MINOR_MASK) >> _S3M_TRACKER_VER_MINOR_SHIFT
    );

    header.name[_S3M_TITLE_LEN - 1] = 0;
    musmod_set_title (_Self->track, header.name);

    musinsl_set_count (instruments, header.insnum);
    muspatl_set_count (patterns, header.patnum);
    musmod_set_order_length (track, header.ordnum);
    musmod_set_stereo (_Self->track, (header.mvolume & _S3M_MVOL_STEREO) != 0);
    musmod_set_amiga_limits (_Self->track, (header.flags & _S3M_FLAG_AMIGA_LIMITS) != 0);
    playState_gVolume   = header.gvolume;
    playState_mVolume   = (header.mvolume & _S3M_MVOL_MASK) >> _S3M_MVOL_SHIFT;
    initState_speed     = header.initialspeed;
    initState_tempo     = header.initialtempo;
    _Self->signed_data = (header.format == _S3M_FILE_FORMAT_1);

    maxused = 0;
    for (i = 0; i < 32; i++)
    {
        chn = &(mod_Channels[i]);
        chtype = getchtyp(header.channelset[i] & 31);
        if ((header.channelset[i] & 128) == 0)
        {
            if ((chtype > 0) && (chtype < 3))
                maxused = i + 1;
            mixchn_set_flags(chn, MIXCHNFL_ENABLED + MIXCHNFL_MIXING);
        }
        else
            mixchn_set_flags(chn, 0);
        mixchn_set_type(chn, chtype);
    }
    mod_ChannelsCount = maxused;
    if (DEBUG_FILE_S3M_LOAD)
        DEBUG_INFO_ (NULL, "Channels: %hu", mod_ChannelsCount);

    if (!fread(&Order, musmod_get_order_length (track), 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read patterns order.");
        _Self->err = E_S3M_FILE_READ;
        return NULL;
    }
    // check order if there's one 'real' (playable) entry ...
    i = 0;
    while ((i < musmod_get_order_length (track)) && (Order[i] >= 254))
        i++;

    if (i == musmod_get_order_length (track))
    {
        DEBUG_ERR("s3mloader_load", "Playable entry not found.");
        _Self->err = E_S3M_PATTERNS_ORDER;
        return NULL;
    }
    if (!fread(&(_Self->inspara), musinsl_get_count (instruments) * 2, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read instruments headers.");
        _Self->err = E_S3M_FILE_READ;
        return NULL;
    }

    if (!fread (&(_Self->patpara), muspatl_get_count (patterns) * 2, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read patterns offsets.");
        _Self->err = E_S3M_FILE_READ;
        return NULL;
    }

    if (!s3mloader_allocbuf(_Self))
    {
        DEBUG_ERR("s3mloader_load", "Failed to allocate DOS memory for buffer.");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return NULL;
    }

    s3mloader_alloc_patterns(_Self);

    count = muspatl_get_count (patterns);
    for (i = 0; i < count; i++)
        if (!s3mloader_load_pattern(_Self, i))
            return NULL;

    count = musinsl_get_count (instruments);
    for (i = 0; i < count; i++)
        if (!s3mloader_load_instrument(_Self, i))
            return NULL;

    if (UseEMS)
        s3mloader_alloc_samples(_Self);

    for (i = 0; i < count; i++)
        if (_Self->smppara[i])
            if (!s3mloader_load_sample(_Self, i))
                return NULL;

    if (musinsl_is_EM_data (instruments))
        musinsl_set_EM_handle_name (instruments);
    if (muspatl_is_EM_data (patterns))
        muspatl_set_EM_handle_name (patterns);

    s3mloader_dump_patterns (self);

    musmod_set_loaded (_Self->track, true);

    track = _Self->track;
    _Self->track = NULL;
    return track;
}

const char *s3mloader_get_error(S3MLOADER *self)
{
    _S3M_LOADER *_Self = self;
    uint16_t i;

    if (_Self)
    {
        if (_Self->err == E_S3M_SUCCESS)
            return NULL;

        i = 0;
        while (S3M_ERRORS[i].code != E_S3M_UNKNOWN)
        {
            if (S3M_ERRORS[i].code == _Self->err)
                return S3M_ERRORS[i].msg;
            i++;
        }

        return S3M_ERRORS[i].msg;
    }
    else
        return NULL;
}

void s3mloader_free(S3MLOADER *self)
{
    _S3M_LOADER *_Self = self;

    if (_Self)
    {
        if (_Self->buffer)
        {
            _dos_freemem(FP_SEG(_Self->buffer));
            _Self->buffer = NULL;
        }
        if (_Self->f)
        {
            fclose(_Self->f);
            _Self->f = NULL;
        }
    }
}

void s3mloader_delete(S3MLOADER **self)
{
    _S3M_LOADER **_Self = (_S3M_LOADER **) self;

    if (_Self)
        if (*_Self)
            __delete (self);
}
