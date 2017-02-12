/* emstool.c -- DOS Expanded Memory Manager interface.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#endif

#include "..\pascal\pascal.h"
#include "..\watcomc\malloc.h"
#include "..\watcomc\stdio.h"
#include "..\watcomc\stdlib.h"
#include "emstool.h"

#define DOS_DRIVER_NAME_OFF 10
typedef char DOSDriverName_t[8];
const static DOSDriverName_t EMMDriverName = "EMMXXXX0";

typedef struct EMMErrorDesc_t
{
    EMMError_t code;
    char *msg;
};

const static struct EMMErrorDesc_t EMM_ERRORS[] =
{
    { 0x80, "Internal EMM driver error" },
    { 0x81, "EMM hardware failure" },
    { 0x83, "Unknown EMM handle" },
    { 0x84, "This EMM-function does not exist" },
    { 0x85, "No more free EMM handles" },
    { 0x86, "Error with save/restore mapping" },
    { 0x87, "More pages requested than available" },
    { 0x88, "No enough free pages" },
    { 0x89, "Zero pages requested" },
    { 0x8a, "Problem with access - this logical page does not belong to this handle" },
    { 0x8b, "Wrong page number" },
    { 0x8c, "Not enough memory to save mapping" },
    { 0x8d, "Mapping already saved" },
    { 0x8e, "Error restoring mapping - was not saved before" },
    { 0, "" }
};

const static char EMM_ERROR_UNKNOWN[] = "Unknown error";

const char * PUBLIC_CODE GetEMMErrorMsg(EMMError_t err)
{
    uint16_t i = 0;
    while (EMM_ERRORS[i].code)
    {
        if (EMM_ERRORS[i].code == err)
            return EMM_ERRORS[i].msg;
        i++;
    };
    return EMM_ERROR_UNKNOWN;
}

/* Initialize for Pascal linker */
static struct EMMHandleEntry_t *handleList = (void *)0;

void __near insert_handle(EMMHandle_t handle)
{
    struct EMMHandleEntry_t *n;
    if (_memmax() < sizeof(struct EMMHandleEntry_t))
        return;
    n = malloc(sizeof(struct EMMHandleEntry_t));
    if (n)
    {
        n->handle = handle;
        n->next = handleList;
        handleList = n;
    };
}

void __near remove_handle(EMMHandle_t handle)
{
    struct EMMHandleEntry_t *i, *h;
    i = NULL;
    h = handleList;
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
            handleList = h->next;
        free(h);
    };
}

bool PUBLIC_CODE CheckEMM(void)
{
    union REGPACK regs;
    DOSDriverName_t *p;
    regs.w.ax = 0x3567;
    intr(0x21, &regs);
    p = MK_FP(regs.w.es, DOS_DRIVER_NAME_OFF);
    return (memcmp(p, EMMDriverName, sizeof(DOSDriverName_t)) == 0);
}

bool PUBLIC_CODE GetEMMVersion(void)
{
    union REGPACK regs;
    regs.w.ax = 0x4600;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        EmsVersion.Lo = 0;
        EmsVersion.Hi = 0;
        return false;
    }
    else
    {
        EmsVersion.Lo = regs.h.al & 0x0f;
        EmsVersion.Hi = regs.h.al >> 4;
        return true;
    };
}

uint16_t PUBLIC_CODE GetEMMFrameSeg(void)
{
    union REGPACK regs;
    regs.w.ax = 0x4100;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return -1;
    }
    else
        return regs.w.bx;
}

uint16_t PUBLIC_CODE EmsFreePages(void)
{
    union REGPACK regs;
    regs.w.ax = 0x4200;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return 0;
    }
    else
        return regs.w.bx;
}

EMMHandle_t PUBLIC_CODE EmsAlloc(uint16_t pages)
{
    union REGPACK regs;
    regs.w.ax = 0x4300;
    regs.w.bx = pages;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return -1;
    }
    else
    {
        insert_handle(regs.w.dx);
        return regs.w.dx;
    };
}

bool PUBLIC_CODE EmsFree(EMMHandle_t handle)
{
    union REGPACK regs;
    regs.w.ax = 0x4500;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return false;
    }
    else
    {
        remove_handle(regs.w.dx);
        return true;
    };
}

bool PUBLIC_CODE EmsMap(EMMHandle_t handle, uint16_t logPage, uint8_t physPage)
{
    union REGPACK regs;
    regs.h.ah = 0x44;
    regs.h.al = physPage;
    regs.w.bx = logPage;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

bool PUBLIC_CODE EmsSaveMap(EMMHandle_t handle)
{
    union REGPACK regs;
    regs.w.ax = 0x4700;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

bool PUBLIC_CODE EmsRestoreMap(EMMHandle_t handle)
{
    union REGPACK regs;
    regs.w.ax = 0x4800;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

uint16_t PUBLIC_CODE EmsGetHandleSize(EMMHandle_t handle)
{
    union REGPACK regs;
    regs.w.ax = 0x4c00;
    regs.w.dx = handle;
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return 0;
    }
    else
        return regs.w.bx;
}

bool PUBLIC_CODE EmsSetHandleName(EMMHandle_t handle, EMMHandleName_t *name)
{
    union REGPACK regs;
    regs.w.ax = 0x5301;
    regs.w.dx = handle;
    regs.w.si = FP_OFF(name);
    regs.w.ds = FP_SEG(name);
    intr(0x67, &regs);
    if (regs.h.ah)
    {
        EmsEC = regs.h.ah;
        return false;
    }
    else
        return true;
}

/*** Initialization ***/

void EMMInit(void)
{
    EmsInstalled = CheckEMM();
    if (EmsInstalled)
    {
        if (GetEMMVersion())
        {
            FrameSEG[0] = GetEMMFrameSeg();
            FrameSEG[1] = FrameSEG[0] + 0x0400;
            FrameSEG[2] = FrameSEG[1] + 0x0400;
            FrameSEG[3] = FrameSEG[2] + 0x0400;
            FramePTR[0] = MK_FP(FrameSEG[0], 0);
            FramePTR[1] = MK_FP(FrameSEG[1], 0);
            FramePTR[2] = MK_FP(FrameSEG[2], 0);
            FramePTR[3] = MK_FP(FrameSEG[3], 0);
        };
    };
    handleList = NULL;
}

void EMMDone(void)
{
    while (handleList != NULL)
        EmsFree(handleList->handle);
}

DEFINE_REGISTRATION(emstool, EMMInit, EMMDone)
