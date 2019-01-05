(* s3mvars.pas -- Pascal declarations for s3mvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mvars;

interface

uses
    ems,
    s3mtypes;

(*$I defines.pas*)

(*$ifdef DEBUG_WRITE_OUTPUT_STREAM*)
var
    _debug_stream: array [0..1] of Pointer;
(*$endif*)  (* DEBUG_WRITE_OUTPUT_STREAM *)

(* EMM *)

var
    UseEMS: boolean;
    SavHandle: TEMSHDL;

(* play options *)

var
    playOption_ST3Order: boolean;
    playOption_LoopSong: boolean;

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
    playState_rate: Word;
    playState_tempo: byte;
    playState_speed: byte;
    playState_gVolume: byte;
    playState_mVolume: byte;
    playState_tick_samples_per_channel: Word;

(* position in song - you can change it while playing to jump arround *)

var
    playState_order: byte;
    playState_pattern: byte;
    playState_row: byte;
    playState_tick: byte;
    playState_tick_samples_per_channel_left: Word;

(* pattern loop *)

var
    playState_patLoopActive: boolean;
    playState_patLoopCount: byte;
    playState_patLoopStartRow: byte;

(* pattern delay *)

var
    playState_patDelayCount: byte;

procedure playState_set_speed;
procedure playState_set_tempo;

implementation

uses
    watcom,
    dos_;

(*$l s3mvars.obj*)

procedure playState_set_speed; external;
procedure playState_set_tempo; external;

end.
