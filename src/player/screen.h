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

/* Text window */

#define scrWidth 80
#define scrHeight 25

#pragma pack(push, 1);
typedef struct window_rect_t
{
    bool visible;
    bool focused;
    uint8_t fgColor, bgColor;
    uint8_t x0, y0, x1, y1;
    uint8_t width, height;
};
#pragma pack(pop);
typedef struct window_rect_t WINDOWRECT;

void PUBLIC_CODE window_init(
    WINDOWRECT *self,
    uint8_t fgColor, uint8_t bgColor,
    uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1
);

bool PUBLIC_CODE window_is_created(WINDOWRECT *self);
void PUBLIC_CODE window_show(WINDOWRECT *self);
void PUBLIC_CODE window_focus(WINDOWRECT *self);
void PUBLIC_CODE window_leave(WINDOWRECT *self);
void PUBLIC_CODE window_close(WINDOWRECT *self);

#endif  /* SCREEN_H */
