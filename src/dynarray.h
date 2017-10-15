/* dynarray.h -- declarations for dynamic array handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DYNARRAY_H
#define DYNARRAY_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"

/*** Dynamic array virtual methods ***/

typedef void __far _dynarr_clear_item_t(void *parent, void *item);
typedef void __far _dynarr_free_item_t(void *parent, void *item);

/*** Dynamic array structure ***/

#pragma pack(push, 1);
typedef struct _dynarr_t
{
    void *parent;
    uint16_t item_size;
    uint16_t size;
    void *list;
    _dynarr_clear_item_t *clear_item;
    _dynarr_free_item_t *free_item;
};
#pragma pack(pop);

struct _dynarr_t *
         __far _dynarr_new(void);
void     __far _dynarr_init(struct _dynarr_t *self, void *parent, uint16_t item_size,
            _dynarr_clear_item_t clear_item,
            _dynarr_free_item_t free_item);
void     __far _dynarr_clear_list(struct _dynarr_t *self);
void     __far _dynarr_delete(struct _dynarr_t **self);
void     __far _dynarr_set_item(struct _dynarr_t *self, uint16_t index, void *item);
void    *__far _dynarr_get_item(struct _dynarr_t *self, uint16_t index);
bool     __far _dynarr_set_size(struct _dynarr_t *self, uint16_t size);
uint16_t __far _dynarr_get_size(struct _dynarr_t *self);
void     __far _dynarr_free(struct _dynarr_t *self);

#endif  /* DYNARRAY_H */
