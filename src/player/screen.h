/* screen.h -- declarations for screen.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYER_SCREEN_H_INCLUDED
#define _PLAYER_SCREEN_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

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

/* virtual methods table (vmt) */

typedef struct screen_window_t SCRWIN;

#pragma pack(push, 1);
typedef struct screen_window_vmt_t
{
    void __far (*on_resize) (SCRWIN *self);
    void __far (*draw) (SCRWIN *self);
    bool __far (*keypress) (SCRWIN *self, char key);
    void __far (*free)(SCRWIN *self);
};
#pragma pack(pop);
typedef struct screen_window_vmt_t SCRWINVMT;

/* structure */

#pragma pack(push, 1);
typedef struct screen_window_t
{
    char *__class_name;
    SCRWINVMT __vmt;
    WINFLAGS flags;
    SCRRECT rect;
    void *data;
};
#pragma pack(pop);

#define _scrwin_is_created(o)       ((o) != NULL)
#define _scrwin_get_class_name(o)   (o)->__class_name
#define _scrwin_get_flags(o)        (o)->flags
#define _scrwin_set_flags(o, v)     _scrwin_get_flags (o) = (v)
#define _scrwin_get_data(o)         (o)->data
#define _scrwin_set_data(o, v)      _scrwin_get_data (o) = (void *) (v)
#define _scrwin_on_resize(o)        (o)->__vmt.on_resize (o)
#define _scrwin_draw(o)             (o)->__vmt.draw (o)
#define _scrwin_keypress(o, k)      (o)->__vmt.keypress (o, k)
#define _scrwin_free(o)             (o)->__vmt.free (o)

void     __far scrwin_init (SCRWIN *self, char *class_name);
#define        scrwin_is_created(o)     _scrwin_is_created (o)
#define        scrwin_get_class_name(o) _scrwin_get_class_name(o)
#define        scrwin_set_flags(o, v)   _scrwin_set_flags (o, v)
#define        scrwin_get_flags(o)      _scrwin_get_flags (o)
void     __far scrwin_set_rect (SCRWIN *self, SCRRECT *rect);
void     __far scrwin_set_coords (SCRWIN *self, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void     __far scrwin_set_width (SCRWIN *self, uint8_t value);
uint8_t  __far scrwin_get_width (SCRWIN *self);
void     __far scrwin_set_height (SCRWIN *self, uint8_t value);
uint8_t  __far scrwin_get_height (SCRWIN *self);
#define        scrwin_set_data(o, v)    _scrwin_set_data (o, v)
#define        scrwin_get_data(o)       _scrwin_get_data (o)
#define        scrwin_on_resize(o)      _scrwin_on_resize (o)
void     __far scrwin_draw (SCRWIN *self);
bool     __far scrwin_keypress (SCRWIN *self, char key);
void     __far scrwin_show (SCRWIN *self);
void     __far scrwin_focus (SCRWIN *self);
void     __far scrwin_leave (SCRWIN *self);
void     __far scrwin_close (SCRWIN *self);
#define        scrwin_free(o)           _scrwin_free (o)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux scrwin_init "*";
#pragma aux scrwin_set_rect "*";
#pragma aux scrwin_set_coords "*";
#pragma aux scrwin_set_width "*";
#pragma aux scrwin_get_width "*";
#pragma aux scrwin_set_height "*";
#pragma aux scrwin_get_height "*";
#pragma aux __scrwin_on_resize "*";
#pragma aux __scrwin_draw "*";
#pragma aux scrwin_draw "*";
#pragma aux __scrwin_keypress "*";
#pragma aux scrwin_keypress "*";
#pragma aux scrwin_show "*";
#pragma aux scrwin_focus "*";
#pragma aux scrwin_leave "*";
#pragma aux scrwin_close "*";
#pragma aux __scrwin_free "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYER_SCREEN_H_INCLUDED */
