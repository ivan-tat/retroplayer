/* screen.c -- simple text mode library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>

#include "pascal.h"
#include "cc/conio.h"

#include "player/screen.h"

void PUBLIC_CODE window_init(
    WINDOWRECT *self,
    uint8_t fgColor, uint8_t bgColor,
    uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1
)
{
    if (self)
    {
        self->visible = false;
        self->focused = false;
        self->fgColor = fgColor;
        self->bgColor = bgColor;
        self->x0 = x0;
        self->y0 = y0;
        self->x1 = x1;
        self->y1 = y1;
        self->width = x1 - x0 + 1;
        self->height = y1 - y0 + 1;
    }
}

bool PUBLIC_CODE window_is_created(WINDOWRECT *self)
{
    return self != NULL;
}

void PUBLIC_CODE window_show(WINDOWRECT *self)
{
    if (self)
    {
        window(self->x0, self->y0, self->x1, self->y1);
        textcolor(self->fgColor);
        textbackground(self->bgColor);
        clrscr();
        self->visible = true;
        self->focused = true;
    }
}

void PUBLIC_CODE window_focus(WINDOWRECT *self)
{
    if (self->visible)
    {
        window(self->x0, self->y0, self->x1, self->y1);
        textcolor(self->fgColor);
        textbackground(self->bgColor);
        self->focused = true;
    }
}

void PUBLIC_CODE window_leave(WINDOWRECT *self)
{
    self->focused = false;
    window(1, 1, scrWidth, scrHeight);
}

void PUBLIC_CODE window_close(WINDOWRECT *self)
{
    if (self)
    {
        if (self->focused)
            window_leave(self);
        self->visible = false;
    }
}

