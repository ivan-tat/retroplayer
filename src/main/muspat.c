/* muspat.c -- musical pattern handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$muspat$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/stdio.h"
#include "cc/dos.h"
#include "debug.h"
#include "dynarray.h"
#include "dos/ems.h"
#include "main/muspat.h"

/*** Music pattern channel's event ***/

void __far muspatchnevent_clear (MUSPATCHNEVENT *self)
{
    self->flags = 0;
    self->data.instrument  = CHN_INS_NONE;
    self->data.note        = CHN_NOTE_NONE;
    self->data.note_volume = CHN_NOTEVOL_NONE;
    self->data.command     = CHN_CMD_NONE;
    self->data.parameter   = 0;
}

/*** Music pattern row's event ***/

void __far muspatrowevent_clear (MUSPATROWEVENT *self)
{
    self->channel = 0;
    self->event.flags = 0;
    self->event.data.instrument  = CHN_INS_NONE;
    self->event.data.note        = CHN_NOTE_NONE;
    self->event.data.note_volume = CHN_NOTEVOL_NONE;
    self->event.data.command     = CHN_CMD_NONE;
    self->event.data.parameter   = 0;
}

/*** Music pattern ***/

MUSPATFLAGS __far __muspat_set_flags (MUSPATFLAGS _flags, MUSPATFLAGS _mask, MUSPATFLAGS _set, bool raise)
{
    MUSPATFLAGS result;

    result = _flags & _mask;
    if (raise)
        result |= _set;

    return result;
}

void __far muspat_init (MUSPAT *self)
{
    if (self)
    {
        memset(self, 0, sizeof(MUSPAT));
        _muspat_set_EM_data_handle(self, EMSBADHDL);
    }
}

void *__far muspat_get_data (MUSPAT *self)
{
    if (self)
    {
        if (_muspat_is_EM_data(self))
            return _muspat_get_EM_data(self);
        else
            return _muspat_get_data(self);
    }
    else
        return NULL;
}

void *__far muspat_map_EM_data (MUSPAT *self)
{
    if (self)
    {
        if (_muspat_is_EM_data(self))
            if (emsMap(_muspat_get_EM_data_handle(self), _muspat_get_EM_data_page(self), 0))
                return _muspat_get_EM_data(self);
    }

    return NULL;
}

/* assumes EM data is mapped before call */
void __far muspat_set_packed_row_start (MUSPAT *self, uint16_t row, uint16_t offset)
{
    uint16_t *offsets;

    if (row)
    {
        offsets = muspat_get_data (self);
        offsets [row - 1] = offset - _muspat_get_packed_data_start (self);
    }
}

/* assumes EM data is mapped before call */
uint16_t __far muspat_get_packed_row_start (MUSPAT *self, uint16_t row)
{
    uint16_t start;
    uint16_t *offsets;

    start = _muspat_get_packed_data_start (self);
    if (row)
    {
        offsets = muspat_get_data (self);
        start += offsets [row - 1];
    }

    return start;
}

/* assumes EM data is mapped before call */
uint16_t __far muspat_get_packed_size (MUSPAT *self)
{
    uint16_t rows;
    uint16_t start;
    uint16_t *offsets;

    rows = _muspat_get_rows (self);
    start = _muspat_get_packed_data_start (self);
    if (rows)
    {
        offsets = muspat_get_data (self);
        start += offsets [rows - 1];
    }

    return start;
}

void __far muspat_free (MUSPAT *self)
{
    void *p;

    if (self)
    {
        if (_muspat_is_EM_data(self))
        {
            if (_muspat_is_own_EM_handle(self))
                emsFree(_muspat_get_EM_data_handle(self));
        }
        else
        {
            p = _muspat_get_data(self);
            if (p)
                _dos_freemem(FP_SEG(p));
        }
    }
}

/*** Music pattern reader ***/

typedef unsigned char music_pattern_row_event_flags_t;
typedef music_pattern_row_event_flags_t MUSPATROWEVFLAGS;

#define MUSPATROWEVFL_CHN               (1 << 0)
#define MUSPATROWEVFL_CHNEVENT_SHIFT    1

void __far _muspatio_seek (MUSPATIO *self, unsigned int row, unsigned char channel);
void __far _muspatio_seek_packed (MUSPATIO *self, unsigned int row, unsigned char channel);
void __far _muspatio_read (MUSPATIO *self, MUSPATROWEVENT *event);
void __far _muspatio_read_packed (MUSPATIO *self, MUSPATROWEVENT *event);
bool __far _muspatio_is_end_of_row (MUSPATIO *self);
bool __far _muspatio_is_end_of_row_packed (MUSPATIO *self);
void __far _muspatio_end_row (MUSPATIO *self);
void __far _muspatio_end_row_packed_write (MUSPATIO *self);
void __far _muspatio_write (MUSPATIO *self, MUSPATROWEVENT *event);
void __far _muspatio_write_packed (MUSPATIO *self, MUSPATROWEVENT *event);
void __far _muspatio_close (MUSPATIO *self);

void __near muspatio_open_read (MUSPATIO *self)
{
    self->m_seek            = & _muspatio_seek;
    self->m_read_write      = & _muspatio_read;
    self->m_is_end_of_row   = & _muspatio_is_end_of_row;
    self->m_end_row         = & _muspatio_end_row;
    self->m_close           = & _muspatio_close;
    //~ self->row_start         = 0;
    //~ self->row_end           = 0;
    //~ self->offset            = 0;
    //~ self->channel           = 0;
}

void __near muspatio_open_write (MUSPATIO *self)
{
    self->m_seek            = & _muspatio_seek;
    self->m_read_write      = & _muspatio_write;
    self->m_is_end_of_row   = & _muspatio_is_end_of_row;
    self->m_end_row         = & _muspatio_end_row;
    self->m_close           = & _muspatio_close;
    //~ self->row_start         = 0;
    //~ self->row_end           = 0;
    //~ self->offset            = 0;
    //~ self->channel           = 0;
}

void __near muspatio_open_packed_read (MUSPATIO *self)
{
    self->m_seek            = & _muspatio_seek_packed;
    self->m_read_write      = & _muspatio_read_packed;
    self->m_is_end_of_row   = & _muspatio_is_end_of_row_packed;
    self->m_end_row         = & _muspatio_end_row;
    self->m_close           = & _muspatio_close;
    self->row_start         = _muspat_get_packed_data_start (self->pattern);
    self->row_end           = muspat_get_packed_row_start (self->pattern, 1);
    self->offset            = self->row_start;
    //~ self->channel           = 0;
}

void __near muspatio_open_packed_write (MUSPATIO *self)
{
    self->m_seek            = & _muspatio_seek_packed;
    self->m_read_write      = & _muspatio_write_packed;
    self->m_is_end_of_row   = & _muspatio_is_end_of_row_packed;
    self->m_end_row         = & _muspatio_end_row_packed_write;
    self->m_close           = & _muspatio_close;
    self->row_start         = _muspat_get_packed_data_start (self->pattern);
    //~ self->row_end           = 0;    // undefined
    self->offset            = self->row_start;
    //~ self->channel           = 0;
}

void __near muspatio_clear (MUSPATIO *self)
{
    memset (self, 0, sizeof (MUSPATIO));
}

bool __far muspatio_open (MUSPATIO *self, MUSPAT *pattern, MUSPATIOMODE mode)
{
    if (self && pattern)
    {
        muspatio_clear (self);

        self->pattern = pattern;

        if (muspat_is_EM_data (pattern))
        {
            self->data = muspat_map_EM_data (pattern);
            if (!self->data)
            {
                self->error = "EM map failed";
                return false;
            }
        }
        else
        {
            self->data = muspat_get_data (pattern);
            if (!self->data)
            {
                self->error = "No data";
                return false;
            }
        }

        self->mode = mode;

        if (muspat_is_data_packed (pattern))
        {
            switch (mode)
            {
            case MUSPATIOMD_READ:
                muspatio_open_packed_read (self);
                return true;
            case MUSPATIOMD_WRITE:
                muspatio_open_packed_write (self);
                return true;
            default:
                self->error = "Bad mode";
                return false;
            }
        }
        else
        {
            switch (mode)
            {
            case MUSPATIOMD_READ:
                muspatio_open_read (self);
                return true;
            case MUSPATIOMD_WRITE:
                muspatio_open_write (self);
                return true;
            default:
                self->error = "Bad mode";
                return false;
            }
        }
    }

    self->error = "Bad arguments";
    return false;
}

void __far _muspatio_seek (MUSPATIO *self, unsigned int row, unsigned char channel)
{
    MUSPAT *pattern;

    self->row = row;
    pattern = self->pattern;

    self->row_start = _muspat_get_row_start (pattern, row, 0);
    self->row_end = _muspat_get_row_start (pattern, row + 1, 0);
    self->offset = _muspat_get_row_start (pattern, row, channel);
    self->channel = channel;
}

void __far _muspatio_seek_packed (MUSPATIO *self, unsigned int row, unsigned char channel)
{
    MUSPAT *pattern;

    self->row = row;
    pattern = self->pattern;

    self->row_start = muspat_get_packed_row_start (pattern, row);
    self->row_end = muspat_get_packed_row_start (pattern, row + 1);
    self->offset = self->row_start;
    self->channel = 0;
}

void __far _muspatio_read (MUSPATIO *self, MUSPATROWEVENT *event)
{
    unsigned char *data;
    MUSCHNEVFLAGS flags;
    unsigned char channel;

    data = self->data + self->offset;

    event->channel = self->channel;
    flags = 0;

    event->event.data.instrument = data [0];
    if (event->event.data.instrument != CHN_INS_NONE)
        flags |= MUSPATCHNEVFL_INS;

    event->event.data.note = data [1];
    if (event->event.data.note != CHN_NOTE_NONE)
        flags |= MUSPATCHNEVFL_NOTE;

    event->event.data.note_volume = data [2];
    if (event->event.data.note_volume != CHN_NOTEVOL_NONE)
        flags |= MUSPATCHNEVFL_VOL;

    event->event.data.command = data [3];
    event->event.data.parameter = data [4];
    if (event->event.data.command != CHN_CMD_NONE)
        flags |= MUSPATCHNEVFL_CMD;

    event->event.flags = flags;
    self->offset += 5;
    self->channel++;
}

void __far _muspatio_read_packed (MUSPATIO *self, MUSPATROWEVENT *event)
{
    unsigned char *data;
    MUSPATROWEVFLAGS row_flags;
    unsigned char channel;

    data = self->data + self->offset;

    row_flags = data [0];
    data++;

    if (row_flags & MUSPATROWEVFL_CHN)
    {
        channel = data [0];
        data++;
    }
    else
        channel = self->channel;

    event->channel = channel;
    event->event.flags = (row_flags >> MUSPATROWEVFL_CHNEVENT_SHIFT) & MUSPATCHNEVFL_ALL;

    if (row_flags & (MUSPATCHNEVFL_INS << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        event->event.data.instrument = data [0];
        data++;
    }
    else
        event->event.data.instrument = CHN_INS_NONE;

    if (row_flags & (MUSPATCHNEVFL_NOTE << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        event->event.data.note = data [0];
        data++;
    }
    else
        event->event.data.note = CHN_NOTE_NONE;

    if (row_flags & (MUSPATCHNEVFL_VOL << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        event->event.data.note_volume = data [0];
        data++;
    }
    else
        event->event.data.note_volume = CHN_NOTEVOL_NONE;

    if (row_flags & (MUSPATCHNEVFL_CMD << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        event->event.data.command = data [0];
        event->event.data.parameter = data [1];
        data += 2;
    }
    else
    {
        event->event.data.command = CHN_CMD_NONE;
        event->event.data.parameter = 0;
    }

    self->offset = FP_OFF (data) - FP_OFF (self->data);
    self->channel = channel + 1;
}

void __far _muspatio_write (MUSPATIO *self, MUSPATROWEVENT *event)
{
    unsigned char *data;

    if (event->channel < muspat_get_channels (self->pattern))
    {
        if (!(event->event.flags & MUSPATCHNEVFL_INS))
            event->event.data.instrument = CHN_INS_NONE;

        if (!(event->event.flags & MUSPATCHNEVFL_NOTE))
            event->event.data.note = CHN_NOTE_NONE;

        if (!(event->event.flags & MUSPATCHNEVFL_VOL))
            event->event.data.note_volume = CHN_NOTEVOL_NONE;

        if (!(event->event.flags & MUSPATCHNEVFL_CMD))
        {
            event->event.data.command = CHN_CMD_NONE;
            event->event.data.parameter = 0;
        }

        muspatio_seek (self, self->row, event->channel);

        data = self->data + self->offset;
        data [0] = event->event.data.instrument;
        data [1] = event->event.data.note;
        data [2] = event->event.data.note_volume;
        data [3] = event->event.data.command;
        data [4] = event->event.data.parameter;
        self->offset += 5;
        self->channel++;
    }
}

void __far _muspatio_write_packed (MUSPATIO *self, MUSPATROWEVENT *event)
{
    unsigned char *data;
    unsigned int offset;
    MUSPATROWEVFLAGS row_flags;
    unsigned char channel;

    data = self->data + self->offset;

    row_flags = (event->event.flags & MUSPATCHNEVFL_ALL) << MUSPATROWEVFL_CHNEVENT_SHIFT;

    channel = event->channel;
    if (channel != self->channel)
        row_flags |= MUSPATROWEVFL_CHN;

    data [0] = row_flags;
    data++;

    if (row_flags & MUSPATROWEVFL_CHN)
    {
        data [0] = channel;
        data++;
    }

    if (row_flags & (MUSPATCHNEVFL_INS << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        data [0] = event->event.data.instrument;
        data++;
    }

    if (row_flags & (MUSPATCHNEVFL_NOTE << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        data [0] = event->event.data.note;
        data++;
    }

    if (row_flags & (MUSPATCHNEVFL_VOL << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        data [0] = event->event.data.note_volume;
        data++;
    }

    if (row_flags & (MUSPATCHNEVFL_CMD << MUSPATROWEVFL_CHNEVENT_SHIFT))
    {
        data [0] = event->event.data.command;
        data [1] = event->event.data.parameter;
        data += 2;
    }

    self->offset = FP_OFF (data) - FP_OFF (self->data);
    self->channel = channel + 1;
}

bool __far _muspatio_is_end_of_row (MUSPATIO *self)
{
    return (self->channel >= muspat_get_channels (self->pattern)) || (self->offset >= self->row_end);
}

bool __far _muspatio_is_end_of_row_packed (MUSPATIO *self)
{
    return self->offset >= self->row_end;
}

void __far _muspatio_end_row (MUSPATIO *self)
{
    muspatio_seek (self, self->row + 1, 0);
}

void __far _muspatio_end_row_packed_write (MUSPATIO *self)
{
    if (self->row < _muspat_get_rows (self->pattern))
    {
        self->row++;

        muspat_set_packed_row_start (self->pattern, self->row, self->offset);

        self->row_start = self->offset;
        self->row_end = 0;
        self->channel = 0;
    }
}

void __far _muspatio_close (MUSPATIO *self)
{
    muspatio_clear (self);
}

/*** Music patterns list ***/

MUSPATLFLAGS __far __muspatl_set_flags (MUSPATLFLAGS _flags, MUSPATLFLAGS _mask, MUSPATLFLAGS _set, bool raise)
{
    MUSPATLFLAGS result;

    result = _flags & _mask;
    if (raise)
        result |= _set;

    return result;
}

void __far _muspatl_init_item (void *self, void *item)
{
    muspat_init((MUSPAT *)item);
}

void __far _muspatl_free_item (void *self, void *item)
{
    muspat_free((MUSPAT *)item);
}

void __far muspatl_init (MUSPATLIST *self)
{
    if (self)
    {
        dynarr_init (_muspatl_get_list (self), self, sizeof(MUSPAT), _muspatl_init_item, _muspatl_free_item);
        _muspatl_set_EM_handle(self, EMSBADHDL);
    }
}

void __far muspatl_free (MUSPATLIST *self)
{
    if (self)
    {
        dynarr_free (_muspatl_get_list (self));

        if (_muspatl_is_own_EM_handle(self))
            emsFree (_muspatl_get_EM_handle (self));
    }
}

/*** Music patterns order ***/

void __far _muspatorder_init_item (void *self, void *item)
{
    *(MUSPATORDENT *) item = MUSPATORDENT_END;
}

void __far _muspatorder_free_item (void *self, void *item)
{
}

void __far muspatorder_init (MUSPATORDER *self)
{
    if (self)
        dynarr_init (_muspatorder_get_list (self), self, sizeof (MUSPATORDENT), _muspatorder_init_item, _muspatorder_free_item);
}

int __far muspatorder_find_next_pattern (MUSPATORDER *self, int first, int last, int pos, int step, bool skipend)
{
    MUSPATORDENT *entry;
    bool found;

    found = false;
    while ((!found) && (first <= pos) && (pos <= last))
    {
        entry = muspatorder_get (self, pos);
        switch (*entry)
        {
        case MUSPATORDENT_SKIP:
            pos += step;
            break;
        case MUSPATORDENT_END:
            if (skipend)
                pos += step;
            else
                pos = -1;
            break;
        default:
            found = true;
            break;
        }
    }

    return found ? pos : -1;
}

int __far muspatorder_find_last (MUSPATORDER *self, bool skipend)
{
    MUSPATORDENT *entry;
    int i, last;
    bool found;

    if (self)
    {
        last = muspatorder_get_count (self) - 1;
        found = false;

        if (! skipend)
        {
            /* Search for first MUSPATORDENT_END mark (as usual) */
            i = 0;
            while ((!found) && (i < last))
            {
                entry = muspatorder_get (self, i);
                if (*entry == MUSPATORDENT_END)
                    found = true;
                else
                    i++;
            }
            i--;
        }
        else
        {
            /* It is not important, we can also do simply order_last = order_length - 1 */
            i = last;
            while ((!found) && (i > 0))
            {
                entry = muspatorder_get (self, i);
                if ((*entry != MUSPATORDENT_SKIP)
                &&  (*entry != MUSPATORDENT_END))
                    found = true;
                else
                    i--;
            }
        }
    }
    else
        i = 0;

    return i;
}

void __far muspatorder_free (MUSPATORDER *self)
{
    if (self)
        dynarr_free (_muspatorder_get_list (self));
}

/*** Debug ***/

#if DEBUG == 1

#include "hexdigts.h"

static const char __halftones[16] = "cCdDefFgGaAb????";
static const char __octaves[16] = "0123456789??????";

void __far DEBUG_get_pattern_channel_event_str (char *s, MUSPATCHNEVENT *event)
{
    uint8_t v;

    if (event->flags & MUSPATCHNEVFL_NOTE)
    {
        v = event->data.note;
        switch (v)
        {
        case CHN_NOTE_NONE:
            s[0] = '.';
            s[1] = '.';
            break;
        case CHN_NOTE_OFF:
            s[0] = '=';
            s[1] = '=';
            break;
        default:
            if (v <= CHN_NOTE_MAX)
            {
                s[0] = __halftones[v & 0x0f];
                s[1] = __octaves[v >> 4];
            }
            else
            {
                s[0] = '?';
                s[1] = '?';
            }
            break;
        }
    }
    else
    {
        s[0] = '.';
        s[1] = '.';
    }

    s[2] = ' ';

    if (event->flags & MUSPATCHNEVFL_INS)
    {
        v = event->data.instrument;
        switch (v)
        {
        case CHN_INS_NONE:
            s[3] = '.';
            s[4] = '.';
            break;
        default:
            v = _get_instrument (v);
            v++;
            s[3] = HEXDIGITS[v >> 4];
            s[4] = HEXDIGITS[v & 0x0f];
            break;
        }
    }
    else
    {
        s[3] = '.';
        s[4] = '.';
    }

    s[5] = ' ';

    if (event->flags & MUSPATCHNEVFL_VOL)
    {
        v = event->data.note_volume;
        switch (v)
        {
        case CHN_NOTEVOL_NONE:
            s[6] = '.';
            s[7] = '.';
            break;
        default:
            if (v <= CHN_NOTEVOL_MAX)
            {
                s[6] = '0' + (v / 10);
                s[7] = '0' + (v % 10);
            }
            else
            {
                s[6] = '?';
                s[7] = '?';
            }
            break;
        }
    }
    else
    {
        s[6] = '.';
        s[7] = '.';
    }

    s[8] = ' ';

    if (event->flags & MUSPATCHNEVFL_CMD)
    {
        v = event->data.command;
        if (v == CHN_CMD_NONE)
        {
            s[9] = '.';
            s[10] = '.';
            s[11] = '.';
        }
        else
        {
            if (v <= CHN_CMD_MAX)
                s[9] = 'A' + v - 1;
            else
                s[9] = '?';

            snprintf (s + 10, 2, "%02X", event->data.parameter);
        }
    }
    else
    {
        s[9] = '.';
        s[10] = '.';
        s[11] = '.';
    }

    s[12] = 0;
}

void __far DEBUG_dump_pattern_info (MUSPAT *pattern, uint8_t index)
{
    void *data;
    char s[64], f[64];

    if (muspat_is_EM_data (pattern))
    {
        if (!muspat_map_EM_data (pattern))
        {
            DEBUG_ERR_ ("Failed to map EM for %s.", "pattern");
            return;
        }
        snprintf (s, 64,
            "EM (page=0x%04X, offset=0x%04X)",
            muspat_get_EM_data_page (pattern),
            muspat_get_EM_data_offset (pattern)
        );
    }
    else
    {
        data = muspat_get_data (pattern);
        snprintf (s, 64,
            "DOS (address=0x%04X:0x%04X)",
            FP_SEG (data),
            FP_OFF (data)
        );
    }

    if (muspat_is_data_packed (pattern))
    {
        snprintf (f, 64,
            "packed (size=0x%04X, data_start=0x%04X)",
            muspat_get_packed_size (pattern),
            muspat_get_packed_data_start (pattern)
        );
    }
    else
        snprintf (f, 64,
            "raw (channels=%u, row_size=0x04X)",
            muspat_get_channels (pattern),
            muspat_get_channels (pattern) * sizeof (MUSPATCHNEVDATA)
        );

    DEBUG_MSG_ ("pattern_index=%hu, rows=%hu, mem_size=0x%04X, place=%s, format=%s.",
        index,
        muspat_get_rows (pattern),
        muspat_get_size (pattern),
        s,
        f
    );
}

// "s" must hold atleast 64 bytes or (num_channels * 13) bytes
bool __far DEBUG_dump_pattern (MUSPAT *pattern, char *s, uint8_t num_channels)
{
    MUSPATIO f;
    MUSPATROWEVENT e, empty;
    unsigned int size;
    unsigned int rows, row;
    unsigned char c, next_c;
    bool row_read, row_ev_ok;

    if ((!pattern) || (!muspatio_open (&f, pattern, MUSPATIOMD_READ)))
    {
        DEBUG_ERR_ ("Failed to read pattern (%s)", f.error);
        return false;
    }

    if (muspat_is_data_packed (pattern))
        size = muspat_get_packed_size (pattern);
    else
        size = muspat_get_size (pattern);

    DEBUG_dump_mem (f.data, size, "data: ");

    rows = muspat_get_rows (pattern);
    for (row = 0; row < rows; row++)
    {
        muspatio_seek (&f, row, 0);

        /* Linear reading of pattern's events while increasing channel number */
        muspatrowevent_clear (&empty);
        c = 0;
        row_read = !muspatio_is_end_of_row (&f);
        while (c < num_channels)
        {
            // walk through from current channel (c) to the end
            row_ev_ok = false;
            next_c = num_channels;

            if (row_read)
            {
                muspatio_read (&f, &e);
                if (e.channel < num_channels)
                {
                    // walk through from current channel (c) to current event's channel
                    row_ev_ok = true;
                    next_c = e.channel;
                    row_read = !muspatio_is_end_of_row (&f);
                }
            }

            /* walk through from channel (c) to (next_c)  */
            while (c < next_c)
            {
                empty.channel = c;
                DEBUG_get_pattern_channel_event_str (s + empty.channel * 13, & (empty.event));
                s[empty.channel * 13 + 12] = '|';
                c++;
            }

            if (row_ev_ok)
            {
                DEBUG_get_pattern_channel_event_str (s + e.channel * 13, & (e.event));
                s[e.channel * 13 + 12] = '|';
                c++;
            }
        }

        s[(num_channels - 1) * 13 + 12] = 0;
        _DEBUG_LOG (DBGLOG_MSG, NULL, 0, NULL, "%03hhu: |%s|", row, s);
    }

    muspatio_close (&f);
    return true;
}

#endif  /* DEBUG == 1 */
