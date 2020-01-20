/* plays3m.h -- declarations for plays3m.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYS3M_H_INCLUDED
#define _PLAYS3M_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "main/musmod.h"
#include "main/mixchn.h"
#include "main/musmodps.h"

#if LINKER_TPC != 1
void __noreturn _start_c (void);
int main (int argc, const char **argv);
#endif /* LINKER_TPC != 1 */

uint32_t getFreeDOSMemory(void);
uint32_t getFreeEMMMemory(void);

/* Information windows */

void get_note_name(char *__dest, uint8_t note);

void __far plays3m_main (void);

/*** Initialization ***/

DECLARE_REGISTRATION (plays3m)

/*** Linking ***/

#ifdef __WATCOMC__

#if LINKER_TPC != 1
#pragma aux _start_c "*";
#pragma aux main "*";
#endif  /* LINKER_TPC != 1 */

#pragma aux getFreeDOSMemory "*";
#pragma aux getFreeEMMMemory "*";

#pragma aux get_note_name "*";

#pragma aux plays3m_main "*";

#pragma aux register_plays3m "*";
#pragma aux unregister_plays3m "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYS3M_H_INCLUDED */
