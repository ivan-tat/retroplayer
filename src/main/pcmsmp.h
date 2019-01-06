/* pcmsmp.h -- declarations for pcmsmp.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PCMSMP_H
#define PCMSMP_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/dos.h"
#include "dos/ems.h"
#include "dynarray.h"
#include "main/s3mtypes.h"

/*** PCM sample ***/

/* Loop type (2 bits) */

typedef uint8_t pcm_sample_loop_t;
typedef pcm_sample_loop_t PCMSMPLOOP;

#define PCMSMPLOOP_NONE     0
#define PCMSMPLOOP_FORWARD  1
#define PCMSMPLOOP_BACKWARD 2
#define PCMSMPLOOP_PINGPONG 3
#define PCMSMPLOOP_MASK     3

/* Flags */

typedef uint8_t pcm_sample_flags_t;
typedef pcm_sample_flags_t PCMSMPFLAGS;

#define PCMSMPFL_AVAIL    (1 << 0)  /* data is allocated */
#define PCMSMPFL_EM       (1 << 1)  /* data is in EM */
#define PCMSMPFL_OWNHDL   (1 << 2)  /* has own EM handle, needs to be freed when done */
#define PCMSMPFL_16BITS   (1 << 3)  /* sample format is 16 bits if set */
#define PCMSMPFL_LOOPSHIFT 4
#define PCMSMPFL_LOOP_NONE     (PCMSMPLOOP_NONE     << PCMSMPFL_LOOPSHIFT)
#define PCMSMPFL_LOOP_FORWARD  (PCMSMPLOOP_FORWARD  << PCMSMPFL_LOOPSHIFT)
#define PCMSMPFL_LOOP_BACKWARD (PCMSMPLOOP_BACKWARD << PCMSMPFL_LOOPSHIFT)
#define PCMSMPFL_LOOP_PINGPONG (PCMSMPLOOP_PINGPONG << PCMSMPFL_LOOPSHIFT)
#define PCMSMPFL_LOOP_MASK     (PCMSMPLOOP_MASK     << PCMSMPFL_LOOPSHIFT)

/* Structure */

#define PCMSMP_TITLE_LEN 16

#pragma pack(push, 1);
typedef struct pcm_sample_t
{
    PCMSMPFLAGS flags;
    uint8_t  volume;
    uint16_t size;
    union
    {
        struct
        {
            uint16_t offset;
            uint16_t segment;
        } dos;
        struct
        {
            uint16_t offset;
            uint16_t page;
            EMSHDL   handle;
        } em;
    } data;
    uint16_t rate;
    uint16_t loop_start;
    uint16_t loop_end;
    char     title[PCMSMP_TITLE_LEN];
};  /* 32 bytes */
#pragma pack(pop);

typedef struct pcm_sample_t PCMSMP;

/* Methods */

PCMSMPFLAGS __far __pcmsmp_set_flags (PCMSMPFLAGS _flags, PCMSMPFLAGS _mask, PCMSMPFLAGS _set, bool raise);

#define _pcmsmp_get_flags(o)                (o)->flags
#define _pcmsmp_set_flags(o, v)             _pcmsmp_get_flags (o) = (v)
#define _pcmsmp_set_available(o, v)         _pcmsmp_set_flags (o, __pcmsmp_set_flags (_pcmsmp_get_flags (o), ~PCMSMPFL_AVAIL, PCMSMPFL_AVAIL, v))
#define _pcmsmp_is_available(o)             ((_pcmsmp_get_flags (o) & PCMSMPFL_AVAIL) != 0)
#define _pcmsmp_set_EM_data(o, v)           _pcmsmp_set_flags (o, __pcmsmp_set_flags (_pcmsmp_get_flags (o), ~PCMSMPFL_EM, PCMSMPFL_EM, v))
#define _pcmsmp_is_EM_data(o)               ((_pcmsmp_get_flags (o) & PCMSMPFL_EM) != 0)
#define _pcmsmp_set_own_EM_handle(o, v)     _pcmsmp_set_flags (o, __pcmsmp_set_flags (_pcmsmp_get_flags (o), ~PCMSMPFL_OWNHDL, PCMSMPFL_OWNHDL, v))
#define _pcmsmp_is_own_EM_handle(o)         ((_pcmsmp_get_flags (o) & PCMSMPFL_OWNHDL) != 0)
#define _pcmsmp_set_16bits(o)               _pcmsmp_set_flags (o, __pcmsmp_set_flags (_pcmsmp_get_flags (o), ~PCMSMPFL_16BITS, PCMSMPFL_16BITS, v))
#define _pcmsmp_get_16bits(o)               ((_pcmsmp_get_flags (o) & PCMSMPFL_16BITS) != 0)
#define _pcmsmp_set_bits(o, v)              _pcmsmp_set_16bits (o, (v) == 16)
#define _pcmsmp_get_bits(o)                 (_pcmsmp_get_16bits (o) ? 16 : 8)
#define _pcmsmp_set_loop(o, v)              _pcmsmp_set_flags (o, __pcmsmp_set_flags (_pcmsmp_get_flags (o), ~PCMSMPFL_LOOP_MASK, ((v) & PCMSMPLOOP_MASK) << PCMSMPFL_LOOPSHIFT, true))
#define _pcmsmp_get_loop(o)                 ((_pcmsmp_get_flags (o) >> PCMSMPFL_LOOPSHIFT) & PCMSMPLOOP_MASK)
#define _pcmsmp_get_size(o)                 (o)->size
#define _pcmsmp_set_size(o, v)              _pcmsmp_get_size (o) = (v)
#define _pcmsmp_get_length(o)               (_pcmsmp_get_16bits (o) ? _pcmsmp_get_size (o) >> 1 : _pcmsmp_get_size (o))
#define _pcmsmp_get_mem_size(o)             (_pcmsmp_get_size (o) + 1024)
#define _pcmsmp_get_data_off(o)             (o)->data.dos.offset
#define _pcmsmp_set_data_off(o, v)          _pcmsmp_get_data_off (o) = (v)
#define _pcmsmp_get_data_seg(o)             (o)->data.dos.segment
#define _pcmsmp_set_data_seg(o, v)          _pcmsmp_get_data_seg (o) = (v)
#define _pcmsmp_get_data(o)                 MK_FP (_pcmsmp_get_data_seg (o), _pcmsmp_get_data_off (o))
#define _pcmsmp_set_data(o, v)              { _pcmsmp_set_data_off (o, v); _pcmsmp_set_data_seg (o, v); }
#define _pcmsmp_get_EM_data_offset(o)       (o)->data.em.offset
#define _pcmsmp_set_EM_data_offset(o, v)    _pcmsmp_set_data_off (o, v)
#define _pcmsmp_get_EM_data_page(o)         (o)->data.em.page
#define _pcmsmp_set_EM_data_page(o, v)      _pcmsmp_get_EM_data_page (o) = (v)
#define _pcmsmp_get_EM_data_handle(o)       (o)->data.em.handle
#define _pcmsmp_set_EM_data_handle(o, v)    _pcmsmp_get_EM_data_handle (o) = (v)
#define _pcmsmp_get_EM_data_pages_count(o)  (uint16_t) ((((uint32_t) _pcmsmp_get_mem_size (o) + 0x3fff) >> 1) >> 13)
#define _pcmsmp_get_EM_data(o)              MK_FP (emsFrameSeg, _pcmsmp_get_EM_data_offset (o))
#define _pcmsmp_get_rate(o)                 (o)->rate
#define _pcmsmp_set_rate(o, v)              _pcmsmp_get_rate (o) = (v)
#define _pcmsmp_get_loop_start(o)           (o)->loop_start
#define _pcmsmp_set_loop_start(o, v)        _pcmsmp_get_loop_start (o) = (v)
#define _pcmsmp_get_loop_end(o)             (o)->loop_end
#define _pcmsmp_set_loop_end(o, v)          _pcmsmp_get_loop_end (o) = (v)
#define _pcmsmp_get_volume(o)               (o)->volume
#define _pcmsmp_set_volume(o, v)            _pcmsmp_get_volume (o) = (v)
#define _pcmsmp_get_title(o)                (o)->title

void  __far pcmsmp_init (PCMSMP *self);
#define     pcmsmp_get_flags(o)                 _pcmsmp_get_flags (o)
#define     pcmsmp_set_flags(o, v)              _pcmsmp_set_flags (o, v)
#define     pcmsmp_set_available(o, v)          _pcmsmp_set_available (o, v)
#define     pcmsmp_is_available(o)              _pcmsmp_is_available (o)
#define     pcmsmp_set_EM_data(o ,v)            _pcmsmp_set_EM_data (o, v)
#define     pcmsmp_is_EM_data(o)                _pcmsmp_is_EM_data (o)
#define     pcmsmp_set_own_EM_handle(o ,v)      _pcmsmp_set_own_EM_handle (o, v)
#define     pcmsmp_is_own_EM_handle(o)          _pcmsmp_is_own_EM_handle (o)
#define     pcmsmp_set_bits(o ,v)               _pcmsmp_set_bits (o, v)
#define     pcmsmp_get_bits(o)                  _pcmsmp_get_bits (o)
#define     pcmsmp_set_loop(o ,v)               _pcmsmp_set_loop (o, v)
#define     pcmsmp_get_loop(o)                  _pcmsmp_get_loop (o)
#define     pcmsmp_set_size(o ,v)               _pcmsmp_set_size (o, v)
#define     pcmsmp_get_size(o)                  _pcmsmp_get_size (o)
#define     pcmsmp_get_length(o)                _pcmsmp_get_length (o)
#define     pcmsmp_get_mem_size(o)              _pcmsmp_get_mem_size (o)
#define     pcmsmp_set_data(o ,v)               _pcmsmp_set_data (o, v)
#define     pcmsmp_get_data(o)                  _pcmsmp_get_data (o)
#define     pcmsmp_set_EM_data_offset(o ,v)     _pcmsmp_set_EM_data_offset (o, v)
#define     pcmsmp_get_EM_data_offset(o)        _pcmsmp_get_EM_data_offset (o)
#define     pcmsmp_set_EM_data_page(o ,v)       _pcmsmp_set_EM_data_page (o, v)
#define     pcmsmp_get_EM_data_page(o)          _pcmsmp_get_EM_data_page (o)
#define     pcmsmp_set_EM_data_handle(o ,v)     _pcmsmp_set_EM_data_handle (o, v)
#define     pcmsmp_get_EM_data_handle(o)        _pcmsmp_get_EM_data_handle (o)
#define     pcmsmp_get_EM_data_pages_count(o)   _pcmsmp_get_EM_data_pages_count (o)
void *__far pcmsmp_map_EM_data (PCMSMP *self);
#define     pcmsmp_set_rate(o, v)               _pcmsmp_set_rate (o, v)
#define     pcmsmp_get_rate(o)                  _pcmsmp_get_rate (o)
#define     pcmsmp_set_loop_start(o, v)         _pcmsmp_set_loop_start (o, v)
#define     pcmsmp_get_loop_start(o)            _pcmsmp_get_loop_start (o)
#define     pcmsmp_set_loop_end(o, v)           _pcmsmp_set_loop_end (o, v)
#define     pcmsmp_get_loop_end(o)              _pcmsmp_get_loop_end (o)
#define     pcmsmp_set_volume(o, v)             _pcmsmp_set_volume (o, v)
#define     pcmsmp_get_volume(o)                _pcmsmp_get_volume (o)
void  __far pcmsmp_set_title (PCMSMP *self, char *value);
char *__far pcmsmp_get_title (PCMSMP *self);
void  __far pcmsmp_free (PCMSMP *self);

/*** PCM samples list ***/

/* Flags */

typedef uint16_t pcm_samples_list_flags_t;
typedef pcm_samples_list_flags_t PCMSMPLFLAGS;

#define PCMSMPLFL_EM     (1<<0) /* data is in EM */
#define PCMSMPLFL_OWNHDL (1<<1) /* has own EM handle, needs to be freed when done */

/* Structure */

#pragma pack(push, 1);
typedef struct pcm_samples_list_t
{
    PCMSMPLFLAGS flags;
    DYNARR list;
    EMSHDL handle;
};
#pragma pack(pop);

typedef struct pcm_samples_list_t PCMSMPLIST;

/* Methods */

PCMSMPLFLAGS __far __pcmsmpl_set_flags (PCMSMPLFLAGS _flags, PCMSMPLFLAGS _mask, PCMSMPLFLAGS _set, bool raise);

#define _pcmsmpl_get_flags(o)               (o)->flags
#define _pcmsmpl_set_flags(o, v)            _pcmsmpl_get_flags (o) = (v)
#define _pcmsmpl_set_EM_data(o, v)          _pcmsmpl_set_flags (o, __pcmsmpl_set_flags (_pcmsmpl_get_flags (o), ~PCMSMPLFL_EM, PCMSMPLFL_EM, v))
#define _pcmsmpl_is_EM_data(o)              ((_pcmsmpl_get_flags (o) & PCMSMPLFL_EM) != 0)
#define _pcmsmpl_set_own_EM_handle(o, v)    _pcmsmpl_set_flags (o, __pcmsmpl_set_flags (_pcmsmpl_get_flags (o), ~PCMSMPLFL_OWNHDL, PCMSMPLFL_OWNHDL, v))
#define _pcmsmpl_is_own_EM_handle(o)        ((_pcmsmpl_get_flags (o) & PCMSMPLFL_OWNHDL) != 0)
#define _pcmsmpl_get_list(o)                & ((o)->list)
#define _pcmsmpl_get_EM_handle(o)           (o)->handle
#define _pcmsmpl_set_EM_handle(o, v)        _pcmsmpl_get_EM_handle (o) = (v)
#define _pcmsmpl_set_EM_handle_name(o, v)   emsSetHandleName (_pcmsmpl_get_EM_handle (o), v)
#define _pcmsmpl_set_count(o, v)            dynarr_set_size (_pcmsmpl_get_list (o), v)
#define _pcmsmpl_get_count(o)               dynarr_get_size (_pcmsmpl_get_list (o))
#define _pcmsmpl_set(o, i, v)               dynarr_set_item (_pcmsmpl_get_list (o), i, v)
#define _pcmsmpl_get(o, i)                  dynarr_get_item (_pcmsmpl_get_list (o), i)
#define _pcmsmpl_indexof(o, v)              dynarr_indexof (_pcmsmpl_get_list (o), v)
#define _pcmsmpl_get_used_EM(o)             ((uint32_t) emsGetHandleSize (_pcmsmpl_get_EM_handle (o)) << 4)

void __far pcmsmpl_init (PCMSMPLIST *self);
#define    pcmsmpl_set_flags(o, v)          _pcmsmpl_set_flags (o, v)
#define    pcmsmpl_get_flags(o)             _pcmsmpl_get_flags (o)
#define    pcmsmpl_set_EM_data(o, v)        _pcmsmpl_set_EM_data (o, v)
#define    pcmsmpl_is_EM_data(o)            _pcmsmpl_is_EM_data (o)
#define    pcmsmpl_set_own_EM_handle(o, v)  _pcmsmpl_set_own_EM_handle (o, v)
#define    pcmsmpl_is_own_EM_handle(o)      _pcmsmpl_is_own_EM_handle (o)
#define    pcmsmpl_set_EM_handle(o, v)      _pcmsmpl_set_EM_handle (o, v)
#define    pcmsmpl_get_EM_handle(o)         _pcmsmpl_get_EM_handle (o)
#define    pcmsmpl_set_EM_handle_name(o, v) _pcmsmpl_set_EM_handle_name (o, v)
#define    pcmsmpl_set_count(o, v)          _pcmsmpl_set_count (o, v)
#define    pcmsmpl_get_count(o)             _pcmsmpl_get_count (o)
#define    pcmsmpl_set(o, i, v)             _pcmsmpl_set (o, i, v)
#define    pcmsmpl_get(o, i)                _pcmsmpl_get (o, i)
#define    pcmsmpl_indexof(o, v)            _pcmsmpl_indexof (o, v)
#define    pcmsmpl_get_used_EM(o)           _pcmsmpl_get_used_EM (o)
void __far pcmsmpl_free (PCMSMPLIST *self);

/*** Debug ***/

#if DEBUG == 1

void __far DEBUG_dump_sample_info (PCMSMP *smp, uint8_t index);

#else

#define DEBUG_dump_sample_info(smp, index)

#endif  // DEBUG

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux __pcmsmp_set_flags "*";

#pragma aux pcmsmp_init "*";
#pragma aux pcmsmp_map_EM_data "*";
#pragma aux pcmsmp_set_title "*";
#pragma aux pcmsmp_get_title "*";
#pragma aux pcmsmp_free "*";

#pragma aux __pcmsmpl_set_flags "*";

#pragma aux pcmsmpl_init "*";
#pragma aux pcmsmpl_free "*";

#pragma aux DEBUG_dump_sample_info "*";

#endif  /* __WATCOMC__ */

#endif  /* PCMSMP_H */
