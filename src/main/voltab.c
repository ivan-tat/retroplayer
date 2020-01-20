/* voltab.c -- functions to handle volume table.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$voltab$*"
#endif  /* __WATCOMC__ */

#include "pascal.h"
#include "cc/string.h"
#include "cc/i86.h"
#include "cc/dos.h"
#if DEBUG == 1
# include "cc/stdio.h"
#endif  /* DEBUG == 1 */
#include "main/voltab.h"

#if DEFINE_LOCAL_DATA == 1

voltab_t *volumetableptr;

#endif  /* DEFINE_LOCAL_DATA == 1 */

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
    #if DEBUG == 1
    FILE *f;
    #endif  /* DEBUG == 1 */

    // MSB (signed)
    for (vol = 1; vol <= 64; vol++)
        for (sample = 0; !(sample & 0x100); sample++)
        {
            *p = (((int16_t)((int8_t)(sample & 0xff))) * vol) << 2;
            p++;
        }

    // LSB (unsigned)
    for (vol = 1; vol <= 64; vol++)
        for (sample = 0; !(sample & 0x100); sample++)
        {
            *p = (int16_t)(sample * vol) >> 6;
            p++;
        }

    #if DEBUG == 1
    f = fopen ("_vol0", "wb+");
    if (f)
    {
        fwrite (volumetableptr, sizeof (voltab_t) / 2, 1, f);
        fclose (f);
    }
    f = fopen ("_vol1", "wb+");
    if (f)
    {
        fwrite ((uint8_t *)volumetableptr + sizeof (voltab_t) / 2, sizeof (voltab_t) / 2, 1, f);
        fclose (f);
    }
    #endif  /* DEBUG == 1 */
}

void voltab_free(void)
{
    if (volumetableptr)
    {
        _dos_freemem(FP_SEG(volumetableptr));
        volumetableptr = NULL;
    }
}
