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
#include "main/musmod.h"
#include "main/mixchn.h"

uint32_t getFreeDOSMemory(void);
uint32_t getFreeEMMMemory(void);

/* Information windows */

void get_note_name(char *__dest, uint8_t note);
uint8_t nextord(uint8_t nr);
uint8_t prevorder(uint8_t nr);

void __far plays3m_main (void);

/*** Initialization ***/

DECLARE_REGISTRATION (plays3m)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux getFreeDOSMemory "*";
#pragma aux getFreeEMMMemory "*";

#pragma aux get_note_name "*";
#pragma aux nextord "*";
#pragma aux prevorder "*";

#pragma aux plays3m_main "*";

#pragma aux register_plays3m "*";
#pragma aux unregister_plays3m "*";

#endif  /* __WATCOMC__ */

#endif  /* PLAYS3M_H */
