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

/*** General I/O ***/

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

/* Private */

extern bool     cc_gotbreak;
extern char     cc_lastkey;
extern uint16_t cc_screenwidth;
extern uint16_t cc_screenheight;
extern uint8_t  cc_textattrorig;

/* Publics */

extern bool     cc_checkbreak;
extern bool     cc_checkeof;
extern bool     cc_checksnow;
extern bool     cc_directvideo;
extern uint16_t cc_lastmode;
extern uint8_t  cc_textattr;
extern union text_rect_t cc_windmin;
extern union text_rect_t cc_windmax;

void cc_clreol(void);
void cc_clrscr(void);
void cc_gotoxy(uint8_t x, uint8_t y);
void cc_window(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
extern void PUBLIC_CODE pascal_textbackground(uint8_t color);
extern void PUBLIC_CODE pascal_textcolor(uint8_t color);
void cc_textmode(uint16_t mode);

#ifdef __WATCOMC__
#pragma aux pascal_textbackground modify [ax bx cx dx si di es];
#pragma aux pascal_textcolor      modify [ax bx cx dx si di es];
#endif

/*** Keyboard functions ***/

extern bool cc_kbhit(void);
extern char cc_getch(void);

/*** Initialization ***/

/* Private */

void _cc_console_set_mode(uint16_t mode);
void _cc_console_on_mode_change(void);
void _cc_console_on_start(void);

/* Publics */

void cc_console_init(void);

/* Aliases */

/*** General I/O ***/

#define inp cc_inp
#define outp cc_outp

/*** Text-mode variables ***/

#define checkbreak      cc_checkbreak;
#define checkeof        cc_checkeof;
#define checksnow       cc_checksnow;
#define directvideo     cc_directvideo;
#define lastmode        cc_lastmode;
#define textattr        cc_textattr;
#define windmin         cc_windmin;
#define windmax         cc_windmax;

/*** Text-mode functions ***/

#define textmode       cc_textmode
#define window         cc_window
#define clrscr         cc_clrscr
#define clreol         cc_clreol
#define gotoxy         cc_gotoxy
#define textbackground(c)       pascal_textbackground(c)
#define textcolor(c)            pascal_textcolor(c)

/*** Keyboard functions ***/

#define kbhit cc_kbhit
#define getch cc_getch

/*** Initialization ***/

#define console_init cc_console_init

/* Linking */

#ifdef __WATCOMC__
//#pragma aux cc_inp "*";
//#pragma aux cc_outp "*";

#pragma aux cc_gotbreak "*";
#pragma aux cc_lastkey "*";
#pragma aux cc_screenwidth "*";
#pragma aux cc_screenheight "*";
#pragma aux cc_textattrorig "*";

#pragma aux cc_checkbreak "*";
#pragma aux cc_checkeof "*";
#pragma aux cc_checksnow "*";
#pragma aux cc_directvideo "*";
#pragma aux cc_lastmode "*";
#pragma aux cc_textattr "*";
#pragma aux cc_windmin "*";
#pragma aux cc_windmax "*";

#pragma aux cc_textmode "*";
#pragma aux cc_window "*";
#pragma aux cc_clrscr "*";
#pragma aux cc_clreol "*";
#pragma aux cc_gotoxy "*";

#pragma aux cc_kbhit "*";
#pragma aux cc_getch "*";

#pragma aux _cc_console_on_mode_change "*";
#pragma aux _cc_console_set_mode "*";
#pragma aux _cc_console_on_start "*";
#pragma aux cc_console_init "*";

#endif

#endif  /* CC_CONIO_H */
