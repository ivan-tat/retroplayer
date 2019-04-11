/* w_inf.c -- information window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "common.h"
#include "hw/sb/sbctl.h"
#include "main/muspat.h"
#include "main/musmod.h"
#include "main/musmodps.h"
#include "main/mixer.h"
#include "main/s3mplay.h"
#include "player/screen.h"

#include "player/w_inf.h"

void __far win_information_on_resize (SCRWIN *self);
void __far win_information_draw (SCRWIN *self);
/*
void __far win_information_keypress (SCRWIN *self, char key);
*/
void __far win_information_free (SCRWIN *self);

static const SCRWINVMT __win_information_vmt =
{
    &win_information_on_resize,
    &win_information_draw,
    /*
    &win_information_keypress,
    */
    NULL,
    &win_information_free
};

/* private data */

typedef struct win_information_data_t
{
    MUSPLAYER *player;
    MUSMOD *track;
    PLAYSTATE *ps;
};

static const char *mixq[MIXQ_MAX + 1] =
{
    "nearest",
    "fastest"
};

/* private methods */

/* public methods */

bool __far win_information_init (SCRWIN *self)
{
    struct win_information_data_t *data;

    scrwin_init (self, "information window");
    _copy_vmt (self, __win_information_vmt, SCRWINVMT);
    data = _new (struct win_information_data_t);
    if (!data)
        return false;
    scrwin_set_data (self, data);
    memset (data, 0, sizeof (struct win_information_data_t));
    return true;
}

void __far win_information_set_player (SCRWIN *self, MUSPLAYER *value)
{
    struct win_information_data_t *data;

    data = (struct win_information_data_t *) scrwin_get_data (self);
    data->player = value;
}
void __far win_information_set_track (SCRWIN *self, MUSMOD *value)
{
    struct win_information_data_t *data;

    data = (struct win_information_data_t *) scrwin_get_data (self);
    data->track = value;
}

void __far win_information_set_play_state (SCRWIN *self, PLAYSTATE *value)
{
    struct win_information_data_t *data;

    data = (struct win_information_data_t *) scrwin_get_data (self);
    data->ps = value;
}

void __far win_information_on_resize (SCRWIN *self)
{
}

void __far win_information_draw(SCRWIN *self)
{
    struct win_information_data_t *data;
    MUSPLAYER *player;
    MUSMOD *track;
    PLAYSTATE *ps;
    MUSPATLIST *patterns;
    MUSPAT *pat;
    MIXER *mixer;

    if (scrwin_is_created(self))
    {
        data = (struct win_information_data_t *) scrwin_get_data (self);
        player = data->player;
        track = data->track;
        ps = player_get_play_state (player);
        patterns = musmod_get_patterns (track);
        pat = muspatl_get (patterns, ps->pattern);
        mixer = player_get_mixer (player);

        textbackground(_blue);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            textcolor(_white);
            clrscr();
            gotoxy(2, 1);
            printf("Title:........................... Type:...............................");
            gotoxy(2, 2);
            printf("Speed:... Tempo:... GVol:.. MVol:... Order:........ Loop:... MixQ:.......");
            gotoxy(2, 3);
            printf("Pos:.../... Pat:..(.....) Row:.../... Tick:... PDelay:.. PLoop:...(...)");
            gotoxy(2, 4);
            printf("Device:............................ Rate:..... Channels:. Bits:.. Quality:....");
            window(self->rect.x0, self->rect.y0, self->rect.x1, self->rect.y1 + 1);
            gotoxy(1, 5);
            textcolor(_black);
            printf(
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
                "\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc"
            );
            window(self->rect.x0, self->rect.y0, self->rect.x1, self->rect.y1);
            textbackground(_lightgray);
            textcolor(_lightgreen);
            gotoxy(8, 1);
            textbackground(_blue);
            printf("%s", musmod_get_title (track));
            gotoxy(41, 1);
            printf("%s", musmod_get_format (track));
            textcolor(_yellow);
            gotoxy(9, 4);
            printf("%s", player_device_get_name (player));
            gotoxy(43, 4);
            printf("%05u", player_get_output_rate (player));
            gotoxy(58, 4);
            printf("%hu", player_get_output_channels (player));
            gotoxy(65, 4);
            printf("%02hu", player_get_output_bits (player));
            gotoxy(76, 4);
            printf(player_get_output_lq (player) ? "Low" : "High");
        }

        textcolor(_lightcyan);
        gotoxy(8, 2);
        printf("%03u", ps->speed);
        gotoxy(18, 2);
        printf("%03u", ps->tempo);
        gotoxy(27, 2);
        printf("%02u", ps->global_volume);
        gotoxy(35, 2);
        printf("%03u", player_get_master_volume (player));
        gotoxy(45, 2);
        printf ((ps->flags & PLAYSTATEFL_SKIPENDMARK) ? "Extended" : "Normal");
        gotoxy(59, 2);
        printf ((ps->flags & PLAYSTATEFL_SONGLOOP) ? "On" : "Off");
        gotoxy (68, 2);
        printf (mixq[mixer_get_quality (mixer)]);
        gotoxy(6, 3);
        printf("%03u", ps->order);
        gotoxy(10, 3);
        printf("%03u", ps->order_last);
        gotoxy(18, 3);
        printf("%02u", ps->pattern);
        gotoxy(21, 3);
        if (muspat_is_EM_data(pat))
            printf("E%04X", muspat_get_EM_data_page(pat));
        else
            printf("D%04X", FP_SEG(muspat_get_data(pat)));
        gotoxy(32, 3);
        printf("%03u", ps->row);
        gotoxy(36, 3);
        printf("%03u", muspat_get_rows(pat));
        gotoxy(45, 3);
        printf("%03u", ps->tick);
        gotoxy(56, 3);
        printf("%02u", ps->patdelay_count);
        gotoxy(65, 3);
        printf("%03u", ps->patloop_start_row);
        gotoxy(69, 3);
        if (ps->flags & PLAYSTATEFL_PATLOOP)
            printf("%03u", ps->patloop_count);
        else
            printf("---");
    }
}

/*
void __far win_information_keypress (SCRWIN *self, char key)
{
}
*/

/* free */

void __far win_information_free (SCRWIN *self)
{
    if (scrwin_get_data (self))
        _delete (scrwin_get_data (self));
}
