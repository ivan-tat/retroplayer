/* hwowner.c -- generic hardware resource owner library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "common.h"
#include "debug.h"

#include "hw/hwowner.h"

#pragma pack(push, 1);
typedef struct hwowner_t HWOWNERITEM;
typedef struct hwowner_t
{
    HWOWNERID id;
    const char *name;
};
#pragma pack(pop);

#define MAX_ITEMS 3

#pragma pack(push, 1);
typedef struct hwowner_entry_t HWOWNERENT;
typedef struct hwowner_entry_t
{
    HWOWNERENT *next;
    HWOWNERITEM *items[MAX_ITEMS];
};  // size of entry is 16 bytes
#pragma pack(pop);

#pragma pack(push, 1);
typedef struct hwowners_list_t HWOWNERSLIST;
typedef struct hwowners_list_t
{
    HWOWNERENT *root;
};
#pragma pack(pop);

#define OWNER ((HWOWNERITEM *)owner)

static HWOWNERID _hwowner_id;
static HWOWNERSLIST _hwowners_list;

/* Entry */

void __near _entry_clear(HWOWNERENT *self)
{
    if (self)
        memset(self, 0, sizeof(HWOWNERENT));
}

int __near _entry_find_item(HWOWNERENT *self, HWOWNERITEM *item)
{
    int i;

    for (i = 0; i < MAX_ITEMS; i++)
        if (self->items[i] == item)
            return i;

    return -1;
}

int __near _entry_find_item_by_id(HWOWNERENT *self, HWOWNERID id)
{
    int i;
    HWOWNERITEM *item;

    for (i = 0; i < MAX_ITEMS; i++)
    {
        item = self->items[i];
        if (item && (item->id == id))
            return i;
    }

    return -1;
}

bool __near _entry_is_empty(HWOWNERENT *self)
{
    int i;

    for (i = 0; i < MAX_ITEMS; i++)
        if (self->items[i])
            return false;

    return true;
}

/* List */

void __near _list_clear(HWOWNERSLIST *self)
{
    if (self)
        self->root = NULL;
}

HWOWNERENT *__near _list_find_item(HWOWNERSLIST *self, HWOWNERITEM *item, int *index)
{
    HWOWNERENT *entry;
    int i;

    if (self)
    {
        entry = self->root;

        while (entry)
        {
            i = _entry_find_item(entry, item);
            if (i >= 0)
            {
                *index = i;
                return entry;
            }
            entry = entry->next;
        }
    }

    return NULL;
}

HWOWNERENT *__near _list_find_item_by_id(HWOWNERSLIST *self, HWOWNERID id, int *index)
{
    HWOWNERENT *entry;
    int i;

    if (self)
    {
        entry = self->root;

        while (entry)
        {
            i = _entry_find_item_by_id(entry, id);
            if (i >= 0)
            {
                *index = i;
                return entry;
            }
            entry = entry->next;
        }
    }

    return NULL;
}

bool __near _list_add_item(HWOWNERSLIST *self, HWOWNERITEM *item)
{
    HWOWNERENT *entry;
    int i;

    if (self)
    {
        entry = _list_find_item(self, NULL, &i);
        if (entry)
        {
            entry->items[i] = item;
            return true;
        }
        else
        {
            entry = _new(HWOWNERENT);
            if (entry)
            {
                _entry_clear(entry);
                entry->next = self->root;
                entry->items[0] = item;
                self->root = entry;
                return true;
            }
        }
    }

    return false;
}

bool __near _list_remove_item(HWOWNERSLIST *self, HWOWNERITEM *item)
{
    HWOWNERENT *prev, *entry;
    int i;

    if (self)
    {
        prev = NULL;
        entry = self->root;

        while (entry)
        {
            i = _entry_find_item(entry, item);
            if (i >= 0)
            {
                entry->items[i] = NULL;
                if (_entry_is_empty(entry))
                {
                    if (prev)
                        prev->next = entry->next;
                    else
                        self->root = entry->next;

                    _delete(entry);
                }
                return true;
            }

            prev = entry;
            entry = entry->next;
        }
    }

    return false;
}

void __near _list_free(HWOWNERSLIST *self)
{
    HWOWNERENT *entry;
    HWOWNERITEM *item;
    int i;

    if (self)
        while (self->root)
        {
            entry = self->root;
            for (i = 0; i < MAX_ITEMS; i++)
            {
                item = entry->items[i];
                if (item)
                    _delete(item);
            }
            self->root = entry->next;
            _delete(entry);
        }
}

/* Entry */

void hwowner_init(HWOWNERITEM *self, const char *name)
{
    if (self)
    {
        self->id = ++_hwowner_id;
        self->name = name;
    }
}

HWOWNER *hwowner_register(const char *name)
{
    HWOWNERITEM *owner;

    owner = _new(HWOWNERITEM);
    if (owner)
    {
        hwowner_init(owner, name);
        if (_list_add_item(&_hwowners_list, owner))
        {
            DEBUG_INFO_("hwowner_register", "Registered HW owner \"%s\" with id=0x%04X.", name, owner->id);
            return owner;
        }
    }

    DEBUG_FAIL("hwowner_register", "Failed to register HW owner.");
    return NULL;
}

HWOWNERID hwowner_get_id(HWOWNER *owner)
{
    if (owner)
        return OWNER->id;
    else
        return 0;
}

const char *hwowner_get_name(HWOWNERID id)
{
    HWOWNERENT *entry;
    int i;

    entry = _list_find_item_by_id(&_hwowners_list, id, &i);
    if (entry)
        return entry->items[i]->name;
    else
        return NULL;
}

void hwowner_unregister(HWOWNER *owner)
{
    HWOWNERID id;
    const char *name;

    if (owner)
    {
        id = OWNER->id;
        name = OWNER->name;
        _list_remove_item(&_hwowners_list, owner);
        _delete(owner);
        DEBUG_INFO_("hwowner_unregister", "Unregistered HW owner \"%s\" with id=0x%04X.", name, id);
    }
}

/*** Initialization ***/

void __near _hwowner_init(void)
{
    _hwowner_id = 0;
    _list_clear(&_hwowners_list);
}

void __near _hwowner_done(void)
{
    _list_free(&_hwowners_list);
}

DEFINE_REGISTRATION(hwowner, _hwowner_init, _hwowner_done)
