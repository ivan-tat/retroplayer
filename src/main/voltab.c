/* voltab.c -- functions to handle volume table.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal.h"
#include "cc/string.h"
#include "cc/i86.h"
#include "cc/dos.h"
#ifdef DEBUG
# include "cc/stdio.h"
#endif

#include "main/voltab.h"

// TODO: remove PUBLIC_CODE macros when done.

void voltab_init(void)
{
    volumetableptr = NULL;
}

bool voltab_alloc(void)
{
    uint16_t seg;

    if (!_dos_allocmem(_dos_para(sizeof(voltab_t)), &seg))
    {
        volumetableptr = MK_FP(seg, 0);
        memset(volumetableptr, 0, sizeof(voltab_t));
        return true;
    }

    return false;
}

void voltab_calc(void)
{
    uint8_t vol;
    int16_t sample, *p = (int16_t *)volumetableptr;
    #ifdef DEBUG
    FILE *f;
    #endif

    for (vol = 1; vol <= 64; vol++)
        for (sample = 0; !(sample & 0x100); sample++)
        {
            *p = (((int16_t)((int8_t)(sample & 0xff))) * vol) << 2;
            p++;
        }

    if (DEBUG)
    {
        f = fopen ("_vol", "wb+");
        if (f)
        {
            fwrite (volumetableptr, sizeof (voltab_t), 1, f);
            fclose (f);
        }
    }
}

void voltab_free(void)
{
    if (volumetableptr)
    {
        _dos_freemem(FP_SEG(volumetableptr));
        volumetableptr = NULL;
    }
}
