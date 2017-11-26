/* w_inf.c -- information window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "hw/sb/sbctl.h"
#include "main/muspat.h"
#include "main/s3mvars.h"
#include "main/s3mplay.h"
#include "player/screen.h"

#include "player/w_inf.h"

void __far win_information_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 1, scrWidth, 5,
        &win_information_draw,
        NULL
    );
}

void __far win_information_draw(SCRWIN *self)
{
    MUSPAT *pat;

    if (scrwin_is_created(self))
    {
        textbackground(_blue);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            textcolor(_white);
            clrscr();
            gotoxy(2, 1);
            printf("Title:........................... Tracker:...............................");
            gotoxy(2, 2);
            printf("Speed:... Tempo:... GVol:.. MVol:... Order:........ Loop:...");
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
             // FIXME: pascal to C conversion
            mod_Title[1 + mod_Title[0]] = 0;
            printf("%s", mod_Title + 1);
            gotoxy(44, 1);
            // FIXME: pascal to C conversion
            mod_TrackerName[1 + mod_TrackerName[0]] = 0;
            printf("%s", mod_TrackerName + 1);
            textcolor(_yellow);
            gotoxy(9, 4);
            printf("%s", sb_get_name(SBDEV_REF_FIXME));
            gotoxy(43, 4);
            printf("%05u", sb_get_rate(SBDEV_REF_FIXME));
            gotoxy(58, 4);
            printf("%hu", sb_get_channels(SBDEV_REF_FIXME));
            gotoxy(65, 4);
            printf("%02hu", sb_get_sample_bits(SBDEV_REF_FIXME));
            gotoxy(76, 4);
            printf(player_is_lq_mode() ? "Low" : "High");
        }

        pat = muspatl_get(mod_Patterns, playState_pattern);

        textcolor(_lightcyan);
        gotoxy(8, 2);
        printf("%03u", playGetSpeed());
        gotoxy(18, 2);
        printf("%03u", playGetTempo());
        gotoxy(27, 2);
        printf("%02u", playState_gVolume);
        gotoxy(35, 2);
        printf("%03u", playGetMasterVolume());
        gotoxy(45, 2);
        printf(playOption_ST3Order ? "Extended" : "Normal");
        gotoxy(59, 2);
        printf(playOption_LoopSong ? "On" : "Off");
        gotoxy(6, 3);
        printf("%03u", playState_order);
        gotoxy(10, 3);
        printf("%03u", LastOrder);
        gotoxy(18, 3);
        printf("%02u", playState_pattern);
        gotoxy(21, 3);
        if (muspat_is_EM_data(pat))
            printf("E%04X", muspat_get_EM_data_page(pat));
        else
            printf("D%04X", FP_SEG(muspat_get_data(pat)));
        gotoxy(32, 3);
        printf("%03u", playState_row);
        gotoxy(36, 3);
        printf("%03u", muspat_get_rows(pat));
        gotoxy(45, 3);
        printf("%03u", playState_tick);
        gotoxy(56, 3);
        printf("%02u", playGetPatternDelay());
        gotoxy(65, 3);
        printf("%03u", playState_patLoopStartRow);
        gotoxy(69, 3);
        if (playState_patLoopActive)
            printf("%03u", playState_patLoopCount);
        else
            printf("---");
    }
}
