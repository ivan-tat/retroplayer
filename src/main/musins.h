/* musins.h -- declarations for musins.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MUSINS_H
#define MUSINS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "dos/ems.h"
#include "dynarray.h"

/*** Musical instrument ***/

typedef uint8_t music_instrument_type_t;
typedef music_instrument_type_t MUSINSTYPE;

#define MUSINST_EMPTY 0
#define MUSINST_PCM   1
#define MUSINST_ADLIB 2

#define SMPFLAG_LOOP 0x01

#define MUSINS_TITLE_LENGTH_MAX 28
#pragma pack(push, 1);
typedef struct instrument_t
{
    MUSINSTYPE bType;
    uint8_t  DOSname[12];
    uint8_t  unused1;
    uint16_t memseg;        // segment of sampledata
    uint16_t slength;       // length of sample <= 64KB
    uint16_t HI_length;     // <- not used !
    uint16_t loopbeg;       // loop begin <= 64KB
    uint16_t HI_loopbeg;    // <- not used !
    uint16_t loopend;       // loop end <= 64KB
    uint16_t HI_loopend;    // <- not used !
    uint8_t  vol;           // default instrument volume
    uint8_t  unused2;
    uint8_t  packinfo;      // =0 or we can't play it !
    uint8_t  flags;         // bit 0: loop (all other bits are ignored)
    uint16_t c2speed;       // frequency at middle C (actually C-4)
    uint16_t c2speed_hi;    // ignored
    uint8_t  unused3[12];
    uint8_t  IName[MUSINS_TITLE_LENGTH_MAX];
    uint32_t SCRS_ID;
};
#pragma pack(pop);
typedef struct instrument_t MUSINS;

typedef struct instrument_t PCMSMP;

void       PUBLIC_CODE musins_init(MUSINS *self);
void       PUBLIC_CODE musins_set_type(MUSINS *self, MUSINSTYPE value);
MUSINSTYPE PUBLIC_CODE musins_get_type(MUSINS *self);
void       PUBLIC_CODE musins_set_looped(MUSINS *self, bool value);
bool       PUBLIC_CODE musins_is_looped(MUSINS *self);
void       PUBLIC_CODE musins_set_EM_data(MUSINS *self, bool value);
bool       PUBLIC_CODE musins_is_EM_data(MUSINS *self);
void       PUBLIC_CODE musins_set_EM_data_page(MUSINS *self, uint16_t value);
uint16_t   PUBLIC_CODE musins_get_EM_data_page(MUSINS *self);
void       PUBLIC_CODE musins_set_data(MUSINS *self, void *value);
void      *PUBLIC_CODE musins_get_data(MUSINS *self);
void      *PUBLIC_CODE musins_map_EM_data(MUSINS *self);
void       PUBLIC_CODE musins_set_length(MUSINS *self, uint32_t value);
uint32_t   PUBLIC_CODE musins_get_length(MUSINS *self);
void       PUBLIC_CODE musins_set_loop_start(MUSINS *self, uint32_t value);
uint32_t   PUBLIC_CODE musins_get_loop_start(MUSINS *self);
void       PUBLIC_CODE musins_set_loop_end(MUSINS *self, uint32_t value);
uint32_t   PUBLIC_CODE musins_get_loop_end(MUSINS *self);
void       PUBLIC_CODE musins_set_volume(MUSINS *self, uint8_t value);
uint8_t    PUBLIC_CODE musins_get_volume(MUSINS *self);
void       PUBLIC_CODE musins_set_rate(MUSINS *self, uint32_t value);
uint32_t   PUBLIC_CODE musins_get_rate(MUSINS *self);
void       PUBLIC_CODE musins_set_title(MUSINS *self, char *value);
char      *PUBLIC_CODE musins_get_title(MUSINS *self);
void       PUBLIC_CODE musins_free(MUSINS *self);

/*** Musical instruments list ***/

#define MAX_INSTRUMENTS 99
    /* 1..99 samples */

typedef MUSINS instrumentsList_t[MAX_INSTRUMENTS];
typedef instrumentsList_t MUSINSLIST;

MUSINSLIST *PUBLIC_CODE musinsl_new(void);
void        PUBLIC_CODE musinsl_init(MUSINSLIST *self);
void        PUBLIC_CODE musinsl_set_EM_data(MUSINSLIST *self, bool value);
bool        PUBLIC_CODE musinsl_is_EM_data(MUSINSLIST *self);
MUSINS     *PUBLIC_CODE musinsl_get(MUSINSLIST *self, uint16_t index);
void        PUBLIC_CODE musinsl_set_EM_data_handle(MUSINSLIST *self, EMSHDL value);
void        PUBLIC_CODE musinsl_set_EM_handle_name(MUSINSLIST *self);
uint32_t    PUBLIC_CODE musinsl_get_used_EM(MUSINSLIST *self);
void        PUBLIC_CODE musinsl_free(MUSINSLIST *self);
void        PUBLIC_CODE musinsl_delete(MUSINSLIST **self);

/*** Variables ***/

extern instrumentsList_t *mod_Instruments;  /* pointer to data for all instruments */
extern uint16_t mod_InstrumentsCount;
extern bool     mod_Samples_EMData;     /* samples in EMS ? */
extern uint16_t mod_Samples_EMHandle;   /* hanlde to access EMS for samples */

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux musins_init "*";
#pragma aux musins_set_type "*";
#pragma aux musins_get_type "*";
#pragma aux musins_set_looped "*";
#pragma aux musins_is_looped "*";
#pragma aux musins_set_EM_data "*";
#pragma aux musins_is_EM_data "*";
#pragma aux musins_set_EM_data_page "*";
#pragma aux musins_get_EM_data_page "*";
#pragma aux musins_set_data "*";
#pragma aux musins_get_data "*";
#pragma aux musins_map_EM_data "*";
#pragma aux musins_set_length "*";
#pragma aux musins_get_length "*";
#pragma aux musins_set_loop_start "*";
#pragma aux musins_get_loop_start "*";
#pragma aux musins_set_loop_end "*";
#pragma aux musins_get_loop_end "*";
#pragma aux musins_set_volume "*";
#pragma aux musins_get_volume "*";
#pragma aux musins_set_rate "*";
#pragma aux musins_get_rate "*";
#pragma aux musins_set_title "*";
#pragma aux musins_get_title "*";
#pragma aux musins_free "*";

#pragma aux musinsl_new "*";
#pragma aux musinsl_init "*";
#pragma aux musinsl_set_EM_data "*";
#pragma aux musinsl_is_EM_data "*";
#pragma aux musinsl_get "*";
#pragma aux musinsl_set_EM_data_handle "*";
#pragma aux musinsl_set_EM_handle_name "*";
#pragma aux musinsl_get_used_EM "*";
#pragma aux musinsl_free "*";
#pragma aux musinsl_delete "*";

#pragma aux mod_Instruments "*";
#pragma aux mod_InstrumentsCount "*";
#pragma aux mod_Samples_EMData "*";
#pragma aux mod_Samples_EMHandle "*";

#endif  /* __WATCOMC__ */

#endif  /* MUSINS_H */
