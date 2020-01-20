/* c_init.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$c_init$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/dos.h"

#if DEFINE_LOCAL_DATA == 1

const uint8_t SaveIntVecIndexes [SAVEINTVEC_COUNT] =
{
    0x00, 0x02, 0x1B, 0x21, 0x23, 0x24, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x75
};
void __far *SaveIntVecs [SAVEINTVEC_COUNT] = { 0 };

#endif  /* DEFINE_LOCAL_DATA == 1 */

void cc_dos_savevectors (void)
{
    unsigned i;

    for (i = 0; i < SAVEINTVEC_COUNT; i++)
        SaveIntVecs [i] = _cc_dos_getvect (SaveIntVecIndexes [i]);
}

void cc_dos_restorevectors (void)
{
    unsigned i;

    for (i = 0; i < SAVEINTVEC_COUNT; i++)
        _cc_dos_setvect (SaveIntVecIndexes [i], SaveIntVecs [i]);
}
