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
#include "cc/dos.h"
#include "main/musmod.h"
#include "main/mixchn.h"
#include "main/musmodps.h"
#include "main/mixer.h"
#include "main/fillvars.h"

#if DEBUG_WRITE_OUTPUT_STREAM == 1

// write sound output streams to files (mixing buffer and DMA buffer)
#include "cc/stdio.h"
extern FILE *_debug_stream[2];

void __far DEBUG_open_output_streams (void);
void __far DEBUG_close_output_streams (void);

#endif  /* DEBUG_WRITE_OUTPUT_STREAM */

void __far fill_DMAbuffer (PLAYSTATE *ps, MIXER *mixer, SNDDMABUF *outbuf);

/*** Linking ***/

#ifdef __WATCOMC__

#if DEBUG_WRITE_OUTPUT_STREAM == 1
#pragma aux _debug_stream "*";
#pragma aux DEBUG_open_output_streams "*";
#pragma aux DEBUG_close_output_streams "*";
#endif

#pragma aux fill_DMAbuffer "*";

#endif  /* __WATCOMC__ */

#endif  /* FILLDMA_H */
