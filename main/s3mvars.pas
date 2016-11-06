(* s3mvars.pas -- Pascal declarations for s3mvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mvars;

interface

uses
    s3mtypes;

(* general module information *)

var
    mod_isLoaded: boolean;
    mod_Title: TModTitle;               (* name given by the musician *)
    mod_TrackerName: TModTrackerName;   (* tracker version file was created with *)

(* module options *)

var
    modOption_ST2Vibrato: boolean;      (* not supported *)
    modOption_ST2Tempo: boolean;        (* not supported *)
    modOption_AmigaSlides: boolean;     (* not supported *)
    modOption_SBfilter: boolean;        (* not supported *)
    modOption_CostumeFlag: boolean;     (* not supported *)
    modOption_VolZeroOptim: boolean;    (* PSIs volume-zero optimization *)
    modOption_AmigaLimits: boolean;     (* check for amiga limits *)
    modOption_SignedData: boolean;      (* signed/unsigned data *)
                                        (* (only volumetable differs in those modes) *)

(* play options *)

var
    playOption_ST3Order: boolean;
        (* if true then handle order like ST3 - if a "--"=255 is found -
            stop or loop to the song start (look playOption_LoopSong) *)
        (* if false - play the whole order and simply skip the "--"
            if (CurOrder==OrdNum) then stop or loop to the beginning *)
    playOption_LoopSong: boolean;
        (* flag if restart if we reach the end of the S3M module *)

(* instruments *)

var
    Instruments: ^TInstrArray;  (* pointer to data for all instruments *)
    InsNum: word;

(* patterns *)

var
    Pattern: TPatternsArray;    (* segment for every pattern *)
                                (* $Fxyy -> at EMS page YY on Offset X*5120 *)
    PatNum: word;
    PatLength: word;            (* length of one pattern *)

(* song arrangment *)

var
    Order: TOrderArray; (* song arrangement *)
    OrdNum: word;
    LastOrder: byte;    (* last order to play *)
{$IFDEF BETATEST}
    StartOrder: word;
{$ENDIF}

(* channels *)

var
    Channel: TChannelArray; (* all public/private data for every channel *)
    UsedChannels: byte;     (* possible values : 1..32 (kill all Adlib) *)

(* initial state *)

var
    InitTempo: byte;    (* initial tempo *)
    InitSpeed: byte;    (* initial speed *)

(* play state *)

var
    EndOfSong: boolean;
    CurTempo: byte;     (* current tempo - count of ticks per note *)
    CurSpeed: byte;     (* current speed - length of one tick *)
    GVolume: byte;      (* global volume -> usedvol = instrvol*gvolume/255 *)
    MVolume: byte;      (* master volume -> calc posttables *)

(* position in song - you can change it while playing to jump arround *)

var
    CurOrder: byte;     (* position in song arrangement *)
    CurPattern: byte;   (* current pattern - is specified also by [curorder] *)
                        (* so it's only for the user ... *)
    CurLine: byte;      (* current line in pattern *)
    CurTick: byte;      (* current tick - we only calc one tick per call *)
                        (* (look at MIXING.ASM) *)

(* pattern loop *)

var
    PLoop_On: boolean;  (* in a Pattern loop? *)
    PLoop_No: byte;     (* number of loops left *)
    PLoop_To: byte;     (* position to loop to *)

(* pattern delay *)

var
    PatternDelay: byte;

(* EMM *)

var
    UseEMS: boolean;
    PatEMSHandle: word; (* handle to access EMS for patterns *)
    SmpEMSHandle: word; (* hanlde to access EMS for samples *)
                        (* I seperated them, but that does not matter, well ? *)
    EMSPat: boolean;    (* patterns in EMS ? *)
    EMSSmp: boolean;    (* samples in EMS ? *)
    PatPerPage: byte;   (* count of patterns per page (<64!!!) *)

implementation

end.
