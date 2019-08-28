/* _env.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$_env$*"
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "common.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "cc/errno.h"
#include "cc/stdlib.h"
#include "cc/stdlib/_env.h"

/* For the case if environment is not initialized */
static char *_env_list_empty = NULL;

/* DOS program segment prefix */

void _psp_get_dosenv(uint16_t self, struct dosenvlist_t *env)
{
    struct cc_dospsp_t *p;

    p = MK_FP(self, 0);
    env->arr = MK_FP(p->env_seg, 0);
    env->size = ((struct cc_dosmcb_t *)(MK_FP(p->env_seg - 1, 0)))->size * 16;
}

/* DOS environment */

bool _dosenv_next(struct dosenvlist_t *self, uint16_t *i)
{
    char *p;
    uint16_t len;

    p = self->arr;
    len = strlen(&(p[*i]));
    if (len)
    {
        *i += len + 1;
        return true;
    }
    else
        return false;
}

uint16_t _dosenv_find_end(struct dosenvlist_t *self, uint16_t *count)
{
    uint16_t i;
    bool stop;

    i = 0;
    stop = false;
    *count = 0;
    do
    {
        if (_dosenv_next(self, &i))
            *count++;
        else
            stop = true;
    } while (!stop);
    return i;
}

char *_dosenv_find(struct dosenvlist_t *self, const char *name, uint16_t nlen)
{
    uint16_t i;
    char *s;

    i = 0;
    do
    {
        s = &(self->arr[i]);
        if (!_dosenv_next(self, &i))
            return NULL;
        if ((memcmp(name, s, nlen) == 0) && (s[nlen] == '='))
            return s;
    } while (true); /* FIXME: weird? */
}

/* Returns zero (false) on success */
bool _env_name_check(const char *name, uint16_t *nlen)
{
    if (!name)
    {
        cc_errno = CC_EINVAL;
        return  true;
    }
    *nlen = strlen(name);
    if (!*nlen)
    {
        cc_errno = CC_EINVAL;
        return true;
    }
    if (strchr(name, '='))
    {
        cc_errno = CC_EINVAL;
        return true;
    }
    return false;
}

/* Internal environment variables list */

void _environ_clear(struct envstrlist_t *self)
{
    if (self)
    {
        self->arr = &_env_list_empty;
        self->size = 1;
    }
}

bool _environ_alloc(struct envstrlist_t *self, uint16_t count)
{
    uint16_t seg;

    if (self)
    {
        if (count)
        {
            if (_dos_allocmem(_dos_para(sizeof(char *) * count), &seg))
            {
                self->arr = MK_FP(seg, 0);
                self->size = count;
                return true;
            }
            else
            {
                cc_errno = CC_ENOMEM;
                return false;
            }
        }
        else
        {
            cc_errno = CC_EINVAL;
            return false;
        }
    }
    else
    {
        cc_errno = CC_EINVAL;
        return false;
    }
}

void _environ_fill(struct envstrlist_t *self, struct dosenvlist_t *dosenv)
{
    char **p;
    uint16_t count, i;

    if (self && self->arr)
    {
        p = self->arr;
        count = self->size;
        i = 0;
        do
        {
            *p[0] = dosenv->arr[i];
            p = &(p[1]);
            count--;
        } while ((count != 1) && _dosenv_next(dosenv, &i));
        *p = NULL;
    }
}

void _environ_free(struct envstrlist_t *self)
{
    if (self && self->arr && self->arr != &_env_list_empty)
    {
        _dos_freemem(FP_SEG(self->arr));
        _environ_clear(self);
    }
}

/* Semi-public environment variables list */

void cc_environ_sync(void)
{
    cc_environ = _env_list.arr;
}

void cc_environ_clear(void)
{
    _environ_clear(&_env_list);
    cc_environ_sync();
}

bool cc_environ_build(void)
{
    uint16_t count;

    _dosenv_find_end(&_dos_env, &count);
    if (count)
    {
        count++;
        if (!_environ_alloc(&_env_list, count))
            return false;
        _environ_fill(&_env_list, &_dos_env);
        cc_environ_sync();
    }
    else
        cc_environ_clear();

    return true;
}

bool cc_environ_rebuild(void)
{
    _environ_free(&_env_list);
    return cc_environ_build();
}

bool cc_environ_init(void)
{
    _environ_clear(&_env_list);
    return cc_environ_build();
}

void cc_environ_free(void)
{
    _environ_free(&_env_list);
    cc_environ_sync();
}

/*** Initialization ***/

/*static void *_oldexit_environ = NULL;*/

void __far environ_done(void);

bool environ_init(void)
{
    cc_atexit(&environ_done);

    _cc_psp = _dos_getmasterpsp ();
    _psp_get_dosenv (_cc_psp, &_dos_env);

    return cc_environ_init();
}

void __far environ_done(void)
{
    cc_environ_free();
    /*exitproc = _oldexit_environ;*/
}
