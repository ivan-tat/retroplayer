/* voltab.h -- declarations for voltab.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef VOLTAB_H
#define VOLTAB_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_DATA/PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

typedef int16_t voltab_t[65][256];

extern voltab_t *PUBLIC_DATA volumetableptr;

void PUBLIC_CODE initVolumeTable(void);
bool PUBLIC_CODE allocVolumeTable(void);
void PUBLIC_CODE calcVolumeTable(bool sign);
void PUBLIC_CODE freeVolumeTable(void);

#endif  /* VOLTAB_H */
