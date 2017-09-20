/* conio.h -- declarations for custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_CONIO_H
#define CC_CONIO_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal/pascal.h"

#ifdef __WATCOMC__

char cc_inp(unsigned int port);
#pragma aux cc_inp = \
    "in al,dx" \
    parm nomemory [dx] \
    value [al] \
    modify nomemory exact [];

void cc_outp(unsigned int port, char value);
#pragma aux cc_outp = \
    "out dx,al" \
    parm nomemory [dx] [al] \
    modify nomemory exact [];

#pragma intrinsic(cc_inp, cc_outp);

#else   /* !__WATCOMC__ */

extern char cc_inp(unsigned int port);
extern void cc_outp(unsigned int port, char value);

#endif  /* !__WATCOMC__ */

#define inp cc_inp
#define outp cc_outp

/* Aliases */

/* Keyboard functions */

#define kbhit() pascal_keypressed()
#define getch() pascal_readkey()

/* Text-mode functions */

#define _TEXTBW40 0
#define _TEXTC40  1
#define _TEXTBW80 2
#define _TEXTC80  3
#define _TEXTMONO 7
#define _Font8x8  256

#define _black          0
#define _blue           1
#define _green          2
#define _cyan           3
#define _red            4
#define _magenta        5
#define _brown          6
#define _lightgray      7
#define _darkgray       8
#define _lightblue      9
#define _lightgreen     10
#define _lightcyan      11
#define _lightred       12
#define _lightmagenta   13
#define _yellow         14
#define _white          15
#define _blink          128

extern uint16_t PUBLIC_DATA lastmode;
extern uint8_t  PUBLIC_DATA textattr;
extern uint16_t PUBLIC_DATA windmin;
extern uint16_t PUBLIC_DATA windmax;

#define clreol()                pascal_clreol()
#define clrscr()                pascal_clrscr()
#define gotoxy(x, y)            pascal_gotoxy(x, y)
#define textbackground(c)       pascal_textbackground(c)
#define textcolor(c)            pascal_textcolor(c)
#define textmode(m)             pascal_textmode(m)
#define window(x1, y1, x2, y2)  pascal_window(x1, y1, x2, y2)
#define window(x1, y1, x2, y2)  pascal_window(x1, y1, x2, y2)

#endif  /* CC_CONIO_H */
