/* debug.c -- simple library for logging debug messages.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "pascal/pascal.h"

#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdlib.h"
#include "cc/stdio.h"
#include "cc/string.h"

#include "debug.h"

#ifdef DEFINE_LOCAL_DATA

FILE *debuglogfile;

#else

extern FILE *PUBLIC_DATA debuglogfile;

#endif

static const struct
{
    uint8_t textcolor;
    char tag;
} msg_tags[] =
{
    { 7,  0 },
    { 3,  'i' },
    { 6,  'W' },
    { 12, 'E' }
};

#define MAX_TAG_LENGTH 64

void __far _DEBUG_LOG(const int type, const char *file, int line, const char *method, const char *format, ...)
{
    va_list ap;
    const char *s;
    int len;
    char textcol, tag, _tag[MAX_TAG_LENGTH];

    va_start(ap, format);

    _tag[0] = 0;
    _tag[MAX_TAG_LENGTH - 1] = 0;
    len = 0;

    switch (type)
    {
        case DBGLOG_MSG:
        case DBGLOG_INFO:
        case DBGLOG_WARN:
        case DBGLOG_ERR:
            if (!debuglogfile)
                textcol = msg_tags[type].textcolor;
            tag = msg_tags[type].tag;
            if (tag)
            {
                cc_snprintf(_tag, MAX_TAG_LENGTH - 1, "(%c)", tag);
                len = strlen(_tag);
            }
            break;
        default:
            textcol = 7;
            break;
    };

    if (len < MAX_TAG_LENGTH - 1)
        if (file)
        {
            if (line)
                cc_snprintf(_tag + len, MAX_TAG_LENGTH - len - 1,
                    (len > 0) ? " [%s:%d]" : "[%s: %d]", file, line);
            else
                cc_snprintf(_tag + len, MAX_TAG_LENGTH - len - 1,
                    (len > 0) ? " [%s]" : "[%s]", file);
            len = strlen(_tag);
        };

    if (len < MAX_TAG_LENGTH - 1)
        if (method)
        {
            cc_snprintf(_tag + len, MAX_TAG_LENGTH - len - 1,
                (len > 0) ? " %s()" : "%s()", method);
            len = strlen(_tag);
        };

    if (len)
    {
        s = format ? "%s: " : "%s";
        if (debuglogfile)
            cc_fprintf(debuglogfile, s, _tag);
        else
        {
            textcolor(7);
            cc_printf(s, _tag);
        }
    };

    if (format)
    {
        if (debuglogfile)
            cc_vfprintf(debuglogfile, format, ap);
        else
        {
            textcolor(textcol);
            cc_vprintf(format, ap);
        }
    };

    va_end(ap);

    s = CRLF;
    if (debuglogfile)
        cc_fprintf(debuglogfile, s);
    else
    {
        textcolor(7);
        cc_printf(s);
    }
}

void __far _DEBUG_BEGIN(const char *file, int line, const char *method)
{
    _DEBUG_LOG(DBGLOG_INFO, file, line, method, "Begin.");
}

void __far _DEBUG_END(const char *file, int line, const char *method)
{
    _DEBUG_LOG(DBGLOG_INFO, file, line, method, "End.");
}

void __far _DEBUG_FAIL(const char *file, int line, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_ERR, file, line, method, "Return: Failed (%s).", msg);
}

void __far _DEBUG_SUCCESS(const char *file, int line, const char *method)
{
    _DEBUG_LOG(DBGLOG_INFO, file, line, method, "Return: Success.");
}

void __far _DEBUG_REG(const char *file, const char *method, const char *name)
{
    _DEBUG_LOG(DBGLOG_INFO, file, 0, method, "Registering module <%s>...", name);
}

void __far _DEBUG_UNREG(const char *file, const char *method, const char *name)
{
    _DEBUG_LOG(DBGLOG_INFO, file, 0, method, "Unregistering module <%s>...", name);
}

void PUBLIC_CODE Debug_Msg(const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_MSG, file, 0, method, msg);
}

void PUBLIC_CODE Debug_Info(const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_INFO, file, 0, method, msg);
}

void PUBLIC_CODE Debug_Warn(const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_WARN, file, 0, method, msg);
}

void PUBLIC_CODE Debug_Err(const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_ERR, file, 0, method, msg);
}

void PUBLIC_CODE Debug_Begin(const char *file, const char *method)
{
    _DEBUG_BEGIN(file, 0, method);
}

void PUBLIC_CODE Debug_End(const char *file, const char *method)
{
    _DEBUG_END(file, 0, method);
}

void PUBLIC_CODE Debug_Fail(const char *file, const char *method, const char *msg)
{
    _DEBUG_FAIL(file, 0, method, msg);
}

void PUBLIC_CODE Debug_Success(const char *file, const char *method)
{
    _DEBUG_SUCCESS(file, 0, method);
}

/*** Initialization ***/

void debugInit(void)
{
    //debuglogfile = fopen("debug.log", "wb");
    debuglogfile = NULL;
    DEBUG_INFO("debugInit", "Start logging.");
}

void debugDone(void)
{
    DEBUG_INFO("debugDone", "End logging.");
    if (debuglogfile)
        fclose(debuglogfile);
}

DEFINE_REGISTRATION(debug, debugInit, debugDone)
