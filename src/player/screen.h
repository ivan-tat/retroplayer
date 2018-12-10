/* screen.h -- declarations for screen.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef SCREEN_H
#define SCREEN_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

/* Screen window */

#define scrWidth 80
#define scrHeight 25

#pragma pack(push, 1);
typedef struct screen_rect_t
{
    uint8_t x0, y0, x1, y1;
};
#pragma pack(pop);
typedef struct screen_rect_t SCRRECT;

typedef uint16_t WINFLAGS;

// state
#define WINFL_VISIBLE     (1<<0)
#define WINFL_FOCUSED     (1<<1)
// events
#define WINFL_FULLREDRAW  (1<<2)
#define WINFL_FOCUSREDRAW (1<<3)
#define WINFL_REDRAW      (WINFL_FULLREDRAW | WINFL_FOCUSREDRAW)

typedef struct screen_window_t SCRWIN;
#pragma pack(push, 1);
typedef struct screen_window_t
{
    WINFLAGS flags;
    SCRRECT rect;
    void __far (*draw)(SCRWIN *self);
    bool __far (*keypress)(SCRWIN *self, char key);
};
#pragma pack(pop);

void     PUBLIC_CODE scrwin_init(
    SCRWIN *self,
    uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
    void *draw,
    void *keypress
);
bool     PUBLIC_CODE scrwin_is_created(SCRWIN *self);
void     PUBLIC_CODE scrwin_set_flags(SCRWIN *self, WINFLAGS value);
WINFLAGS PUBLIC_CODE scrwin_get_flags(SCRWIN *self);
void     PUBLIC_CODE scrwin_set_width(SCRWIN *self, uint8_t value);
uint8_t  PUBLIC_CODE scrwin_get_width(SCRWIN *self);
void     PUBLIC_CODE scrwin_set_height(SCRWIN *self, uint8_t value);
uint8_t  PUBLIC_CODE scrwin_get_height(SCRWIN *self);
void     PUBLIC_CODE scrwin_draw(SCRWIN *self);
bool     PUBLIC_CODE scrwin_keypress(SCRWIN *self, char key);
void     PUBLIC_CODE scrwin_show(SCRWIN *self);
void     PUBLIC_CODE scrwin_focus(SCRWIN *self);
void     PUBLIC_CODE scrwin_leave(SCRWIN *self);
void     PUBLIC_CODE scrwin_close(SCRWIN *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux scrwin_init "*";
#pragma aux scrwin_is_created "*";
#pragma aux scrwin_set_flags "*";
#pragma aux scrwin_get_flags "*";
#pragma aux scrwin_set_width "*";
#pragma aux scrwin_get_width "*";
#pragma aux scrwin_set_height "*";
#pragma aux scrwin_get_height "*";
#pragma aux scrwin_draw "*";
#pragma aux scrwin_keypress "*";
#pragma aux scrwin_show "*";
#pragma aux scrwin_focus "*";
#pragma aux scrwin_leave "*";
#pragma aux scrwin_close "*";

#endif  /* __WATCOMC__ */

#endif  /* SCREEN_H */
