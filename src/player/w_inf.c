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
    MUSPATLIST *patterns;
    MUSPAT *pat;

    if (scrwin_is_created(self))
    {
        patterns = mod_Patterns;
        textbackground(_blue);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            textcolor(_white);
            clrscr();
            gotoxy(2, 1);
            printf("Title:........................... Type:...............................");
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
            printf("%s", mod_Title);
            gotoxy(41, 1);
            printf("%s", mod_TrackerName);
            textcolor(_yellow);
            gotoxy(9, 4);
            printf("%s", player_device_get_name());
            gotoxy(43, 4);
            printf("%05u", player_get_output_rate());
            gotoxy(58, 4);
            printf("%hu", player_get_output_channels());
            gotoxy(65, 4);
            printf("%02hu", player_get_output_bits());
            gotoxy(76, 4);
            printf(player_get_output_lq() ? "Low" : "High");
        }

        pat = muspatl_get (patterns, playState_pattern);

        textcolor(_lightcyan);
        gotoxy(8, 2);
        printf("%03u", player_get_speed());
        gotoxy(18, 2);
        printf("%03u", player_get_tempo());
        gotoxy(27, 2);
        printf("%02u", playState_gVolume);
        gotoxy(35, 2);
        printf("%03u", player_get_master_volume());
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
        printf("%02u", player_get_pattern_delay());
        gotoxy(65, 3);
        printf("%03u", playState_patLoopStartRow);
        gotoxy(69, 3);
        if (playState_patLoopActive)
            printf("%03u", playState_patLoopCount);
        else
            printf("---");
    }
}
