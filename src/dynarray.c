/* dynarray.c -- dynamic array handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$dynarray$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "dynarray.h"

void __far dynarr_init(DYNARR *self, void *parent, uint16_t item_size, dynarr_init_item_t init_item, dynarr_free_item_t free_item)
{
    if (self)
    {
        self->parent = parent;
        self->item_size = item_size;
        self->size = 0;
        self->list = NULL;
        self->init_item = init_item;
        self->free_item = free_item;
    }
}

void *__near dynarr_item_ptr(DYNARR *self, void *item, uint16_t index)
{
    return MK_FP(FP_SEG(item), FP_OFF(item) + index * self->item_size);
}

void __far dynarr_init_items(DYNARR *self, uint16_t index, uint16_t count)
{
    uint16_t i, end;

    if (self)
        if (self->list && count)
        {
            if (self->init_item)
            {
                end = index + count;
                for (i = index; i < end; i++)
                    self->init_item(self->parent, dynarr_item_ptr(self, self->list, i));
            }
            else
                memset(dynarr_item_ptr(self, self->list, index), 0, count * self->item_size);
        }
}

void __far dynarr_free_items(DYNARR *self, uint16_t index, uint16_t count)
{
    uint16_t i, end;

    if (self)
        if (self->list && count && self->free_item)
        {
            end = index + count;
            for (i = index; i < end; i++)
                self->free_item(self->parent, dynarr_item_ptr(self, self->list, i));
        }
}

void __far dynarr_set_item(DYNARR *self, uint16_t index, void *item)
{
    void *dest;

    if (self)
        if (self->list && index < self->size)
        {
            dest = dynarr_item_ptr(self, self->list, index);
            if (item)
                memcpy(dest, item, self->item_size);
            else
                if (self->init_item)
                    self->init_item(self->parent, dest);
                else
                    memset(dest, 0, self->item_size);
        }
}

void *__far dynarr_get_item(DYNARR *self, uint16_t index)
{
    if (self)
        if (self->list && index < self->size)
            return dynarr_item_ptr(self, self->list, index);

    return NULL;
}

int32_t __far dynarr_indexof(DYNARR *self, void *item)
{
    uint16_t offset;

    if (self)
    {
        if (self->list
        && (FP_SEG (self->list) == FP_SEG (item))
        && (FP_OFF (self->list) <= FP_OFF (item)))
        {
            offset = FP_OFF (item) - FP_OFF (self->list);
            if ((offset < self->item_size * self->size)
            && ((offset % self->item_size) == 0))
                return offset / self->item_size;
        }
    }

    return -1;
}

bool __far dynarr_set_size(DYNARR *self, uint16_t size)
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
                    dynarr_init_items(self, self->size, size - self->size);
                    self->size = size;
                }
            }
            else
            {
                // Shrink
                dynarr_free_items(self, size, self->size - size);

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

uint16_t __far dynarr_get_size(DYNARR *self)
{
    if (self)
        return self->size;
    else
        return 0;
}

void __far dynarr_free(DYNARR *self)
{
    if (self)
    {
        if (self->list)
        {
            dynarr_free_items(self, 0, self->size);
            _dos_freemem(FP_SEG(self->list));
        }
    }
}
