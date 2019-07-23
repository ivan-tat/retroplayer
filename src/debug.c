/* debug.c -- simple library for logging debug messages.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$debug$*"
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdlib.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "debug.h"

#ifdef DEFINE_LOCAL_DATA

FILE *debuglogfile;

#endif  /* DEFINE_LOCAL_DATA */

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

static const char _hexdigits[16] = "0123456789ABCDEF";

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
            #if DEBUG_WRITE_LOG == 1
            if (!debuglogfile)
                textcol = msg_tags[type].textcolor;
            #else
            textcol = msg_tags[type].textcolor;
            #endif
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
    }

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
        }

    if (len < MAX_TAG_LENGTH - 1)
        if (method)
        {
            cc_snprintf(_tag + len, MAX_TAG_LENGTH - len - 1,
                (len > 0) ? " %s()" : "%s()", method);
            len = strlen(_tag);
        }

    if (len)
    {
        s = format ? "%s: " : "%s";
        #if DEBUG_WRITE_LOG == 1
        if (debuglogfile)
            cc_fprintf(debuglogfile, s, _tag);
        else
        {
            textcolor(7);
            cc_printf(s, _tag);
        }
        #else
        textcolor (7);
        cc_printf (s, _tag);
        #endif
    }

    if (format)
    {
        #if DEBUG_WRITE_LOG == 1
        if (debuglogfile)
            cc_vfprintf(debuglogfile, format, ap);
        else
        {
            textcolor(textcol);
            cc_vprintf(format, ap);
        }
        #else
        textcolor (textcol);
        cc_vprintf (format, ap);
        #endif
    }

    va_end(ap);

    s = CRLF;
    #if DEBUG_WRITE_LOG == 1
    if (debuglogfile)
        cc_fprintf(debuglogfile, s);
    else
    {
        textcolor(7);
        cc_printf(s);
    }
    #else
    textcolor (7);
    cc_printf (s);
    #endif
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

/*
 * Example:
 *      #define LEN 8
 *      char s [LEN + 1];
 *      _DEBUG_get_xnum (0xBAAD5EED, LEN, s);
 *      // s = "BAAD5EED";
 *
 *  Description:
 *      Destination must be of the size (len + 1) bytes to hold the result.
 */
void __far _DEBUG_get_xnum (uint32_t value, char len, char *dest)
{
    char *c, count;

    c = & (dest [len - 1]);
    count = len;
    while (count)
    {
        *c = _hexdigits [value & 15];
        value >>= 4;
        c--;
        count--;
    }
    dest[len] = 0;
}

/*
 * Example:
 *      #define MAX 16
 *      char s [MAX * 4 + 1];
 *      _DEBUG_get_xline (_hexdigits, 12, MAX, s);
 *      // s = "30 31 32 33 34 35 36 37 38 39 41 42             0123456789AB";
 *
 *  Description:
 *      Destination must be of the size (max * 4 + 1) bytes to hold the result.
 */
void __far _DEBUG_get_xline (void *buf, uint8_t size, uint8_t max, char *dest)
{
    char *p;
    int i;

    p = (char *) buf;
    i = size;
    while (i)
    {
        _DEBUG_get_xnum (*p, 2, dest);
        dest [2] = ' ';
        p++;
        dest += 3;
        i--;
    }

    if (max - size)
    {
        memset (dest, ' ', (max - size) * 3);
        dest += (max - size) * 3;
    }

    p = (char *) buf;
    i = size;
    while (i)
    {
        *dest = (*p < 32) ? '.' : *p;
        p++;
        dest++;
        i--;
    }

    *dest = 0;
}

#define LINE_SIZE 16
void __far _DEBUG_dump_mem (void *buf, unsigned size, const char *padstr)
{
    const char *p;
    unsigned o, left;
    char len;
    char s[4 + 1 + LINE_SIZE * 4 + 1];

    p = (char *) buf;
    o = 0;
    left = size;
    while (left)
    {
        len = (left > LINE_SIZE) ? LINE_SIZE : left;
        _DEBUG_get_xnum (o, 4, s);
        s[4] = ' ';
        _DEBUG_get_xline ((void *) p, len, LINE_SIZE, & (s [5]));
        if (padstr)
            _DEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL, "%s%s", padstr, s);
        else
            _DEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL, "%s", s);
        p += len;
        o += len;
        left -= len;
    }
}
#undef LINE_SIZE

void __far __pascal Debug_Msg (const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_MSG, file, 0, method, msg);
}

void __far __pascal Debug_Info (const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_INFO, file, 0, method, msg);
}

void __far __pascal Debug_Warn (const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_WARN, file, 0, method, msg);
}

void __far __pascal Debug_Err (const char *file, const char *method, const char *msg)
{
    _DEBUG_LOG(DBGLOG_ERR, file, 0, method, msg);
}

void __far __pascal Debug_Begin (const char *file, const char *method)
{
    _DEBUG_BEGIN(file, 0, method);
}

void __far __pascal Debug_End (const char *file, const char *method)
{
    _DEBUG_END(file, 0, method);
}

void __far __pascal Debug_Fail (const char *file, const char *method, const char *msg)
{
    _DEBUG_FAIL(file, 0, method, msg);
}

void __far __pascal Debug_Success (const char *file, const char *method)
{
    _DEBUG_SUCCESS(file, 0, method);
}

/*** Initialization ***/

void debug_init (void)
{
    #if DEBUG_WRITE_LOG == 1
    debuglogfile = fopen ("debug.log", "wb");
    #endif
    DEBUG_INFO ("debug_init", "Start logging.");
}

void debug_done (void)
{
    DEBUG_INFO ("debug_done", "End logging.");
    #if DEBUG_WRITE_LOG == 1
    if (debuglogfile)
        fclose (debuglogfile);
    #endif
}

DEFINE_REGISTRATION (debug, debug_init, debug_done)
