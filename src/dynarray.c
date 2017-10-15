/* dynarray.c -- dynamic array handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"

#include "dynarray.h"

struct _dynarr_t *__far _dynarr_new(void)
{
    uint16_t seg;
            
    if (!_dos_allocmem(_dos_para(sizeof(struct _dynarr_t)), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void __far _dynarr_init(struct _dynarr_t *self, void *parent, uint16_t item_size, _dynarr_clear_item_t clear_item, _dynarr_free_item_t free_item)
{
    if (self)
    {
        self->parent = parent;
        self->item_size = item_size;
        self->size = 0;
        self->list = NULL;
        self->clear_item = clear_item;
        self->free_item = free_item;
    }
}

void *__near _dynarr_item_ptr(struct _dynarr_t *self, void *item, uint16_t index)
{
    return MK_FP(FP_SEG(item), FP_OFF(item) + index * self->item_size);
}

void __far _dynarr_clear_list(struct _dynarr_t *self)
{
    uint16_t i;

    if (self)
        if (self->list && self->size)
        {
            if (self->clear_item)
                for (i = 0; i < self->size; i++)
                    self->clear_item(self->parent, _dynarr_item_ptr(self, self->list, i));
            else
                memset(self->list, 0, self->size * self->item_size);
        }
}

void __far _dynarr_delete(struct _dynarr_t **self)
{
    if (self)
        if (*self)
        {
            _dos_freemem(FP_SEG(*self));
            *self = NULL;
        }
}

void __far _dynarr_set_item(struct _dynarr_t *self, uint16_t index, void *item)
{
    void *dest;

    if (self)
        if (self->list && index < self->size)
        {
            dest = _dynarr_item_ptr(self, self->list, index);
            if (item)
                memcpy(dest, item, self->item_size);
            else
                if (self->clear_item)
                    self->clear_item(self->parent, dest);
                else
                    memset(dest, 0, self->item_size);
        }
}

void *__far _dynarr_get_item(struct _dynarr_t *self, uint16_t index)
{
    if (self)
        if (self->list && index < self->size)
            return _dynarr_item_ptr(self, self->list, index);

    return NULL;
}

bool __far _dynarr_set_size(struct _dynarr_t *self, uint16_t size)
{
    uint16_t seg, max, memsize, i;
    bool result;

    if (self)
    {
        if (self->size != size)
        {
            memsize = _dos_para(size * self->item_size);
            if (size > self->size)
            {
                // Grow
                if (self->size)
                {
                    // do only when new DOS size differs
                    if (_dos_para(self->size * self->item_size) != memsize)
                        result = !_dos_setblock(memsize, FP_SEG(self->list), &max);
                    else
                        result = true;
                }
                else
                {
                    result = !_dos_allocmem(memsize, &seg);
                    if (result)
                        self->list = MK_FP(seg, 0);
                    else
                        self->list = NULL;
                }

                if (result)
                {
                    memset(_dynarr_item_ptr(self, self->list, self->size), 0, (size - self->size) * self->item_size);
                    self->size = size;
                }
            }
            else
            {
                // Shrink
                if (self->free_item)
                {
                    for (i = size; i < self->size; i++)
                        self->free_item(self->parent, _dynarr_item_ptr(self, self->list, i));
                }

                if (size)
                {
                    // do only when new DOS size differs
                    if (_dos_para(self->size * self->item_size) != memsize)
                        _dos_setblock(memsize, FP_SEG(self->list), &max);
                }
                else
                {
                    _dos_freemem(FP_SEG(self->list));
                    self->list = NULL;
                }
                self->size = size;
                result = true;
            }
        }
        else
            result = true;
    }
    else
        result = false;

    return result;
}

uint16_t __far _dynarr_get_size(struct _dynarr_t *self)
{
    if (self)
        return self->size;
    else
        return 0;
}

void __far _dynarr_free(struct _dynarr_t *self)
{
    uint16_t size, i;

    if (self)
    {
        if (self->list)
        {
            if (self->free_item)
                for (i = 0; i < self->size; i++)
                    self->free_item(self->parent, _dynarr_item_ptr(self, self->list, i));

            _dos_freemem(FP_SEG(self->list));
        }
    }
}
