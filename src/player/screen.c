/* screen.c -- simple text mode library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"

#include "player/screen.h"

void PUBLIC_CODE scrwin_init(
    SCRWIN *self,
    uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
    void *draw,
    void *keypress
)
{
    if (self)
    {
        self->flags = 0;
        self->rect.x0 = x0;
        self->rect.y0 = y0;
        self->rect.x1 = x1;
        self->rect.y1 = y1;
        self->draw = draw;
        self->keypress = keypress;
    }
}

bool PUBLIC_CODE scrwin_is_created(SCRWIN *self)
{
    return self != NULL;
}

void PUBLIC_CODE scrwin_set_flags(SCRWIN *self, WINFLAGS value)
{
    if (self)
        self->flags = value;
}

WINFLAGS PUBLIC_CODE scrwin_get_flags(SCRWIN *self)
{
    if (self)
        return self->flags;
    else
        return 0;
}

void PUBLIC_CODE scrwin_set_width(SCRWIN *self, uint8_t value)
{
    if (self)
        self->rect.x1 = self->rect.x0 + value;
}

uint8_t PUBLIC_CODE scrwin_get_width(SCRWIN *self)
{
    if (self)
        return self->rect.x1 - self->rect.x0;
    else
        return 0;
}

void PUBLIC_CODE scrwin_set_height(SCRWIN *self, uint8_t value)
{
    if (self)
        self->rect.y1 = self->rect.y0 + value;
}

uint8_t PUBLIC_CODE scrwin_get_height(SCRWIN *self)
{
    if (self)
        return self->rect.y1 - self->rect.y0;
    else
        return 0;
}

void __near _scrwin_draw_begin(SCRWIN *self)
{
    if (self)
    {
        // enter window's area
        window(self->rect.x0, self->rect.y0, self->rect.x1, self->rect.y1);
    }
}

void __near _scrwin_draw_end(SCRWIN *self)
{
    if (self)
        // leave window's area
        window(1, 1, scrWidth, scrHeight);
}

void PUBLIC_CODE scrwin_draw(SCRWIN *self)
{
    if (self)
        if (self->flags & WINFL_VISIBLE)
        {
            _scrwin_draw_begin(self);

            // send events
            if (self->draw)
                self->draw(self);

            // clear events
            self->flags &= ~WINFL_REDRAW;

            _scrwin_draw_end(self);
        }
}

bool PUBLIC_CODE scrwin_keypress(SCRWIN *self, char key)
{
    if (self)
        if (self->flags & WINFL_VISIBLE)
        {
            // send events
            if (self->keypress)
                return self->keypress(self, key);
        }

    return false;
}

void PUBLIC_CODE scrwin_show(SCRWIN *self)
{
    if (self)
    {
        if (!(self->flags & WINFL_VISIBLE))
        {
            // state
            self->flags |= WINFL_VISIBLE | WINFL_FOCUSED;
            // events
            self->flags |= WINFL_FULLREDRAW | WINFL_FOCUSREDRAW;

            // send events
            scrwin_draw(self);
        }
    }
}

void PUBLIC_CODE scrwin_focus(SCRWIN *self)
{
    if (self)
        if ((self->flags & WINFL_VISIBLE)
        && !(self->flags & WINFL_FOCUSED))
        {
            // state
            self->flags |= WINFL_FOCUSED;
            // events
            self->flags |= WINFL_FOCUSREDRAW;

            // send events
            scrwin_draw(self);
        }
}

void PUBLIC_CODE scrwin_leave(SCRWIN *self)
{
    if (self)
        if ((self->flags & WINFL_VISIBLE)
        &&  (self->flags & WINFL_FOCUSED))
        {
            // state
            self->flags &= ~WINFL_FOCUSED;
            // events
            self->flags |= WINFL_FOCUSREDRAW;

            // send events
            scrwin_draw(self);
        }
}

void PUBLIC_CODE scrwin_close(SCRWIN *self)
{
    if (self)
    {
        if (self->flags & WINFL_FOCUSED)
            scrwin_leave(self);

        // state
        self->flags &= ~(WINFL_VISIBLE | WINFL_FOCUSED);

        // TODO: send event to parent: draw self->rect
    }
}

