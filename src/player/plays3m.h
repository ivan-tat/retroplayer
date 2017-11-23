/* plays3m.h -- declarations for plays3m.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PLAYS3M_H
#define PLAYS3M_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/mixchn.h"

/* Program's options */

extern bool PUBLIC_DATA opt_help;

/* Information windows */

extern uint8_t PUBLIC_DATA chnlastinst[MAX_CHANNELS];

uint32_t PUBLIC_CODE getFreeDOSMemory(void);
uint32_t PUBLIC_CODE getFreeEMMMemory(void);

void __near get_note_name(char *__dest, uint8_t note);
void __near __pascal write_Note(uint8_t note);
uint8_t __near __pascal prevorder(uint8_t nr);

#endif  /* PLAYS3M_H */
