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
#include "main/effects.h"
#include "main/s3mtypes.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"
#include "main/loads3m.h"

#define S3M_MAX_INSTRUMENTS 99
#define S3M_MAX_PATTERNS 100

#define LOADER_BUF_SIZE (10 * 1024)

/*** File header ***/

#define S3M_TITLE_LENGTH 28

#pragma pack(push, 1);
typedef struct S3M_header
{
    char name[S3M_TITLE_LENGTH];
    char charEOF;       // = 0x1A
    uint8_t type;       // = 0x10
    uint8_t unused1[2];
    uint16_t ordnum;
    uint16_t insnum;
    uint16_t patnum;
    uint16_t flags;
    uint16_t cwtv;      // Created With Tracker Version
        // bit  12    = always 1 -> created with Scream Tracker;
        // bits 11..8 = major tracker version;
        //       7..0 = minor tracker version.
    uint16_t ffv;       // file format version
    uint32_t magic;     // = "SCRM"
    uint8_t gvolume;    // global volume
    uint8_t initialspeed;
    uint8_t initialtempo;
    uint8_t mvolume;    // master volume
    uint8_t unused2[12];
    uint8_t channelset[32];
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

typedef struct S3M_loader_t
{
    S3MERR err;
    FILE *f;
    char *buffer;
    bool signed_data;
    uint16_t inspara[S3M_MAX_INSTRUMENTS];
    uint16_t patpara[S3M_MAX_PATTERNS];
    uint32_t smppara[S3M_MAX_INSTRUMENTS];
    uint16_t pat_EM_pages;
    uint16_t pat_EM_page;
    uint16_t pat_EM_page_offset;
    uint16_t smp_EM_pages;
    uint16_t smp_EM_page;
};

void *s3mloader_new(void)
{
    uint16_t seg;

    if (!_dos_allocmem(_dos_para(sizeof(struct S3M_loader_t)), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void s3mloader_init(S3MLOADER *self)
{
    struct S3M_loader_t *_Self = self;

    if (_Self)
        memset(_Self, 0, sizeof(struct S3M_loader_t));
}

bool __near s3mloader_allocbuf(S3MLOADER *self)
{
    struct S3M_loader_t *_Self = self;
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
    struct S3M_loader_t *_Self = self;
    uint16_t patsize;
    uint16_t patperpage;
    uint16_t freepages;
    EMSHDL handle;

    patsize = UsedChannels * 64 * 5;
    #ifdef DEBUGLOAD
    DEBUG_INFO_(NULL, "Pattern memory size: %u.", patsize);
    #endif

    if (UseEMS)
    {
        // let's continue with loading:
        patperpage = 16 * 1024 / patsize;
        #ifdef DEBUGLOAD
        DEBUG_INFO_(NULL, "Patterns per EM page: %u.", patperpage);
        #endif
        _Self->pat_EM_pages = (muspatl_get_count(mod_Patterns) + patperpage - 1) / patperpage;

        freepages = emsGetFreePagesCount();
        if (_Self->pat_EM_pages > freepages)
            _Self->pat_EM_pages = freepages;

        handle = emsAlloc(_Self->pat_EM_pages);
        if (emsEC != E_EMS_SUCCESS)
        {
            DEBUG_ERR("s3mloader_load", "Failed to allocate EM for patterns.");
            muspatl_set_EM_data(mod_Patterns, false);
            return;
        }

        muspatl_set_EM_data(mod_Patterns, true);
        muspatl_set_own_EM_handle(mod_Patterns, true);
        muspatl_set_EM_handle(mod_Patterns, handle);

        _Self->pat_EM_page_offset = 0;
        _Self->pat_EM_page = 0;
    }
}

void __near _unpack_pattern(uint8_t *src, uint8_t *dst, uint8_t maxrow, uint8_t maxchn)
{
    unsigned int i, row;
    unsigned char chn;
    uint8_t *inbuf;
    uint8_t *outbuf;
    unsigned char a;
    bool read;

    /* clear pattern */
    outbuf = dst;
    for (i = maxrow * maxchn - 1; i; i--)
    {
        outbuf[0] = CHNNOTE_EMPTY;    /* note */
        outbuf[1] = 0;                /* instrument */
        outbuf[2] = CHNINSVOL_EMPTY;  /* volume */
        outbuf[3] = EFFIDX_NONE;      /* command */
        outbuf[4] = 0;                /* parameters */
        outbuf += 5;
    };

    row = 0;
    inbuf = src;
    while (row < maxrow)
    {
        a = inbuf[0];
        inbuf++;
        if (!a)
            row++;
        else
        {
            chn = a & S3MPATFL_CHNMASK;
            read = chn < maxchn;
            if (read)
                outbuf = dst + (row * maxchn + chn) * 5;
            if (a & S3MPATFL_NOTE_INS)
            {
                if (read)
                {
                    outbuf[0] = inbuf[0];
                    outbuf[1] = inbuf[1];
                };
                inbuf += 2;
            };
            if (a & S3MPATFL_VOL)
            {
                if (read)
                    outbuf[2] = inbuf[0];
                inbuf++;
            };
            if (a & S3MPATFL_CMD_PARM)
            {
                if (read)
                {
                    outbuf[3] = inbuf[0];
                    outbuf[4] = inbuf[1];
                };
                inbuf += 2;
            };
        };
    };
}

bool __near s3mloader_load_pattern(S3MLOADER *self, uint8_t index)
{
    struct S3M_loader_t *_Self = self;
    uint32_t pos;
    uint16_t length;
    MUSPAT pat_static;
    MUSPAT *pat;
    uint16_t seg;
    void *p;
    bool em;

    pos = _Self->patpara[index] * 16;

    if (!pos)
    {
        /*
        pat = muspatl_get(mod_Patterns, index);
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

    if ((!length) || (length > LOADER_BUF_SIZE))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Bad pattern size.");
        _Self->err = E_S3M_PATTERN_SIZE;
        return false;
    }
    if (!fread(_Self->buffer, length - 2, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Failed to read pattern.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    pat = &pat_static;
    muspat_init(pat);
    muspat_set_channels(pat, UsedChannels);
    muspat_set_rows(pat, 64);
    muspat_set_size(pat, muspat_get_channels(pat) * muspat_get_rows(pat) * 5);

    // try to put in EM
    em = false;
    if (muspatl_is_EM_data(mod_Patterns) && _Self->pat_EM_pages)
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
        muspat_set_EM_data_handle(pat, muspatl_get_EM_handle(mod_Patterns));
        muspat_set_EM_data_page(pat, _Self->pat_EM_page);
        muspat_set_EM_data_offset(pat, _Self->pat_EM_page_offset);
        muspatl_set(mod_Patterns, index, pat);
        p = muspat_map_EM_data(pat);
        if (!p)
        {
            DEBUG_ERR("s3mloader_load_pattern", "Failed to map EM for pattern.");
            _Self->err = E_S3M_EM_MAP;
            return false;
        }
        _Self->pat_EM_page_offset += muspat_get_size(pat);
    }
    else
    {
        if (_dos_allocmem(_dos_para(muspat_get_size(pat)), &seg))
        {
            DEBUG_ERR("s3mloader_load_pattern", "Failed to allocate DOS memory for pattern.");
            _Self->err = E_S3M_DOS_MEM_ALLOC;
            return false;
        }
        p = MK_FP(seg, 0);
        muspat_set_EM_data(pat, false);
        muspat_set_data(pat, p);
        muspatl_set(mod_Patterns, index, pat);
    }

    #ifdef DEBUGLOAD
    if (muspat_is_EM_data(pat))
        DEBUG_MSG_("s3mloader_load_pattern", "p=%hu, r/s=%u/%u, EM=%04X:%04X",
            index, length, muspat_get_size(pat), muspat_get_EM_data_page(pat), muspat_get_EM_data_offset(pat));
    else
        DEBUG_MSG_("s3mloader_load_pattern", "p=%hu, r/s=%u/%u, DOS=%04X:%04X",
            index, length, muspat_get_size(pat), muspat_get_data(pat));
    #endif

    _unpack_pattern(_Self->buffer, p, muspat_get_rows(pat), muspat_get_channels(pat));

    return true;
}

bool __near s3mloader_load_instrument(S3MLOADER *self, uint8_t index)
{
    struct S3M_loader_t *_Self = self;
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

    ins = musinsl_get(mod_Instruments, index);
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
    struct S3M_loader_t *_Self = self;
    uint16_t pages;
    uint32_t memsize;
    int16_t i;
    MUSINS *ins;
    EMSHDL handle;

    if (!emsGetFreePagesCount())
    {
        DEBUG_WARN("s3mloader_alloc_samples", "Not enough EM for samples.");
        musinsl_set_EM_data(mod_Instruments, false);
        return;
    }

    pages = 0;
    for (i = 0; i < S3M_MAX_INSTRUMENTS; i++)
    {
        ins = musinsl_get(mod_Instruments, i);
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

    #ifdef DEBUGLOAD
    DEBUG_INFO_(NULL, "Instruments to load: %u.", InsNum);
    DEBUG_INFO_(NULL, "EM pages are needed for samples: %u.", pages);
    #endif

    if (pages > emsGetFreePagesCount())
        pages = emsGetFreePagesCount();

    handle = emsAlloc(pages);
    if (emsEC != E_EMS_SUCCESS)
    {
        DEBUG_ERR("s3mloader_alloc_samples", "Failed to allocate EM for samples.");
        musinsl_set_EM_data(mod_Instruments, false);
        return;
    }
    musinsl_set_EM_data(mod_Instruments, true);
    musinsl_set_EM_data_handle(mod_Instruments, handle);

    _Self->smp_EM_pages = pages;
    _Self->smp_EM_page = 0;

    #ifdef DEBUGLOAD
    DEBUG_INFO_(NULL, "EM pages allocated for samples: %u.", pages);
    #endif
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
    struct S3M_loader_t *_Self = self;
    char *data;
    MUSINS *ins;
    uint16_t pages, h, dh;
    uint8_t i;
    uint32_t smpsize, memsize;
    uint16_t seg;
    void *loopstart;
    uint32_t loopsize;

    if (fsetpos(_Self->f, _Self->smppara[index] * 16))
    {
        DEBUG_ERR("s3mloader_load_sample", "Failed to read file.");
        _Self->err = E_S3M_FILE_SEEK;
        return false;
    }

    ins = musinsl_get(mod_Instruments, index);

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

    if (UseEMS && musinsl_is_EM_data(mod_Instruments) && (_Self->smp_EM_pages >= pages))
    {
        #ifdef DEBUGLOAD
        DEBUG_INFO_(NULL, "sample=%02u, data=EM:%u-%u.", index, _Self->smp_EM_page, _Self->smp_EM_page + pages - 1);
        #endif
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
        #ifdef DEBUGLOAD
        DEBUG_INFO_(NULL, "sample=%02u, data=DOS:0x%04X0.", index, seg);
        #endif
        data = MK_FP(seg, 0);
        musins_set_EM_data(ins, false);
        musins_set_data(ins, data);
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

bool s3mloader_load(S3MLOADER *self, const char *name)
{
    struct S3M_loader_t *_Self = self;
    S3MHEADER header;
    uint8_t maxused;
    uint8_t i, smpnum;
    uint8_t chtype;
    void *p;
    char *par;
    MIXCHN *chn;
    uint16_t count;

    mod_Instruments = musinsl_new();
    if (!mod_Instruments)
    {
        DEBUG_ERR("s3mloader_load", "Failed to initialize instruments.");
        return false;
    };
    musinsl_init(mod_Instruments);

    mod_Patterns = _new(MUSPATLIST);
    if (!mod_Patterns)
    {
        DEBUG_ERR("s3mloader_load", "Failed to initialize patterns.");
        return false;
    }
    muspatl_init(mod_Patterns);

    UseEMS = UseEMS && emsInstalled && emsGetFreePagesCount();

    _Self->err = E_S3M_SUCCESS;
    _Self->f = fopen(name, "rb");
    if (!_Self->f)
    {
        DEBUG_ERR("s3mloader_load", "Failed to open file.");
        _Self->err = E_S3M_FILE_OPEN;
        return false;
    }

    if (!fread(&header, sizeof(S3MHEADER), 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read file''s header.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if ((header.type != 16)
    || (header.magic != 0x4d524353)
    || (((header.cwtv >> 8) & 0xff) != 0x13)
    || ((header.ffv != 1) && (header.ffv != 2)))
    {
        DEBUG_ERR("s3mloader_load", "Unsupported file format.");
        _Self->err = E_S3M_FILE_TYPE;
        return false;
    }

    memset(mod_TrackerName, 0, MOD_MAX_TRACKER_NAME_LENGTH);
    snprintf(
        mod_TrackerName,
        MOD_MAX_TRACKER_NAME_LENGTH,
        "Scream Tracker %c.%c%c module",
        '0' + ((header.cwtv >> 8) & 0x0f),
        '0' + ((header.cwtv >> 4) & 0x0f),
        '0' + (header.cwtv & 0x0f)
    );

    memset(mod_Title, 0, MOD_MAX_TITLE_LENGTH);
    strncpy(mod_Title, header.name,
        S3M_TITLE_LENGTH > MOD_MAX_TITLE_LENGTH ? MOD_MAX_TITLE_LENGTH : S3M_TITLE_LENGTH);
    mod_Title[MOD_MAX_TITLE_LENGTH - 1] = 0;

    OrdNum = header.ordnum;
    InsNum = header.insnum;
    muspatl_set_count(mod_Patterns, header.patnum);
    modOption_ST2Vibrato   = (header.flags & 0x01) != 0;
    modOption_ST2Tempo     = (header.flags & 0x02) != 0;
    modOption_AmigaSlides  = (header.flags & 0x04) != 0;
    modOption_VolZeroOptim = (header.flags & 0x08) != 0;
    modOption_AmigaLimits  = (header.flags & 0x10) != 0;
    modOption_SBfilter     = (header.flags & 0x20) != 0;
    modOption_CostumeFlag  = (header.flags & 0x80) != 0;
    modOption_Stereo       = ((header.mvolume & 0x80) != 0);
    playState_gVolume   = header.gvolume;
    playState_mVolume   = header.mvolume & 0x7f;
    initState_speed     = header.initialspeed;
    initState_tempo     = header.initialtempo;
    _Self->signed_data   = (header.ffv == 1);

    maxused = 0;
    for (i = 0; i < 32; i++)
    {
        chn = &(Channel[i]);
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
    UsedChannels = maxused;
    #ifdef DEBUGLOAD
    DEBUG_INFO_(NULL, "Channels: %hu", UsedChannels);
    #endif

    if (!fread(&Order, OrdNum, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read patterns order.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }
    // check order if there's one 'real' (playable) entry ...
    i = 0;
    while ((i < OrdNum) && (Order[i] >= 254))
        i++;

    if (i == OrdNum)
    {
        DEBUG_ERR("s3mloader_load", "Playable entry not found.");
        _Self->err = E_S3M_PATTERNS_ORDER;
        return false;
    }
    if (!fread(&(_Self->inspara), InsNum * 2, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read instruments headers.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if (!fread(&(_Self->patpara), muspatl_get_count(mod_Patterns) * 2, 1, _Self->f))
    {
        DEBUG_ERR("s3mloader_load", "Failed to read patterns offsets.");
        _Self->err = E_S3M_FILE_READ;
        return false;
    }

    if (!s3mloader_allocbuf(_Self))
    {
        DEBUG_ERR("s3mloader_load", "Failed to allocate DOS memory for buffer.");
        _Self->err = E_S3M_DOS_MEM_ALLOC;
        return false;
    }

    s3mloader_alloc_patterns(_Self);

    count = muspatl_get_count(mod_Patterns);
    for (i = 0; i < count; i++)
        if (!s3mloader_load_pattern(_Self, i))
            return false;

    count = InsNum;
    for (i = 0; i < count; i++)
        if (!s3mloader_load_instrument(_Self, i))
            return false;

    if (UseEMS)
        s3mloader_alloc_samples(_Self);

    for (i = 0; i < count; i++)
        if (_Self->smppara[i])
            if (!s3mloader_load_sample(_Self, i))
                return false;

    if (musinsl_is_EM_data(mod_Instruments))
        musinsl_set_EM_handle_name(mod_Instruments);
    if (muspatl_is_EM_data(mod_Patterns))
        muspatl_set_EM_handle_name(mod_Patterns);

    mod_isLoaded = true;

    return true;
}

const char *s3mloader_get_error(S3MLOADER *self)
{
    struct S3M_loader_t *_Self = self;
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
    struct S3M_loader_t *_Self = self;

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
    struct S3M_loader_t **_Self = (struct S3M_loader_t **)self;

    if (_Self)
        if (*_Self)
        {
            _dos_freemem(FP_SEG(*_Self));
            *_Self = NULL;
        }
}
