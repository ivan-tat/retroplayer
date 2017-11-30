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
#include "common.h"
#include "debug.h"

#include "hw/hwowner.h"

typedef struct hwowner_t
{
    HWOWNERID id;
    const char *name;
};

typedef struct hwowner_entry_t HWOWNERENT;
typedef struct hwowner_entry_t
{
    struct hwowner_t *owner;
    HWOWNERENT *next;
};

#define OWNER ((struct hwowner_t *)owner)

static HWOWNERID _hwowner_id;
static HWOWNERENT *_list;

/* List */

bool __near _list_insert_entry(struct hwowner_t *owner)
{
    HWOWNERENT *entry;

    entry = _new(HWOWNERENT);
    if (entry)
    {
        entry->owner = owner;
        entry->next = _list;
        _list = entry;
        return true;
    }
    else
        return false;
}

HWOWNERENT *__near _list_find_entry(HWOWNER *owner)
{
    HWOWNERENT *entry;

    entry = _list;

    while (entry && (entry->owner != owner))
        entry = entry->next;

    return entry;
}

HWOWNERENT *__near _list_find_entry_by_id(HWOWNERID id)
{
    HWOWNERENT *entry;

    entry = _list;

    while (entry && (entry->owner->id != id))
        entry = entry->next;

    return entry;
}

bool __near _list_remove_entry(struct hwowner_t *owner)
{
    HWOWNERENT *prev, *cur;

    prev = NULL;
    cur = _list;

    while (cur && (cur->owner != owner))
    {
        prev = cur;
        cur = cur->next;
    }

    if (cur)
    {
        if (prev)
            prev->next = cur->next;
        else
            _list = cur->next;

        _delete(cur);
        return true;
    }
    else
        return false;
}

/* Entry */

void hwowner_init(struct hwowner_t *self, const char *name)
{
    if (self)
    {
        self->id = ++_hwowner_id;
        self->name = name;
    }
}

HWOWNER *hwowner_register(const char *name)
{
    struct hwowner_t *owner;

    owner = _new(struct hwowner_t);
    if (owner)
    {
        hwowner_init(owner, name);
        DEBUG_INFO_("hwowner_register", "Registered HW owner \"%s\" with id=0x%04X.", name, owner->id);
        return owner;
    }
    else
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
    struct hwowner_entry_t *entry;

    entry = _list_find_entry_by_id(id);
    if (entry)
        return entry->owner->name;
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
        _list_remove_entry(owner);
        DEBUG_INFO_("hwowner_unregister", "Unregistered HW owner \"%s\" with id=0x%04X.", name, id);
    }
}

/* Initialization */

void __near _hwowner_init(void)
{
    _hwowner_id = 0;
    _list = NULL;
}

void __near _hwowner_done(void)
{
    while (_list)
        if (!_list_remove_entry(_list->owner))
            break;
}

DEFINE_REGISTRATION(hwowner, _hwowner_init, _hwowner_done)
