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

#include "pascal.h"

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

/*** Keyboard functions ***/

extern bool cc_kbhit(void);
extern char cc_getch(void);

/*** Text-mode functions ***/

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

#pragma pack(push, 1);
typedef union text_rect_t {
    struct {
        uint8_t x, y;
    } rect;
    uint16_t value;
};
#pragma pack(pop);

extern uint16_t PUBLIC_DATA lastmode;
extern uint8_t  PUBLIC_DATA textattr;
extern union text_rect_t PUBLIC_DATA windmin;
extern union text_rect_t PUBLIC_DATA windmax;

void cc_clreol(void);
void cc_clrscr(void);
void cc_gotoxy(uint8_t x, uint8_t y);
extern void PUBLIC_CODE pascal_textbackground(uint8_t color);
extern void PUBLIC_CODE pascal_textcolor(uint8_t color);
extern void PUBLIC_CODE pascal_textmode(uint16_t mode);
extern void PUBLIC_CODE pascal_window(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

#ifdef __WATCOMC__
#pragma aux pascal_textbackground modify [ax bx cx dx si di es];
#pragma aux pascal_textcolor      modify [ax bx cx dx si di es];
#pragma aux pascal_textmode       modify [ax bx cx dx si di es];
#pragma aux pascal_window         modify [ax bx cx dx si di es];
#endif

/* Aliases */

#define inp cc_inp
#define outp cc_outp

/*** Keyboard functions ***/

#define kbhit cc_kbhit
#define getch cc_getch

/*** Text-mode functions ***/

#define clreol                  cc_clreol
#define clrscr                  cc_clrscr
#define gotoxy                  cc_gotoxy
#define textbackground(c)       pascal_textbackground(c)
#define textcolor(c)            pascal_textcolor(c)
#define textmode(m)             pascal_textmode(m)
#define window(x1, y1, x2, y2)  pascal_window(x1, y1, x2, y2)

/* Linking */

#ifdef __WATCOMC__
//#pragma aux cc_inp "*";
//#pragma aux cc_outp "*";
#pragma aux cc_kbhit "*";
#pragma aux cc_getch "*";
#pragma aux cc_clreol "*";
#pragma aux cc_clrscr "*";
#pragma aux cc_gotoxy "*";
#endif

#endif  /* CC_CONIO_H */
