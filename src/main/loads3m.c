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
#include "main/s3mtypes.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"
#include "main/loads3m.h"

S3MLOADER *PUBLIC_CODE s3mloader_new(void)
{
    uint16_t seg;

    if (!_dos_allocmem(_dos_para(sizeof(S3MLOADER)), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void PUBLIC_CODE s3mloader_clear(S3MLOADER *self)
{
    if (self)
        memset(self, 0, sizeof(S3MLOADER));
}

void PUBLIC_CODE s3mloader_delete(S3MLOADER **self)
{
    if (self)
        if (*self)
        {
            _dos_freemem(FP_SEG(*self));
            *self = NULL;
        }
}

bool PUBLIC_CODE s3mloader_allocbuf(S3MLOADER *self)
{
    uint16_t seg;

    if (self)
        if (!self->buffer)
        {
            if (!_dos_allocmem(_dos_para(10*1024), &seg))
            {
                self->buffer = MK_FP(seg, 0);
                return true;
            }
        }
        else
            return true;

    return false;
}

void PUBLIC_CODE s3mloader_free(S3MLOADER *self)
{
    if (self)
    {
        if (self->buffer)
        {
            _dos_freemem(FP_SEG(self->buffer));
            self->buffer = NULL;
        }
        fclose(&(self->f));
    }
}

/* Packed S3M pattern lead byte flags */
#define S3MPATFL_CHNMASK  0x1f
#define S3MPATFL_NOTE_INS 0x20
#define S3MPATFL_VOL      0x40
#define S3MPATFL_CMD_PARM 0x80

void PUBLIC_CODE unpackPattern(uint8_t *src, uint8_t *dst, uint8_t maxrow, uint8_t maxchn)
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

#define noerror              0
#define notenoughmem         -1
#define wrongformat          -2
#define filecorrupt          -3
#define filenotexist         -4
#define packedsamples        -5
#define Allreadyallocbuffers -6
#define nota386orhigher      -7
#define nosounddevice        -8
#define noS3Minmemory        -9
#define ordercorrupt         -10
#define internal_failure     -11
#define sample2large         -12

bool PUBLIC_CODE s3mloader_load_pattern(S3MLOADER *self, uint8_t index, uint32_t pos)
{
    uint16_t length;
    MUSPAT pat_static;
    MUSPAT *pat;
    uint16_t seg;
    void *p;

    if (!pos)
    {
        pat = patList_get(mod_Patterns, index);
        muspat_clear(pat);
        return true;
    }

    if (fsetpos(&(self->f), pos))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Failed to read file.");
        self->err = filecorrupt;
        return false;
    }
    if (!fread(&length, 2, 1, &(self->f)))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Failed to read pattern.");
        self->err = filecorrupt;
        return false;
    }
    #ifdef DEBUGLOAD
    printf("P%hu:%u", index, length);
    #endif
    if ((!length) || (length > 10 * 1024))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Bad pattern size.");
        self->err = filecorrupt;
        return false;
    }
    if (!fread(self->buffer, length - 2, 1, &(self->f)))
    {
        DEBUG_ERR("s3mloader_load_pattern", "Failed to read pattern.");
        self->err = filecorrupt;
        return false;
    }

    pat = &pat_static;
    muspat_clear(pat);
    muspat_set_channels(pat, UsedChannels);
    muspat_set_rows(pat, 64);

    if (UseEMS
    && patListIsInEM(mod_Patterns)
    && (self->pat_EM_page_offset < patListGetPatPerPage(mod_Patterns)))
    {
        muspat_set_EM_data(pat, true);
        muspat_set_own_EM_handle(pat, false);
        muspat_set_EM_data_handle(pat, patListGetHandle(mod_Patterns));
        muspat_set_EM_data_page(pat, self->pat_EM_page);
        muspat_set_EM_data_offset(pat, self->pat_EM_page_offset * 64 * 5 * UsedChannels);
        patList_set(mod_Patterns, index, pat);
        p = muspat_map_EM_data(pat);
        if (!p)
        {
            DEBUG_ERR("s3mloader_load_pattern", "Failed to map EM for pattern.");
            self->err = internal_failure;
            return false;
        }
    }
    else
    {
        if (_dos_allocmem(_dos_para(64 * 5 * UsedChannels), &seg))
        {
            DEBUG_ERR("s3mloader_load_pattern", "Failed to allocate DOS memory for pattern.");
            self->err = notenoughmem;
            return false;
        }
        p = MK_FP(seg, 0);
        muspat_set_EM_data(pat, false);
        muspat_set_data(pat, p);
        patList_set(mod_Patterns, index, pat);
    }

    unpackPattern(self->buffer, p, 64, UsedChannels);

    pat = patList_get(mod_Patterns, index);

    if (muspat_is_EM_data(pat))
    {
        #ifdef DEBUGLOAD
        printf("E(%04X:%04X)", self->pat_EM_page, self->pat_EM_page_offset);
        #endif
        /* next position in EM: */
        self->pat_EM_page_offset++;
        if ((self->pat_EM_page_offset == patListGetPatPerPage(mod_Patterns)) && self->pat_EM_pages)
        {
            self->pat_EM_pages--;
            self->pat_EM_page++;
            self->pat_EM_page_offset = 0;
        }
    }

    return true;
}
