(* s3mvars.pas -- Pascal declarations for s3mvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mvars;

interface

uses
    s3mtypes;

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
    Instruments: ^TInstrArray;
    InsNum: word;
    EMSSmp: boolean;
    SmpEMSHandle: word;

(* patterns *)

var
    Pattern: TPatternsArray;
    patListCount: word;
    patListPatLength: word;
    patListUseEM: boolean;
    patListEMHandle: word;
    patListPatPerEMPage: byte;

procedure pat_clear(pat: PMUSPAT);
procedure patSetData(pat: PMUSPAT; p: pointer);
procedure patSetDataInEM(pat: PMUSPAT; logpage, part: byte);
function  patIsDataInEM(pat: PMUSPAT): boolean;
function  patGetData(pat: PMUSPAT): pointer;
function  patGetDataLogPageInEM(pat: PMUSPAT): byte;
function  patGetDataPartInEM(pat: PMUSPAT): byte;
function  patMapData(pat: PMUSPAT): pointer;

procedure patList_set(index: integer; pat: PMUSPAT);
function  patList_get(index: integer): PMUSPAT;
function  patListGetUsedEM: longint;
procedure patListFree;
procedure patListInit;
procedure patListDone;

(* song arrangment *)

var
    Order: TOrderArray;
    OrdNum: word;
    LastOrder: byte;
{$IFDEF DEBUG}
    StartOrder: word;
{$ENDIF}

(* channels *)

var
    Channel: TChannelArray;
    UsedChannels: byte;

(* initial state *)

var
    InitTempo: byte;
    InitSpeed: byte;

(* play state *)

var
    EndOfSong: boolean;
    CurTempo: byte;
    CurSpeed: byte;
    GVolume: byte;
    MVolume: byte;

(* position in song - you can change it while playing to jump arround *)

var
    CurOrder: byte;
    CurPattern: byte;
    CurLine: byte;
    CurTick: byte;

(* pattern loop *)

var
    PLoop_On: boolean;
    PLoop_No: byte;
    PLoop_To: byte;

(* pattern delay *)

var
    PatternDelay: byte;

implementation

uses
    dos_,
    emstool;

(*$l s3mvars.obj*)

procedure pat_clear(pat: PMUSPAT); external;
procedure patSetData(pat: PMUSPAT; p: pointer); external;
procedure patSetDataInEM(pat: PMUSPAT; logpage, part: byte); external;
function  patIsDataInEM(pat: PMUSPAT): boolean; external;
function  patGetData(pat: PMUSPAT): pointer; external;
function  patGetDataLogPageInEM(pat: PMUSPAT): byte; external;
function  patGetDataPartInEM(pat: PMUSPAT): byte; external;
function  patMapData(pat: PMUSPAT): pointer; external;

procedure patList_set(index: integer; pat: PMUSPAT); external;
function  patList_get(index: integer): PMUSPAT; external;
function  patListGetUsedEM: longint; external;
procedure patListFree; external;
procedure patListInit; external;
procedure patListDone; external;

end.
