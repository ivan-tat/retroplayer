/* pic.h -- declarations for pic.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PIC_H_INCLUDED
#define _PIC_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "hw/hwowner.h"

#define IRQ_CHANNELS 16

typedef uint16_t irq_mask_t;
typedef irq_mask_t IRQMASK;

/* Sharing IRQ channels */

typedef void __far isr_owner_cb_t(void *data, uint8_t ch);
typedef isr_owner_cb_t ISROWNERCALLBACK;

/* Public methods */

IRQMASK           pic_get_hooked_irq_channels(void);
HWOWNERID         pic_get_irq_owner(uint8_t ch);
ISROWNERCALLBACK *pic_get_irq_handler(uint8_t ch);
void             *pic_get_irq_data(uint8_t ch);

/* HWOWNER methods */

bool hwowner_enable_irq(HWOWNER *self, uint8_t ch);
bool hwowner_enable_irq_channels(HWOWNER *self, IRQMASK mask);
bool hwowner_disable_irq(HWOWNER *self, uint8_t ch);
bool hwowner_disable_irq_channels(HWOWNER *self, IRQMASK mask);
bool hwowner_eoi(HWOWNER *self, uint8_t ch);
bool hwowner_hook_irq(HWOWNER *self, uint8_t ch, ISROWNERCALLBACK *handler, void *data);
bool hwowner_hook_irq_channels(HWOWNER *self, IRQMASK mask, ISROWNERCALLBACK *handler, void *data);
bool hwowner_set_irq_handler(HWOWNER *self, uint8_t ch, ISROWNERCALLBACK *handler, void *data);
bool hwowner_set_irq_channels_handler(HWOWNER *self, IRQMASK mask, ISROWNERCALLBACK *handler, void *data);
bool hwowner_clear_irq_handler(HWOWNER *self, uint8_t ch);
bool hwowner_clear_irq_channels_handler(HWOWNER *self, IRQMASK mask);
bool hwowner_release_irq(HWOWNER *self, uint8_t ch);
bool hwowner_release_irq_channels(HWOWNER *self, IRQMASK mask);

/*** Initialization ***/

DECLARE_REGISTRATION (pic)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux pic_get_hooked_irq_channels "*";
#pragma aux pic_get_irq_owner "*";
#pragma aux pic_get_irq_handler "*";
#pragma aux pic_get_irq_data "*";

#pragma aux hwowner_enable_irq "*";
#pragma aux hwowner_enable_irq_channels "*";
#pragma aux hwowner_disable_irq "*";
#pragma aux hwowner_disable_irq_channels "*";
#pragma aux hwowner_eoi "*";
#pragma aux hwowner_hook_irq "*";
#pragma aux hwowner_hook_irq_channels "*";
#pragma aux hwowner_set_irq_handler "*";
#pragma aux hwowner_set_irq_channels_handler "*";
#pragma aux hwowner_clear_irq_handler "*";
#pragma aux hwowner_clear_irq_channels_handler "*";
#pragma aux hwowner_release_irq "*";
#pragma aux hwowner_release_irq_channels "*";

#pragma aux register_pic "*";
#pragma aux unregister_pic "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PIC_H_INCLUDED */
