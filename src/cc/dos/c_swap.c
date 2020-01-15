/* c_swap.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$c_swap$*"
#endif

#include "cc/dos.h"

void cc_dos_swapvectors (void)
{
    unsigned i;
    void __far *p;

    for (i = 0; i < SAVEINTVEC_COUNT; i++)
    {
        p = _cc_dos_getvect (SaveIntVecIndexes [i]);
        _cc_dos_setvect (SaveIntVecIndexes [i], SaveIntVecs [i]);
        SaveIntVecs [i] = p;
    }
}
