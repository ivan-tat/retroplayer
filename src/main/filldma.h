/* filldma.h -- declarations for filldma.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef FILLDMA_H
#define FILLDMA_H 1

#include "defines.h"

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "../pascal/pascal.h"
#include "fillvars.h"

void PUBLIC_CODE fill_DMAbuffer(void *mixbuf, SNDDMABUF *outbuf);

#endif  /* FILLDMA_H */
