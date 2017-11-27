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

#define DEVSEL_AUTO     1
#define DEVSEL_ENV      2
#define DEVSEL_MANUAL   3

extern bool     PUBLIC_DATA opt_help;
extern char     PUBLIC_DATA opt_filename[256];
extern char     PUBLIC_DATA opt_filename_c[256];
extern uint8_t  PUBLIC_DATA opt_devselect;
extern uint16_t PUBLIC_DATA opt_mode_rate;
extern bool     PUBLIC_DATA opt_mode_stereo;
extern bool     PUBLIC_DATA opt_mode_16bits;
extern bool     PUBLIC_DATA opt_mode_lq;
extern bool     PUBLIC_DATA opt_dumpconf;
extern uint8_t  PUBLIC_DATA opt_mvolume;
extern bool     PUBLIC_DATA opt_st3order;
extern uint8_t  PUBLIC_DATA opt_startpos;
extern bool     PUBLIC_DATA opt_loop;
extern bool     PUBLIC_DATA opt_em;
extern uint8_t  PUBLIC_DATA opt_fps;

/* Information windows */

extern uint8_t PUBLIC_DATA chnlastinst[MAX_CHANNELS];

uint32_t PUBLIC_CODE getFreeDOSMemory(void);
uint32_t PUBLIC_CODE getFreeEMMMemory(void);
void     PUBLIC_CODE memstats(void);

void __near get_note_name(char *__dest, uint8_t note);
void __near __pascal write_Note(uint8_t note);
uint8_t __near __pascal prevorder(uint8_t nr);

void PUBLIC_CODE plays3m_main(void);

/*** Initialization ***/

DECLARE_REGISTRATION(plays3m)

#endif  /* PLAYS3M_H */
