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
#include "debug.h"

#include "dos/ems.h"

#ifdef DEFINE_LOCAL_DATA

bool     PUBLIC_DATA emsInstalled;
EMSERR   PUBLIC_DATA emsEC;
EMSVER   PUBLIC_DATA emsVersion;
uint16_t PUBLIC_DATA emsFrameSeg;    /* real memory segment for first page */
void    *PUBLIC_DATA emsFramePtr;    /* real memory pointer for first page */

#endif

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

const char *PUBLIC_CODE emsGetErrorMsg(void)
{
    int i;

    i = 0;
    while (EMS_ERRORS[i].code)
    {
        if (EMS_ERRORS[i].code == emsEC)
            return EMS_ERRORS[i].msg;
        i++;
    };

    return EMS_ERROR_UNKNOWN;
}

/* Initialize for Pascal linker */

typedef struct emsHandleEntry_t EMSENT;
typedef struct emsHandleEntry_t
{
    EMSHDL handle;
    EMSENT *next;
};

static EMSENT *ems_hlist = (void *)0;

void __near ems_insert_handle(EMSHDL handle)
{
    uint16_t seg;
    EMSENT *n;

    if (!_dos_allocmem(_dos_para(sizeof(EMSENT)), &seg))
    {
        n = MK_FP(seg, 0);
        n->handle = handle;
        n->next = ems_hlist;
        ems_hlist = n;
    }
}

void __near ems_remove_handle(EMSHDL handle)
{
    EMSENT *i, *h;

    i = NULL;
    h = ems_hlist;
    while ((h != NULL) && (h->handle != handle))
    {
        i = h;
        h = h->next;
    };
    if (h != NULL)
    {
        if (i != NULL)
            i->next = h->next;
        else
            ems_hlist = h->next;
        _dos_freemem(FP_SEG(h));
    };
}

#ifndef USE_INTRINSICS

uint16_t emsCalcPagesCount(uint32_t size)
{
    return _emsCalcPagesCount(size);
}

#endif  /* !USE_INTRINSICS */

bool PUBLIC_CODE emsIsInstalled(void)
{
    union REGPACK regs;
    DOSDRVNAME *p;

    regs.w.ax = 0x3567;
    intr(0x21, &regs);
    p = MK_FP(regs.w.es, DOS_DRIVER_NAME_OFF);
    return (memcmp(p, EMS_DRIVER_NAME, sizeof(DOSDRVNAME)) == 0);
}

bool PUBLIC_CODE emsGetVersion(void)
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
    };
}

uint16_t PUBLIC_CODE emsGetFrameSeg(void)
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

uint16_t PUBLIC_CODE emsGetFreePagesCount(void)
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

EMSHDL PUBLIC_CODE emsAlloc(uint16_t pages)
{
    union REGPACK regs;

    regs.w.ax = 0x4300;
    regs.w.bx = pages;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        emsEC = regs.h.ah;
        return -1;
    }
    else
    {
        ems_insert_handle(regs.w.dx);
        return regs.w.dx;
    };
}

bool PUBLIC_CODE emsResize(EMSHDL handle, uint16_t pages)
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
        return false;
}

bool PUBLIC_CODE emsFree(EMSHDL handle)
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
    {
        ems_remove_handle(regs.w.dx);
        return true;
    };
}

bool PUBLIC_CODE emsMap(EMSHDL handle, uint16_t logPage, uint8_t physPage)
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

bool PUBLIC_CODE emsSaveMap(EMSHDL handle)
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

bool PUBLIC_CODE emsRestoreMap(EMSHDL handle)
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

uint16_t PUBLIC_CODE emsGetHandleSize(EMSHDL handle)
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

bool PUBLIC_CODE emsSetHandleName(EMSHDL handle, EMSNAME *name)
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
        return false;
}

/*** Initialization ***/

void emsInit(void)
{
    emsEC = 0;
    emsFrameSeg = 0;
    emsFramePtr = NULL;
    ems_hlist = NULL;
    emsInstalled = emsIsInstalled();
    if (emsInstalled)
    {
        if (emsGetVersion())
        {
            emsFrameSeg = emsGetFrameSeg();
            emsFramePtr = MK_FP(emsFrameSeg, 0);
            DEBUG_MSG_("emsInit", "Found EMS version %hu.%03hu.", emsVersion.Hi, emsVersion.Lo);
            DEBUG_MSG_("emsInit", "%lu KiB of expanded memory available.", (uint32_t)emsGetFreePagesCount()<<4);
        }
        else
        {
            emsInstalled = false;
            DEBUG_ERR("emsInit", "Failed to get EMS version.");
        }
    }
    #ifdef DEBUG
    if (!emsInstalled)
    {
        DEBUG_MSG("emsInit", "No EMS is available.");
    }
    #endif
}

void emsDone(void)
{
    while (ems_hlist != NULL)
        emsFree(ems_hlist->handle);
}

DEFINE_REGISTRATION(ems, emsInit, emsDone)
