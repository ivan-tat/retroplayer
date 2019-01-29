/* w_chn.c -- channels window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "common.h"
#include "main/mixer.h"
#include "main/mixchn.h"
#include "main/effects.h"
#include "player/screen.h"
#include "player/plays3m.h"

#include "player/w_chn.h"

void __far win_channels_on_resize (SCRWIN *self);
void __far win_channels_draw (SCRWIN *self);
/*
void __far win_channels_keypress (SCRWIN *self, char key);
*/
void __far win_channels_free (SCRWIN *self);

static const SCRWINVMT __win_channels_vmt =
{
    &win_channels_on_resize,
    &win_channels_draw,
    /*
    &win_channels_keypress,
    */
    NULL,
    &win_channels_free
};

/* private data */

typedef struct win_channels_data_t
{
    MIXCHNLIST *channels;
};

static const char *CHANTYPES[3] =
{
    "--", "PC", "AD"
};

/* private methods */

/* public methods */

bool __far win_channels_init (SCRWIN *self)
{
    struct win_channels_data_t *data;

    scrwin_init (self, "channels list window");
    _copy_vmt (self, __win_channels_vmt, SCRWINVMT);
    data = _new (struct win_channels_data_t);
    if (!data)
        return false;
    scrwin_set_data (self, data);
    memset (data, 0, sizeof (struct win_channels_data_t));
    return true;
}

void __far win_channels_set_channels (SCRWIN *self, MIXCHNLIST *value)
{
    struct win_channels_data_t *data;

    data = (struct win_channels_data_t *) scrwin_get_data (self);
    data->channels = value;
}

void __far win_channels_on_resize (SCRWIN *self)
{
}

void __far win_channels_draw(SCRWIN *self)
{
    #define _BUF_SIZE 40
    struct win_channels_data_t *data;
    uint8_t i, count;
    MIXCHNLIST *channels;
    MIXCHN *chn;
    MIXCHNTYPE type;
    MIXCHNFLAGS flags;
    uint8_t cmd;
    char flagsstr[6];
    char notestr[4];
    char buf[_BUF_SIZE];
    uint32_t step;

    if (scrwin_is_created(self))
    {
        data = (struct win_channels_data_t *) scrwin_get_data (self);
        channels = data->channels;

        textbackground(_black);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            textcolor(_white);
            clrscr();
            printf("Ch Type Flags Inst Note  Period  Step   Vol Effect");
        }

        textcolor(_lightgray);
        i = 0;
        count = 0;
        while (i < mixchnl_get_count (channels))
        {
            chn = mixchnl_get (channels, i);
            type = mixchn_get_type(chn);
            flags = mixchn_get_flags(chn);
            if  ((type == MIXCHNTYPE_PCM) &&  (mixchnl_get_count (channels) <= 16))
            {
                count++;

                flagsstr[0] = '-';
                flagsstr[1] = '-';
                flagsstr[2] = '-';
                flagsstr[3] = '-';
                flagsstr[4] = '-';
                flagsstr[5] = 0;
                if (flags & MIXCHNFL_ENABLED)
                    flagsstr[0] = 'e';
                if (flags & MIXCHNFL_PLAYING)
                    flagsstr[1] = 'p';
                if (flags & MIXCHNFL_MIXING)
                    flagsstr[2] = 'm';
                if (chn->bSmpFlags & MIXSMPFL_LOOP)
                    flagsstr[3] = 'l';
                if (chn->bEffFlags & EFFFLAG_CONTINUE)
                    flagsstr[4] = 'c';

                if (mixchn_is_enabled(chn))
                    get_note_name (notestr, mixchn_get_note (chn));
                else
                {
                    notestr[0] = '.';
                    notestr[1] = '.';
                    notestr[2] = '.';
                    notestr[3] = 0;
                }

                cmd = mixchn_get_command(chn);
                if (cmd && (cmd <= MAXEFF))
                    chn_effGetName (chn, buf, _BUF_SIZE);
                else
                    buf[0] = 0;

                gotoxy(1, 2 + i);
                step = mixchn_get_sample_step(chn);
                printf("%2hu (%s) %s (%2hu) [%s] %5u %4X.%04X %2hu %s",
                    i+1,
                    CHANTYPES[type],
                    &flagsstr,
                    mixchn_get_instrument_num(chn),
                    &notestr,
                    mixchn_get_sample_period(chn),
                    (uint16_t)(step >> 16),
                    (uint16_t)(step & 0xffff),
                    mixchn_get_note_volume (chn),
                    &buf
                );
                clreol();
            }
            i++;
        }

        if (!count)
            printf("No channels to mix.");
    }
    #undef _BUF_SIZE
}

/*
void __far win_channels_keypress (SCRWIN *self, char key)
{
}
*/

/* free */

void __far win_channels_free (SCRWIN *self)
{
    if (scrwin_get_data (self))
        _delete (scrwin_get_data (self));
}
