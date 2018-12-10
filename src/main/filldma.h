/* filldma.h -- declarations for filldma.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef FILLDMA_H
#define FILLDMA_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/mixer.h"
#include "main/fillvars.h"

// TODO: remove PUBLIC_CODE macros when done.

void PUBLIC_CODE fill_DMAbuffer(MIXBUF *mb, SNDDMABUF *outbuf);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux fill_DMAbuffer "*";

#endif  /* __WATCOMC__ */

#endif  /* FILLDMA_H */
