/* sysdbg.c -- simple library for logging debug messages to stdout.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$sysdbg$*"
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/stdio.h"
#include "cc/io.h"
#include "commdbg.h"
#include "sysdbg.h"

#define MAX_TAG_LENGTH 64

static const struct
{
    char tag;
} msg_tags[] =
{
    { 0 },
    { 'i' },
    { 'W' },
    { 'E' }
};

#define BUF_LEN 256

void __far _SYSDEBUG_LOG (const int type, const char *file, int line, const char *method, const char *format, ...)
{
    va_list ap;
    const char *s;
    int len;
    char tag, _tag[MAX_TAG_LENGTH];
    char buf[BUF_LEN];

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
            tag = msg_tags[type].tag;
            if (tag)
            {
                cc_snprintf (_tag, MAX_TAG_LENGTH - 1, "(%c)", tag);
                len = strlen (_tag);
            }
            break;
        default:
            break;
    }

    if (len < MAX_TAG_LENGTH - 1)
        if (file)
        {
            if (line)
                cc_snprintf (_tag + len, MAX_TAG_LENGTH - len - 1,
                    (len > 0) ? " [%s:%d]" : "[%s: %d]", file, line);
            else
                cc_snprintf (_tag + len, MAX_TAG_LENGTH - len - 1,
                    (len > 0) ? " [%s]" : "[%s]", file);
            len = strlen (_tag);
        }

    if (len < MAX_TAG_LENGTH - 1)
        if (method)
        {
            cc_snprintf (_tag + len, MAX_TAG_LENGTH - len - 1,
                (len > 0) ? " %s()" : "%s()", method);
            len = strlen (_tag);
        }

    if (len)
    {
        s = format ? "%s: " : "%s";
        cc_snprintf (buf, BUF_LEN, s, _tag);
    }
    else
        buf[0] = 0;

    if (format)
    {
        len = strlen (buf);
        if (len + 1 < BUF_LEN)
            cc_vsnprintf (& (buf[len]), BUF_LEN - len, format, ap);
    }
    len = strlen (buf);
    if (len)
        cc_write (0, buf, len);

    va_end(ap);

    s = CRLF;
    cc_write (0, (void *) s, strlen (s));
}

void __far _SYSDEBUG_BEGIN (const char *file, int line, const char *method)
{
    _SYSDEBUG_LOG (DBGLOG_INFO, file, line, method, "Begin.");
}

void __far _SYSDEBUG_END (const char *file, int line, const char *method)
{
    _SYSDEBUG_LOG (DBGLOG_INFO, file, line, method, "End.");
}

void __far _SYSDEBUG_SUCCESS (const char *file, int line, const char *method)
{
    _SYSDEBUG_LOG (DBGLOG_INFO, file, line, method, "End (success).");
}

#define LINE_SIZE 16

void __far _SYSDEBUG_dump_mem (void *buf, unsigned size, const char *padstr)
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
            _SYSDEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL, "%s%s", padstr, s);
        else
            _SYSDEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL, "%s", s);
        p += len;
        o += len;
        left -= len;
    }
}
