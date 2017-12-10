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

typedef int16_t voltab_t[64][256];

extern voltab_t *PUBLIC_DATA volumetableptr;

void voltab_init(void);
bool voltab_alloc(void);
void voltab_calc(void);
void voltab_free(void);

#ifdef __WATCOMC__
#pragma aux voltab_init "*";
#pragma aux voltab_alloc "*";
#pragma aux voltab_calc "*";
#pragma aux voltab_free "*";
#endif

#endif  /* VOLTAB_H */
