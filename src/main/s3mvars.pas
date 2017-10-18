(* s3mvars.pas -- Pascal declarations for s3mvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mvars;

interface

uses
    s3mtypes;

(*$I defines.pas*)

(* EMM *)

var
    UseEMS: boolean;

(* general module information *)

var
    mod_isLoaded: boolean;
    mod_Title: TModTitle;
    mod_TrackerName: TModTrackerName;

(* module options *)

var
    modOption_ST2Vibrato: boolean;
    modOption_ST2Tempo: boolean;
    modOption_AmigaSlides: boolean;
    modOption_SBfilter: boolean;
    modOption_CostumeFlag: boolean;
    modOption_VolZeroOptim: boolean;
    modOption_AmigaLimits: boolean;
    modOption_SignedData: boolean;
    modOption_Stereo: boolean;

(* play options *)

var
    playOption_ST3Order: boolean;
    playOption_LoopSong: boolean;

(* instruments *)

var
    mod_Instruments: PInstrArray;
    InsNum: word;
    EMSSmp: boolean;
    SmpEMSHandle: word;

(* song arrangment *)

var
    Order: TOrderArray;
    OrdNum: word;
    LastOrder: byte;

(* initial state *)

var
    initState_tempo: byte;
    initState_speed: byte;
    initState_startOrder: word;

(* play state *)

var
    playState_songEnded: boolean;
    playState_tempo: byte;
    playState_speed: byte;
    playState_gVolume: byte;
    playState_mVolume: byte;

(* position in song - you can change it while playing to jump arround *)

var
    playState_order: byte;
    playState_pattern: byte;
    playState_row: byte;
    playState_tick: byte;

(* pattern loop *)

var
    playState_patLoopActive: boolean;
    playState_patLoopCount: byte;
    playState_patLoopStartRow: byte;

(* pattern delay *)

var
    playState_patDelayCount: byte;

implementation

uses
    dos_,
    ems;

(*$l s3mvars.obj*)

end.
