/* hwowner.c -- generic hardware resource owner library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/string.h"
#include "debug.h"

#include "hw/hwowner.h"

static uint16_t _hwowner_id = 0;

void hwowner_clear(HWOWNER *self)
{
    if (self)
        memset(self, 0, sizeof(HWOWNER));
}

void hwowner_init(HWOWNER *self, const char *name)
{
    if (self)
    {
        self->id = ++_hwowner_id;
        self->name = name;
    }
}

void hwowner_free(HWOWNER *self)
{
    if (self)
    {
        self->id = 0;
        self->name = NULL;
    }
}

/* Initialization */

void __near _hwowner_init(void)
{
    _hwowner_id = 0;
}

void __near _hwowner_done(void)
{
}

DEFINE_REGISTRATION(hwowner, _hwowner_init, _hwowner_done)
