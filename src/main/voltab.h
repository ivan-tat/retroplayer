/* voltab.h -- declarations for voltab.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef VOLTAB_H
#define VOLTAB_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

// TODO: remove PUBLIC_DATA/PUBLIC_CODE macros when done.

typedef int16_t voltab_t[65][256];

extern voltab_t *PUBLIC_DATA volumetableptr;

void PUBLIC_CODE initVolumeTable(void);
bool PUBLIC_CODE allocVolumeTable(void);
void PUBLIC_CODE calcVolumeTable(bool sign);
void PUBLIC_CODE freeVolumeTable(void);

#endif  /* VOLTAB_H */
