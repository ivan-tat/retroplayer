/* muspat.h -- declarations for muspat.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MUSPAT_H
#define MUSPAT_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "s3mtypes.h"
#include "pascal.h"
#include "cc/i86.h"
#include "dos/ems.h"
#include "dynarray.h"

/*** Music pattern channel's event ***/

#pragma pack(push, 1);
typedef struct music_pattern_channel_event_data_t
{
    unsigned char instrument;
    unsigned char note;
    unsigned char note_volume;
    unsigned char command;
    unsigned char parameter;
};
#pragma pack(pop);
typedef struct music_pattern_channel_event_data_t MUSPATCHNEVDATA;

typedef uint8_t music_pattern_channel_event_flags_t;
typedef music_pattern_channel_event_flags_t MUSCHNEVFLAGS;

#define MUSPATCHNEVFL_INS  (1 << 0)
#define MUSPATCHNEVFL_NOTE (1 << 1)
#define MUSPATCHNEVFL_VOL  (1 << 2)
#define MUSPATCHNEVFL_CMD  (1 << 3)
#define MUSPATCHNEVFL_ALL  (MUSPATCHNEVFL_INS | MUSPATCHNEVFL_NOTE | MUSPATCHNEVFL_VOL | MUSPATCHNEVFL_CMD)

#pragma pack(push, 1);
typedef struct music_pattern_channel_event_t
{
    MUSCHNEVFLAGS flags;
    MUSPATCHNEVDATA data;
};
#pragma pack(pop);
typedef struct music_pattern_channel_event_t MUSPATCHNEVENT;

void __far muspatchnevent_clear (MUSPATCHNEVENT *self);

/*** Music pattern row's event ***/

typedef struct music_pattern_row_event_t
{
    unsigned char channel;
    MUSPATCHNEVENT event;
};
typedef struct music_pattern_row_event_t MUSPATROWEVENT;

void __far muspatrowevent_clear (MUSPATROWEVENT *self);

/*** Music pattern ***/

typedef uint8_t music_pattern_flags_t;
typedef music_pattern_flags_t MUSPATFLAGS;

#define MUSPATFL_EM     (1 << 0)    /* data is in EM */
#define MUSPATFL_OWNHDL (1 << 1)    /* has own EM handle, needs to be freed when done */
#define MUSPATFL_PACKED (1 << 2)    /* data is packed */

#pragma pack(push, 1);
typedef struct music_pattern_t
{
    MUSPATFLAGS flags;
    uint8_t channels;
    uint16_t rows;
    uint16_t size;
    union
    {
        struct
        {
            void *ptr;
        } dos;
        struct
        {
            uint16_t offset;
            uint16_t page;
            EMSHDL   handle;
        } em;
    } data;
};
#pragma pack(pop);
typedef struct music_pattern_t MUSPAT;

MUSPATFLAGS __far __muspat_set_flags (MUSPATFLAGS _flags, MUSPATFLAGS _mask, MUSPATFLAGS _set, bool raise);

#define _muspat_get_flags(o)                (o)->flags
#define _muspat_set_flags(o, v)             _muspat_get_flags (o) = (v)
#define _muspat_set_EM_data(o, v)           _muspat_set_flags (o, __muspat_set_flags (_muspat_get_flags (o), ~MUSPATFL_EM, MUSPATFL_EM, v))
#define _muspat_is_EM_data(o)               ((_muspat_get_flags (o) & MUSPATFL_EM) != 0)
#define _muspat_set_own_EM_handle(o, v)     _muspat_set_flags (o, __muspat_set_flags (_muspat_get_flags (o), ~MUSPATFL_OWNHDL, MUSPATFL_OWNHDL, v))
#define _muspat_is_own_EM_handle(o)         ((_muspat_get_flags (o) & MUSPATFL_OWNHDL) != 0)
#define _muspat_set_data_packed(o, v)       _muspat_set_flags (o, __muspat_set_flags (_muspat_get_flags (o), ~MUSPATFL_PACKED, MUSPATFL_PACKED, v))
#define _muspat_is_data_packed(o)           ((_muspat_get_flags (o) & MUSPATFL_PACKED) != 0)
#define _muspat_get_channels(o)             (o)->channels
#define _muspat_set_channels(o, v)          _muspat_get_channels (o) = (v)
#define _muspat_get_rows(o)                 (o)->rows
#define _muspat_set_rows(o, v)              _muspat_get_rows (o) = (v)
#define _muspat_get_size(o)                 (o)->size
#define _muspat_set_size(o, v)              _muspat_get_size (o) = (v)
#define _muspat_get_data(o)                 (o)->data.dos.ptr
#define _muspat_set_data(o, v)              _muspat_get_data (o) = (v)
#define _muspat_get_EM_data_handle(o)       (o)->data.em.handle
#define _muspat_set_EM_data_handle(o, v)    _muspat_get_EM_data_handle (o) = (v)
#define _muspat_get_EM_data_page(o)         (o)->data.em.page
#define _muspat_set_EM_data_page(o, v)      _muspat_get_EM_data_page (o) = (v)
#define _muspat_get_EM_data_offset(o)       (o)->data.em.offset
#define _muspat_set_EM_data_offset(o, v)    _muspat_get_EM_data_offset (o) = (v)
#define _muspat_get_EM_data(o)              MK_FP (emsFrameSeg, _muspat_get_EM_data_offset (o))
#define _muspat_get_row_start(o, r, c)      (_muspat_get_channels (o) * (r) + (c)) * sizeof (MUSPATCHNEVDATA)
#define _muspat_get_packed_data_start(o)    (_muspat_get_rows (o) * sizeof (uint16_t))

void     __far muspat_init (MUSPAT *self);
#define        muspat_set_EM_data(o, v)         _muspat_set_EM_data (o, v)
#define        muspat_is_EM_data(o)             _muspat_is_EM_data (o)
#define        muspat_set_own_EM_handle(o, v)   _muspat_set_own_EM_handle (o, v)
#define        muspat_is_own_EM_handle          _muspat_is_own_EM_handle (o)
#define        muspat_set_data_packed(o, v)     _muspat_set_data_packed (o, v)
#define        muspat_is_data_packed(o)         _muspat_is_data_packed (o)
#define        muspat_set_channels(o, v)        _muspat_set_channels (o, v)
#define        muspat_get_channels(o)           _muspat_get_channels (o)
#define        muspat_set_rows(o, v)            _muspat_set_rows (o, v)
#define        muspat_get_rows(o)               _muspat_get_rows (o)
#define        muspat_set_size(o, v)            _muspat_set_size (o, v)
#define        muspat_get_size(o)               _muspat_get_size (o)
#define        muspat_set_data(o, v)            _muspat_set_data (o, v)
#define        muspat_set_EM_data_handle(o, v)  _muspat_set_EM_data_handle (o, v)
#define        muspat_get_EM_data_handle(o)     _muspat_get_EM_data_handle (o)
#define        muspat_set_EM_data_page(o, v)    _muspat_set_EM_data_page (o, v)
#define        muspat_get_EM_data_page(o)       _muspat_get_EM_data_page (o)
#define        muspat_set_EM_data_offset(o, v)  _muspat_set_EM_data_offset (o, v)
#define        muspat_get_EM_data_offset(o)     _muspat_get_EM_data_offset (o)
void    *__far muspat_get_data (MUSPAT *self);
void    *__far muspat_map_EM_data (MUSPAT *self);
#define        muspat_get_row_start(o, r, c)    _muspat_get_row_start (o, r, c)
#define        muspat_get_packed_data_start(o)  _muspat_get_packed_data_start (o)
void     __far muspat_set_packed_row_start (MUSPAT *self, uint16_t row, uint16_t offset);  /* assumes EM data is mapped before call */
uint16_t __far muspat_get_packed_row_start (MUSPAT *self, uint16_t row);                   /* assumes EM data is mapped before call */
uint16_t __far muspat_get_packed_size (MUSPAT *self);                                      /* assumes EM data is mapped before call */
void     __far muspat_free (MUSPAT *self);

/*** Music pattern IO ***/

typedef unsigned char music_pattern_io_mode_t;
typedef music_pattern_io_mode_t MUSPATIOMODE;

#define MUSPATIOMD_READ     0
#define MUSPATIOMD_WRITE    1

typedef struct music_pattern_io_t;
typedef struct music_pattern_io_t MUSPATIO;

typedef void __far muspatio_seek_t (MUSPATIO *self, unsigned int row, unsigned char channel);
typedef void __far muspatio_read_write_t (MUSPATIO *self, MUSPATROWEVENT *event);
typedef bool __far muspatio_is_end_of_row_t (MUSPATIO *self);
typedef void __far muspatio_end_row_t (MUSPATIO *self);
typedef void __far muspatio_close_t (MUSPATIO *self);

typedef struct music_pattern_io_t
{
    muspatio_seek_t *m_seek;
    muspatio_read_write_t *m_read_write;
    muspatio_is_end_of_row_t *m_is_end_of_row;
    muspatio_end_row_t *m_end_row;
    muspatio_close_t *m_close;
    char *error;
    MUSPAT *pattern;
    MUSPATIOMODE mode;
    unsigned char *data;
    unsigned int row;
    unsigned int row_start;
    unsigned int row_end;
    unsigned int offset;
    unsigned char channel;
};
typedef struct music_pattern_io_t MUSPATIO;

bool __far muspatio_open (MUSPATIO *self, MUSPAT *pattern, MUSPATIOMODE mode);  /* maps EM data */
#define    muspatio_seek(self, row, channel)    (self)->m_seek ((self), (row), (channel))
#define    muspatio_read(self, event)           (self)->m_read_write ((self), (event))
#define    muspatio_write(self, event)          (self)->m_read_write ((self), (event))
#define    muspatio_is_end_of_row(self)         (self)->m_is_end_of_row (self)
#define    muspatio_end_row(self)               (self)->m_end_row (self)
#define    muspatio_close(self)                 (self)->m_close (self)

/*** Patterns list ***/

typedef uint16_t music_patterns_list_flags_t;
typedef music_patterns_list_flags_t MUSPATLFLAGS;

#define MUSPATLFL_EM     (1 << 0)   /* data is in EM */
#define MUSPATLFL_OWNHDL (1 << 1)   /* has own EM handle, needs to be freed when done */

typedef struct music_patterns_list_t
{
    MUSPATLFLAGS flags;
    DYNARR list;
    EMSHDL handle;
};
typedef struct music_patterns_list_t MUSPATLIST;

/* Methods */

MUSPATLFLAGS __far __muspatl_set_flags (MUSPATLFLAGS _flags, MUSPATLFLAGS _mask, MUSPATLFLAGS _set, bool raise);

#define _muspatl_get_flags(o)               (o)->flags
#define _muspatl_set_flags(o, v)            _muspatl_get_flags (o) = (v)
#define _muspatl_set_EM_data(o, v)          _muspatl_set_flags (o, __muspatl_set_flags (_muspatl_get_flags (o), ~MUSPATLFL_EM, MUSPATLFL_EM, v))
#define _muspatl_is_EM_data(o)              ((_muspatl_get_flags (o) & MUSPATLFL_EM) != 0)
#define _muspatl_set_own_EM_handle(o, v)    _muspatl_set_flags (o, __muspatl_set_flags (_muspatl_get_flags (o), ~MUSPATLFL_OWNHDL, MUSPATLFL_OWNHDL, v))
#define _muspatl_is_own_EM_handle(o)        ((_muspatl_get_flags (o) & MUSPATLFL_OWNHDL) != 0)
#define _muspatl_get_list(o)                & ((o)->list)
#define _muspatl_get_EM_handle(o)           (o)->handle
#define _muspatl_set_EM_handle(o, v)        _muspatl_get_EM_handle (o) = (v)
#define _muspatl_set_EM_handle_name(o, v)   emsSetHandleName (_muspatl_get_EM_handle (o), v)
#define _muspatl_set_count(o, v)            dynarr_set_size (_muspatl_get_list (o), v)
#define _muspatl_get_count(o)               dynarr_get_size (_muspatl_get_list (o))
#define _muspatl_set(o, i, v)               dynarr_set_item (_muspatl_get_list (o), i, v)
#define _muspatl_get(o, i)                  dynarr_get_item (_muspatl_get_list (o), i)
#define _muspatl_get_used_EM(o)             ((uint32_t) emsGetHandleSize (_muspatl_get_EM_handle (o)) << 4)

void __far muspatl_init (MUSPATLIST *self);
#define    muspatl_set_EM_data(o, v)        _muspatl_set_EM_data (o, v)
#define    muspatl_is_EM_data(o)            _muspatl_is_EM_data (o)
#define    muspatl_set_own_EM_handle(o, v)  _muspatl_set_own_EM_handle (o, v)
#define    muspatl_is_own_EM_handle(o)      _muspatl_is_own_EM_handle (o)
#define    muspatl_set_EM_handle(o, v)      _muspatl_set_EM_handle (o, v)
#define    muspatl_get_EM_handle(o)         _muspatl_get_EM_handle (o)
#define    muspatl_set_EM_handle_name(o, v) _muspatl_set_EM_handle_name (o, v)
#define    muspatl_set_count(o, v)          _muspatl_set_count (o, v)
#define    muspatl_get_count(o)             _muspatl_get_count (o)
#define    muspatl_set(o, i, v)             _muspatl_set (o, i, v)
#define    muspatl_get(o, i)                _muspatl_get (o, i)
#define    muspatl_get_used_EM(o)           _muspatl_get_used_EM (o)
void __far muspatl_free (MUSPATLIST *self);

/*** Patterns order ***/

typedef uint8_t music_patterns_order_entry_t;
typedef music_patterns_order_entry_t MUSPATORDENT;

#define MUSPATORDENT_SKIP   0xfe
#define MUSPATORDENT_END    0xff

typedef struct music_patterns_order_t
{
    DYNARR list;
};
typedef struct music_patterns_order_t MUSPATORDER;

#define _muspatorder_get_list(o)        & ((o)->list)
#define _muspatorder_set_count(o, v)    dynarr_set_size (_muspatorder_get_list (o), v)
#define _muspatorder_get_count(o)       dynarr_get_size (_muspatorder_get_list (o))
#define _muspatorder_set(o, i, v)       dynarr_set_item (_muspatorder_get_list (o), i, v)
#define _muspatorder_get(o, i)          dynarr_get_item (_muspatorder_get_list (o), i)

void __far muspatorder_init (MUSPATORDER *self);
#define    muspatorder_set_count(o, v)  _muspatorder_set_count (o, v)
#define    muspatorder_get_count(o)     _muspatorder_get_count (o)
#define    muspatorder_set(o, i, v)     _muspatorder_set (o, i, v)
#define    muspatorder_get(o, i)        _muspatorder_get (o, i)
void __far muspatorder_free (MUSPATORDER *self);

/*** Debug ***/

#if DEBUG == 1

void __far DEBUG_get_pattern_channel_event_str (char *s, MUSPATCHNEVENT *event);    // Format is "no in vl eff" (12 characters + zero)
void __far DEBUG_dump_pattern_info (MUSPAT *pattern, uint8_t index);
bool __far DEBUG_dump_pattern (MUSPAT *self, char *s, uint8_t num_channels);    // "s" must hold atleast 64 bytes or (num_channels * 13) bytes

#endif  /* DEBUG */

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux muspatchnevent_clear "*";

#pragma aux muspatrowevent_clear "*";

#pragma aux __muspat_set_flags "*";

#pragma aux muspat_init "*";
#pragma aux muspat_get_data "*";
#pragma aux muspat_map_EM_data "*";
#pragma aux muspat_set_packed_row_start "*";
#pragma aux muspat_get_packed_row_start "*";
#pragma aux muspat_get_packed_size "*";
#pragma aux muspat_free "*";

#pragma aux muspatio_open "*";

#pragma aux __muspatl_set_flags "*";

#pragma aux muspatl_init "*";
#pragma aux muspatl_free "*";

#pragma aux muspatorder_init "*";
#pragma aux muspatorder_free "*";

#if DEBUG == 1

#pragma aux DEBUG_get_pattern_channel_event_str "*";
#pragma aux DEBUG_dump_pattern_info "*";
#pragma aux DEBUG_dump_pattern "*";

#endif  /* DEBUG */

#endif  /* __WATCOMC__ */

#endif  /* MUSPAT_H */
