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
    uint16_t data_off;
    uint16_t data_seg;  /* DOS segment or EM page */
    EMSHDL handle;
};
#pragma pack(pop);
typedef struct music_pattern_t MUSPAT;

void     __far muspat_init (MUSPAT *self);
void     __far muspat_set_EM_data (MUSPAT *self, bool value);
bool     __far muspat_is_EM_data (MUSPAT *self);
void     __far muspat_set_own_EM_handle (MUSPAT *self, bool value);
bool     __far muspat_is_own_EM_handle (MUSPAT *self);
void     __far muspat_set_data_packed (MUSPAT *self, bool value);
bool     __far muspat_is_data_packed (MUSPAT *self);
void     __far muspat_set_channels (MUSPAT *self, uint8_t value);
uint8_t  __far muspat_get_channels (MUSPAT *self);
void     __far muspat_set_rows (MUSPAT *self, uint16_t value);
uint16_t __far muspat_get_rows (MUSPAT *self);
void     __far muspat_set_size (MUSPAT *self, uint16_t value);
uint16_t __far muspat_get_size (MUSPAT *self);
void     __far muspat_set_data (MUSPAT *self, void *value);
void     __far muspat_set_EM_data_handle (MUSPAT *self, EMSHDL value);
EMSHDL   __far muspat_get_EM_data_handle (MUSPAT *self);
void     __far muspat_set_EM_data_page (MUSPAT *self, uint16_t value);
uint16_t __far muspat_get_EM_data_page (MUSPAT *self);
void     __far muspat_set_EM_data_offset (MUSPAT *self, uint16_t value);
uint16_t __far muspat_get_EM_data_offset (MUSPAT *self);
void    *__far muspat_get_data (MUSPAT *self);
void    *__far muspat_map_EM_data (MUSPAT *self);
uint16_t __far muspat_get_row_start (MUSPAT *self, uint16_t row, uint8_t channel);
uint16_t __far muspat_get_packed_data_start (MUSPAT *self);
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

void        __far muspatl_init (MUSPATLIST *self);
void        __far muspatl_set (MUSPATLIST *self, uint16_t index, MUSPAT *item);
MUSPAT     *__far muspatl_get (MUSPATLIST *self, uint16_t index);
bool        __far muspatl_set_count (MUSPATLIST *self, uint16_t value);
uint16_t    __far muspatl_get_count (MUSPATLIST *self);
void        __far muspatl_set_EM_data (MUSPATLIST *self, bool value);
bool        __far muspatl_is_EM_data (MUSPATLIST *self);
void        __far muspatl_set_own_EM_handle (MUSPATLIST *self, bool value);
bool        __far muspatl_is_own_EM_handle (MUSPATLIST *self);
void        __far muspatl_set_EM_handle (MUSPATLIST *self, EMSHDL value);
EMSHDL      __far muspatl_get_EM_handle (MUSPATLIST *self);
void        __far muspatl_set_EM_handle_name (MUSPATLIST *self);
uint32_t    __far muspatl_get_used_EM (MUSPATLIST *self);
void        __far muspatl_free (MUSPATLIST *self);

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

#pragma aux muspat_init "*";
#pragma aux muspat_set_EM_data "*";
#pragma aux muspat_is_EM_data "*";
#pragma aux muspat_set_own_EM_handle "*";
#pragma aux muspat_is_own_EM_handle "*";
#pragma aux muspat_set_data_packed "*";
#pragma aux muspat_is_data_packed "*";
#pragma aux muspat_set_channels "*";
#pragma aux muspat_get_channels "*";
#pragma aux muspat_set_rows "*";
#pragma aux muspat_get_rows "*";
#pragma aux muspat_set_size "*";
#pragma aux muspat_get_size "*";
#pragma aux muspat_set_data "*";
#pragma aux muspat_set_EM_data_handle "*";
#pragma aux muspat_get_EM_data_handle "*";
#pragma aux muspat_set_EM_data_page "*";
#pragma aux muspat_get_EM_data_page "*";
#pragma aux muspat_set_EM_data_offset "*";
#pragma aux muspat_get_EM_data_offset "*";
#pragma aux muspat_get_data "*";
#pragma aux muspat_map_EM_data "*";
#pragma aux muspat_get_row_start "*";
#pragma aux muspat_get_packed_data_start "*";
#pragma aux muspat_set_packed_row_start "*";
#pragma aux muspat_get_packed_row_start "*";
#pragma aux muspat_get_packed_size "*";
#pragma aux muspat_free "*";

#pragma aux muspatio_open "*";

#pragma aux muspatl_init "*";
#pragma aux muspatl_set "*";
#pragma aux muspatl_get "*";
#pragma aux muspatl_set_count "*";
#pragma aux muspatl_get_count "*";
#pragma aux muspatl_set_EM_data "*";
#pragma aux muspatl_is_EM_data "*";
#pragma aux muspatl_set_own_EM_handle "*";
#pragma aux muspatl_is_own_EM_handle "*";
#pragma aux muspatl_set_EM_handle "*";
#pragma aux muspatl_get_EM_handle "*";
#pragma aux muspatl_set_EM_handle_name "*";
#pragma aux muspatl_get_used_EM "*";
#pragma aux muspatl_free "*";

#if DEBUG == 1

#pragma aux DEBUG_get_pattern_channel_event_str "*";
#pragma aux DEBUG_dump_pattern_info "*";
#pragma aux DEBUG_dump_pattern "*";

#endif  /* DEBUG */

#endif  /* __WATCOMC__ */

#endif  /* MUSPAT_H */
