/* loads3m.c -- library for loading Scream Tracker 3.x music modules.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"
#include "..\ow\stdio.h"
#include "s3mtypes.h"
#include "s3mplay.h"
#include "loads3m.h"

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
