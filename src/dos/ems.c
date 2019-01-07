/* ems.c -- DOS Expanded Memory Service library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/stdlib.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "common.h"
#include "debug.h"

#include "dos/ems.h"

#ifdef DEFINE_LOCAL_DATA

bool     emsInstalled;
EMSERR   emsEC;
EMSVER   emsVersion;
uint16_t emsFrameSeg;    /* real memory segment for first page */
void    *emsFramePtr;    /* real memory pointer for first page */

#endif  /* DEFINE_LOCAL_DATA */

#define DOS_DRIVER_NAME_OFF 10

typedef char DOSDriverName_t[8];
typedef DOSDriverName_t DOSDRVNAME;

const static DOSDRVNAME EMS_DRIVER_NAME = "EMMXXXX0";

/* Error messages */

static const struct emsErrorDesc_t
{
    EMSERR code;
    char *msg;
} EMS_ERRORS[] =
{
    { 0x80, "Internal error in EMS" },
    { 0x81, "EMS hardware failure" },
    { 0x82, "EMS is busy" },
    { 0x83, "Invalid handle" },
    { 0x84, "Undefined function requested" },
    { 0x85, "No more handles available" },
    { 0x86, "Error in save or restore of mapping context" },
    { 0x87, "Allocation request was larger than total expanded memory" },
    { 0x88, "No enough free pages" },
    { 0x89, "Zero pages requested" },
    { 0x8a, "Access error: this logical page does not belong to this handle" },
    { 0x8b, "Wrong page number. Only physical pages 0-3 are defined" },
    { 0x8c, "Mapping context save area is full" },
    { 0x8d, "Can save context only once per handle" },
    { 0x8e, "Unable to restore context without a prior save" },
    { 0x8f, "Subfunction parameter not defined" },
    { 0x90, "Attribute type undefined" },
    { 0x91, "Non-volatility not supported" },
    { 0x92, "Source and dest overlap in EMS RAM (warning, not error)" },
    { 0x93, "Destination area in handle too small" },
    { 0x94, "Conventional memory overlaps EMS RAM" },
    { 0x95, "Offset too large in block move" },
    { 0x96, "Block size too large (> 1M)" },
    { 0x97, "Source and dest are in same handle and overlap" },
    { 0x98, "Source or dest memory type invalid (in offset 4 of packet)" },
    { 0x9a, "Alternate map register set in not supported" },
    { 0x9b, "All alternate map/DMA register sets are all allocated" },
    { 0x9c, "All alternate map/DMA register sets are not supported" },
    { 0x9d, "Specified alternate map/DMA register set invalid or in use" },
    { 0x9e, "Dedicated DMA channels are not supported" },
    { 0x9f, "Specified DMA channel is not supported" },
    { 0xa0, "No handle matches specified name" },
    { 0xa1, "The specified name already exists" },
    { 0xa2, "Source offset+region length > 1M (attempted to wrap)" },
    { 0xa3, "Contents of specified data packet are corrupted or invalid" },
    { 0xa4, "Access to this function has been denied (bad access key)" },
    { 0, NULL }
};

static const char EMS_ERROR_UNKNOWN[] = "Unknown error";

const char *__far emsGetErrorMsg (void)
{
    int i;

    i = 0;
    while (EMS_ERRORS[i].code)
    {
        if (EMS_ERRORS[i].code == emsEC)
            return EMS_ERRORS[i].msg;
        i++;
    }

    return EMS_ERROR_UNKNOWN;
}

#define MAX_ITEMS 6

#pragma pack(push, 1);
typedef struct handle_entry_t HDLENT;
typedef struct handle_entry_t
{
    HDLENT *next;
    EMSHDL items[MAX_ITEMS];
};  // size of entry is 16 bytes
#pragma pack(pop);

#pragma pack(push, 1);
typedef struct handles_list_t HANDLESLIST;
typedef struct handles_list_t
{
    HDLENT *root;
};
#pragma pack(pop);

static HANDLESLIST _handleslist;

/* Entry */

void __near _entry_clear(HDLENT *self)
{
    int i;

    if (self)
    {
        self->next = NULL;
        for (i = 0; i < MAX_ITEMS; i++)
            self->items[i] = EMSBADHDL;
    }
}

int __near _entry_find_item(HDLENT *self, EMSHDL item)
{
    int i;

    for (i = 0; i < MAX_ITEMS; i++)
        if (self->items[i] == item)
            return i;

    return -1;
}

bool __near _entry_is_empty(HDLENT *self)
{
    int i;

    for (i = 0; i < MAX_ITEMS; i++)
        if (self->items[i] != EMSBADHDL)
            return false;

    return true;
}

/* List */

void __near _list_clear(HANDLESLIST *self)
{
    if (self)
        self->root = NULL;
}

HDLENT *__near _list_find_item(HANDLESLIST *self, EMSHDL item, int *index)
{
    HDLENT *entry;
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

bool __near _list_add_item(HANDLESLIST *self, EMSHDL item)
{
    HDLENT *entry;
    int i;

    if (self)
    {
        entry = _list_find_item(self, EMSBADHDL, &i);
        if (entry)
        {
            entry->items[i] = item;
            return true;
        }
        else
        {
            entry = _new(HDLENT);
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

bool __near _list_remove_item(HANDLESLIST *self, EMSHDL item)
{
    HDLENT *prev, *entry;
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
                entry->items[i] = EMSBADHDL;
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

bool __near _ems_free(EMSHDL handle);

void __near _list_free(HANDLESLIST *self)
{
    HDLENT *entry;
    EMSHDL item;
    int i;

    if (self)
        while (self->root)
        {
            entry = self->root;
            for (i = 0; i < MAX_ITEMS; i++)
            {
                item = entry->items[i];
                if (item != EMSBADHDL)
                    _ems_free(item);
            }
            self->root = entry->next;
            _delete(entry);
        }
}

#ifndef USE_INTRINSICS

uint16_t emsCalcPagesCount(uint32_t size)
{
    return _emsCalcPagesCount(size);
}

#endif  /* !USE_INTRINSICS */

bool __far emsIsInstalled (void)
{
    union REGPACK regs;
    DOSDRVNAME *p;

    regs.w.ax = 0x3567;
    intr(0x21, &regs);
    p = MK_FP(regs.w.es, DOS_DRIVER_NAME_OFF);
    return (memcmp(p, EMS_DRIVER_NAME, sizeof(DOSDRVNAME)) == 0);
}

bool __far emsGetVersion (void)
{
    union REGPACK regs;

    regs.w.ax = 0x4600;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        emsVersion.Lo = 0;
        emsVersion.Hi = 0;
        return false;
    }
    else
    {
        emsVersion.Lo = regs.h.al & 0x0f;
        emsVersion.Hi = regs.h.al >> 4;
        return true;
    }
}

uint16_t __far emsGetFrameSeg (void)
{
    union REGPACK regs;

    regs.w.ax = 0x4100;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return -1;
    }
    else
        return regs.w.bx;
}

uint16_t __far emsGetFreePagesCount (void)
{
    union REGPACK regs;

    regs.w.ax = 0x4200;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return 0;
    }
    else
        return regs.w.bx;
}

EMSHDL __near _ems_alloc(uint16_t pages)
{
    union REGPACK regs;

    regs.w.ax = 0x4300;
    regs.w.bx = pages;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return EMSBADHDL;
    }
    else
        return regs.w.dx;
}

EMSHDL __far emsAlloc (uint16_t pages)
{
    EMSHDL h;

    h = _ems_alloc(pages);
    if (h != EMSBADHDL)
        _list_add_item(&_handleslist, h);

    return h;
}

bool __far emsResize (EMSHDL handle, uint16_t pages)
{
    union REGPACK regs;

    if (emsVersion.Hi >= 4)
    {
        regs.w.ax = 0x5100;
        regs.w.bx = pages;
        regs.w.dx = handle;
        intr(0x67, &regs);
        if (regs.h.ah)
        {
            emsEC = regs.h.ah;
            return false;
        }
        else
            return true;
    }
    else
    {
        emsEC = 0x84;
        return false;
    }
}

bool __near _ems_free(EMSHDL handle)
{
    union REGPACK regs;

    regs.w.ax = 0x4500;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

bool __far emsFree (EMSHDL handle)
{
    if (_ems_free(handle))
    {
        _list_remove_item(&_handleslist, handle);
        return true;
    }

    return false;
}

bool __far emsMap (EMSHDL handle, uint16_t logPage, uint8_t physPage)
{
    union REGPACK regs;

    regs.h.ah = 0x44;
    regs.h.al = physPage;
    regs.w.bx = logPage;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

bool __far emsSaveMap (EMSHDL handle)
{
    union REGPACK regs;

    regs.w.ax = 0x4700;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

bool __far emsRestoreMap (EMSHDL handle)
{
    union REGPACK regs;

    regs.w.ax = 0x4800;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

uint16_t __far emsGetHandleSize (EMSHDL handle)
{
    union REGPACK regs;

    regs.w.ax = 0x4c00;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return 0;
    }
    else
        return regs.w.bx;
}

bool __far emsSetHandleName (EMSHDL handle, EMSNAME *name)
{
    union REGPACK regs;

    if (emsVersion.Hi >= 4)
    {
        regs.w.ax = 0x5301;
        regs.w.dx = handle;
        regs.w.si = FP_OFF(name);
        regs.w.ds = FP_SEG(name);
        intr(0x67, &regs);
        if (regs.h.ah)
        {
            emsEC = regs.h.ah;
            return false;
        }
        else
            return true;
    }
    else
    {
        emsEC = 0x84;
        return false;
    }
}

/*** Initialization ***/

void __near ems_init (void)
{
    emsEC = 0;
    emsFrameSeg = 0;
    emsFramePtr = NULL;
    _list_clear(&_handleslist);
    emsInstalled = emsIsInstalled();
    if (emsInstalled)
    {
        if (emsGetVersion())
        {
            emsFrameSeg = emsGetFrameSeg();
            emsFramePtr = MK_FP(emsFrameSeg, 0);
            DEBUG_MSG_("ems_init", "Found EMS version %hu.%03hu.", emsVersion.Hi, emsVersion.Lo);
            DEBUG_MSG_("ems_init", "%lu KiB of expanded memory available.", (uint32_t)emsGetFreePagesCount()<<4);
        }
        else
        {
            emsInstalled = false;
            DEBUG_ERR("ems_init", "Failed to get EMS version.");
        }
    }
    if (DEBUG)
        if (!emsInstalled)
            DEBUG_MSG("ems_init", "No EMS is available.");
}

void __near ems_done (void)
{
    _list_free(&_handleslist);
}

DEFINE_REGISTRATION (ems, ems_init, ems_done)
