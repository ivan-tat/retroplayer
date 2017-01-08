/* readnote.h -- declarations for readnote.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef READNOTE_H
#define READNOTE_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

void PUBLIC_CODE readnewnotes(void);

#endif  /* READNOTE_H */
