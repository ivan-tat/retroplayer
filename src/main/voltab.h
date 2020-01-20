/* voltab.h -- declarations for voltab.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _VOLTAB_H_INCLUDED
#define _VOLTAB_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

typedef int16_t voltab_t[2][64][256];

extern voltab_t *volumetableptr;

void voltab_init(void);
bool voltab_alloc(void);
void voltab_calc(void);
void voltab_free(void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux volumetableptr "*";

#pragma aux voltab_init "*";
#pragma aux voltab_alloc "*";
#pragma aux voltab_calc "*";
#pragma aux voltab_free "*";

#endif  /* __WATCOMC__ */

#endif  /* !_VOLTAB_H_INCLUDED */
