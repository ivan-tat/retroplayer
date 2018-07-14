/* clreol.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>

#include "cc/i86.h"
#include "hw/vbios.h"

#include "cc/conio.h"

void cc_clreol(void)
{
    struct vbios_cursor_state_t cursor;

    vbios_query_cursor_state(0, &cursor);
    vbios_scroll_up(cursor.x, cursor.y, windmax.rect.x, cursor.y, 0, textattr);
}
