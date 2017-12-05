/* voltab.c -- functions to handle volume table.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal.h"
#include "cc/string.h"
#include "cc/i86.h"
#include "cc/dos.h"

#include "main/voltab.h"

// TODO: remove PUBLIC_CODE macros when done.

void PUBLIC_CODE initVolumeTable(void)
{
    volumetableptr = NULL;
}

bool PUBLIC_CODE allocVolumeTable(void)
{
    uint16_t seg;
    if (!_dos_allocmem(_dos_para(sizeof(voltab_t)), &seg))
    {
        volumetableptr = MK_FP(seg, 0);
        memset(volumetableptr, 0, sizeof(voltab_t));
        return true;
    }
    else
        return false;
}

void PUBLIC_CODE calcVolumeTable(void)
{
    uint8_t vol;
    uint16_t sample;
    int16_t *voltab = (int16_t *)volumetableptr;

    for (vol = 0; vol <= 64; vol++)
        for (sample = 0; sample <= 255; sample++)
        {
            *voltab = (int16_t)(vol * (int16_t)((int8_t)sample)) >> 6;
            voltab++;
        }
}

void PUBLIC_CODE freeVolumeTable(void)
{
    if (volumetableptr)
    {
        _dos_freemem(FP_SEG(volumetableptr));
        volumetableptr = NULL;
    }
}
