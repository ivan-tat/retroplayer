/* startup.h -- declarations for startup.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef STARTUP_H
#define STARTUP_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

extern uint16_t PUBLIC_CODE pascal_paramcount(void);
extern void     PUBLIC_CODE pascal_paramstr(char *dest, uint8_t i);

#ifdef __WATCOMC__
#pragma aux pascal_paramcount modify [    bx cx dx si di es ];
#pragma aux pascal_paramstr   modify [ ax bx cx dx si di es ];
#endif

uint16_t PUBLIC_CODE custom_argc(void);
void     PUBLIC_CODE custom_argv(char *dest, uint16_t n, uint8_t i);

/*** Initialization ***/

bool PUBLIC_CODE custom_startup(void);

#endif  /* STARTUP_H */
