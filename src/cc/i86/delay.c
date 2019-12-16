/* delay.c -- part of custom "i86" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$i86$delay$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "sysdbg.h"
#include "hw/bios.h"
#include "cc/i86.h"

#ifdef DEFINE_LOCAL_DATA

uint64_t _cc_delay_base_ticks;

#endif  /* DEFINE_LOCAL_DATA */

/* 55 milliseconds per one timer tick (configured by BIOS).
   One IRQ call per 65536 ticks of total 1193180 timer's ticks per second. */
#define TIMER_PERIOD 55

void __near _delay (uint32_t *ticks, uint64_t *counter);

void cc_delay_init (void)
{
    uint32_t *t, t0;
    uint64_t base_ticks;
    SYSDEBUG_BEGIN ();
    /* wait for new tick */
    t = get_BIOS_data_area_var_ptr (uint32_t, timer_ticks_counter);
    t0 = *t;
    do {} while (t0 == *t);
    /* calculate our internal loop cycles count between timer's interrupt calls */
    base_ticks = ~0ULL;
    _delay (t, &base_ticks);
    _cc_delay_base_ticks = ~base_ticks;
}

/* Wait until timer's interrupt is called or counter expired */
void __near _delay (uint32_t *ticks, uint64_t *counter)
{
    uint32_t t0;
    t0 = *ticks;
    do
    {
        (*counter)--;
        if (!*counter) break;
    } while (t0 == *ticks);
}

void cc_delay(unsigned int __milliseconds)
{
    unsigned int n, c;
    uint32_t *t, t0, t1;
    uint64_t base_ticks;
    /* FIXME: does not work
    union CC_REGPACK regs;
    unsigned long ms;

    ms = (unsigned long)__milliseconds * 1000;
    regs.h.ah = 0x86;
    regs.w.cx = ms & 0xffff;
    regs.w.dx = ms >> 16;
    cc_intr(0x15, &regs);
    */
    if (__milliseconds)
    {
        n = __milliseconds / TIMER_PERIOD;
        c = __milliseconds % TIMER_PERIOD;
        t = get_BIOS_data_area_var_ptr (uint32_t, timer_ticks_counter);
        if (n)
        {
            /* wait for (TIMER_PERIOD * n) milliseconds */
            t0 = *t;
            do t1 = *t; while (t1 - t0 < n);
        }
        if (c)
        {
            /* wait for (c) milliseconds */
            base_ticks = _cc_delay_base_ticks * c / TIMER_PERIOD;
            _delay (t, &base_ticks);
        }
    }
}
