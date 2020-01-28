/* _env.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$_env$*"
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "common.h"
#include "sysdbg.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "cc/errno.h"
#include "cc/malloc.h"
#include "cc/stdlib.h"
#include "cc/stdlib/_env.h"

/* For the case if environment is not initialized */
static char *_env_list_empty = NULL;

#if DEFINE_LOCAL_DATA == 1

struct dosenvlist_t _dos_env;
struct envstrlist_t _env_list;
char **cc_environ;

#endif  /* DEFINE_LOCAL_DATA == 1 */

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
            (*count)++;
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

    #if SYSDEBUG_ENV == 1
    SYSDEBUG_BEGIN ();
    #endif  /* SYSDEBUG_ENV == 1 */
    if (self)
    {
        if (count)
        {
            if (!_dos_allocmem(_dos_para(sizeof(char *) * count), &seg))
            {
                self->arr = MK_FP(seg, 0);
                self->size = count;
                #if SYSDEBUG_ENV == 1
                SYSDEBUG_SUCCESS ();
                #endif  /* SYSDEBUG_ENV == 1 */
                return true;
            }
            else
            {
                cc_errno = CC_ENOMEM;
                #if SYSDEBUG_ENV == 1
                SYSDEBUG_ERR ("_dos_allocmem(): failed.");
                #endif  /* SYSDEBUG_ENV == 1 */
                return false;
            }
        }
        else
        {
            cc_errno = CC_EINVAL;
            #if SYSDEBUG_ENV == 1
            SYSDEBUG_ERR ("self is NULL.");
            #endif  /* SYSDEBUG_ENV == 1 */
            return false;
        }
    }
    else
    {
        cc_errno = CC_EINVAL;
        #if SYSDEBUG_ENV == 1
        SYSDEBUG_ERR ("count is 0.");
        #endif  /* SYSDEBUG_ENV == 1 */
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
            p[0] = &(dosenv->arr[i]);
            p++;
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

    #if SYSDEBUG_ENV == 1
    SYSDEBUG_BEGIN ();
    #endif  /* SYSDEBUG_ENV == 1 */
    _dosenv_find_end(&_dos_env, &count);
    if (count)
    {
        count++;
        #if SYSDEBUG_ENV == 1
        SYSDEBUG_INFO_ ("count=%d", count);
        #endif  /* SYSDEBUG_ENV == 1 */
        if (!_environ_alloc(&_env_list, count))
        {
            #if SYSDEBUG_ENV == 1
            SYSDEBUG_ERR ("Failed.");
            #endif  /* SYSDEBUG_ENV == 1 */
            return false;
        }
        _environ_fill(&_env_list, &_dos_env);
        cc_environ_sync();
    }
    else
        cc_environ_clear();

    #if SYSDEBUG_ENV == 1
    SYSDEBUG_SUCCESS ();
    #endif  /* SYSDEBUG_ENV == 1 */
    return true;
}

bool cc_environ_rebuild(void)
{
    bool status;
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_BEGIN ();
    #endif  /* SYSDEBUG_ENV == 1 */
    _environ_free(&_env_list);
    status = cc_environ_build ();
    #if SYSDEBUG_ENV == 1
    if (status)
        SYSDEBUG_SUCCESS ();
    else
        SYSDEBUG_ERR ("Failed.");
    #endif  /* SYSDEBUG_ENV == 1 */
    return status;
}

/*** Initialization ***/

bool cc_environ_init(void)
{
    bool status;
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_BEGIN ();
    #endif  /* SYSDEBUG_ENV == 1 */
    _environ_clear(&_env_list);
    status = cc_environ_build ();
    #if SYSDEBUG_ENV == 1
    if (status)
        SYSDEBUG_SUCCESS ();
    else
        SYSDEBUG_ERR ("Failed.");
    #endif  /* SYSDEBUG_ENV == 1 */
    return status;
}

void cc_environ_free(void)
{
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_BEGIN ();
    #endif  /* SYSDEBUG_ENV == 1 */
    _environ_free(&_env_list);
    cc_environ_sync();
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_END ();
    #endif  /* SYSDEBUG_ENV == 1 */
}

/*static void *_oldexit_environ = NULL;*/

void __far environ_done(void);

bool environ_init(void)
{
    bool status;
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_BEGIN ();
    SYSDEBUG_INFO_ ("_memmax()=%ld", (uint32_t) _memmax ());
    #endif  /* SYSDEBUG_ENV == 1 */
    cc_atexit(&environ_done);
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_INFO_ ("_psp=%04X, _masterpsp=%04X", _cc_psp, _cc_dos_getmasterpsp ());
    #endif  /* SYSDEBUG_ENV == 1 */
//~ #if LINKER_TPC == 1
    //~ _cc_psp = _cc_dos_getmasterpsp ();
//~ #endif  /* LINKER_TPC == 1 */
    _psp_get_dosenv (_cc_psp, &_dos_env);
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_INFO_ ("_dos_env: .arr=%04X:%04X, .size=%d",
        FP_SEG (_dos_env.arr), FP_OFF (_dos_env.arr), _dos_env.size);
    #endif  /* SYSDEBUG_ENV == 1 */
    status = cc_environ_init ();
    #if SYSDEBUG_ENV == 1
    if (status)
        SYSDEBUG_SUCCESS ();
    else
        SYSDEBUG_ERR ("Failed.");
    #endif  /* SYSDEBUG_ENV == 1 */
    return status;
}

void __far environ_done(void)
{
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_BEGIN ();
    #endif  /* SYSDEBUG_ENV == 1 */
    cc_environ_free();
    /*exitproc = _oldexit_environ;*/
    #if SYSDEBUG_ENV == 1
    SYSDEBUG_END ();
    #endif  /* SYSDEBUG_ENV == 1 */
}
