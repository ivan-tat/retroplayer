/* screen.c -- simple text mode library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "cc/string.h"
#include "common.h"

#include "player/screen.h"

void __far __scrwin_on_resize (SCRWIN *self);
void __far __scrwin_draw (SCRWIN *self);
bool __far __scrwin_keypress (SCRWIN *self, char key);
void __far __scrwin_free (SCRWIN *self);

static const SCRWINVMT __scrwin_vmt =
{
    &__scrwin_on_resize,
    &__scrwin_draw,
    &__scrwin_keypress,
    &__scrwin_free
};

// public methods

void __far scrwin_init (SCRWIN *self, char *class_name)
{
    memset (self, 0, sizeof (SCRWIN));
    self->__class_name = class_name;
    _copy_vmt (self, __scrwin_vmt, SCRWINVMT);
}

void __far scrwin_set_rect (SCRWIN *self, SCRRECT *rect)
{
    self->rect.x0 = rect->x0;
    self->rect.y0 = rect->y0;
    self->rect.x1 = rect->x1;
    self->rect.y1 = rect->y1;
}

void __far scrwin_set_coords (SCRWIN *self, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    SCRRECT rect;
    rect.x0 = x0;
    rect.y0 = y0;
    rect.x1 = x1;
    rect.y1 = y1;
    scrwin_set_rect (self, &rect);
}

void __far scrwin_set_width (SCRWIN *self, uint8_t value)
{
    if (self)
        self->rect.x1 = self->rect.x0 + value;
}

uint8_t __far scrwin_get_width (SCRWIN *self)
{
    if (self)
        return self->rect.x1 - self->rect.x0 + 1;
    else
        return 0;
}

void __far scrwin_set_height (SCRWIN *self, uint8_t value)
{
    if (self)
        self->rect.y1 = self->rect.y0 + value;
}

uint8_t __far scrwin_get_height (SCRWIN *self)
{
    if (self)
        return self->rect.y1 - self->rect.y0 + 1;
    else
        return 0;
}

void __far __scrwin_on_resize (SCRWIN *self)
{
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

void __far __scrwin_draw (SCRWIN *self)
{
}

void __far scrwin_draw (SCRWIN *self)
{
    if (self)
        if (self->flags & WINFL_VISIBLE)
        {
            _scrwin_draw_begin(self);

            // send events
            _scrwin_draw (self);

            // clear events
            self->flags &= ~WINFL_REDRAW;

            _scrwin_draw_end(self);
        }
}

bool __far __scrwin_keypress (SCRWIN *self, char key)
{
    return false;
}

bool __far scrwin_keypress (SCRWIN *self, char key)
{
    if (self)
        if (self->flags & WINFL_VISIBLE)
        {
            // send events
            return _scrwin_keypress (self, key);
        }

    return false;
}

void __far scrwin_show (SCRWIN *self)
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

void __far scrwin_focus (SCRWIN *self)
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

void __far scrwin_leave (SCRWIN *self)
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

void __far scrwin_close (SCRWIN *self)
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

void __far __scrwin_free (SCRWIN *self)
{
}
