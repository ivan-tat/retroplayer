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
    PatNum: word;
    PatLength: word;
    EMSPat: boolean;
    PatEMSHandle: word;
    PatPerPage: byte;

procedure setPattern(index: integer; p_seg: word);
procedure setPatternInEM(index: integer; logpage, part: byte);
function  getPattern(index: integer): pointer;
function  isPatternInEM(index: integer): boolean;
function  getPatternLogPageInEM(index: integer): byte;
function  getPatternPartInEM(index: integer): byte;
function  getusedEMSpat:longint;
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
    dosproc,
    emstool;

(*$l s3mvars.obj*)

(* Patterns >> *)

procedure setPattern(index: integer; p_seg: word);
begin
    PATTERN[index] := p_seg;
end;

procedure setPatternInEM(index: integer; logpage, part: byte);
begin
    PATTERN[index] := $C000 + ((part and $3f) shl 8) + logpage;
end;

function getPattern(index: integer): pointer;
var
    p_seg: word;
begin
    p_seg := PATTERN[index];
    if (p_seg >= $C000) then
        getPattern := ptr(FrameSEG[0], ((p_seg shr 8) and $3f) * patlength)
    else
        getPattern := ptr(p_seg, 0);
end;

function isPatternInEM(index: integer): boolean;
begin
    isPatternInEM := PATTERN[index] >= $C000;
end;

function getPatternLogPageInEM(index: integer): byte;
begin
    getPatternLogPageInEM := PATTERN[index] and $ff;
end;

function getPatternPartInEM(index: integer): byte;
begin
    getPatternPartInEM := (PATTERN[index] shr 8) and $3f;
end;

function getusedEMSpat: longint;
begin
    if (EMSpat) then
        getusedEMSpat := 16*EmsGetHandleSize(patEMShandle)
    else
        getusedEMSpat := 0;
end;

procedure patListFree;
var
    i: integer;
    p: pointer;
begin
    for i := 0 to MAX_PATTERNS-1 do
    begin
        if (not isPatternInEM(i)) then
        begin
            p := getPattern(i);
            if (p <> nil) then freedosmem(p);
            setPattern(i, 0);
        end;
    end;
    if (EMSpat) then
    begin
        EMSfree(patEMShandle);
        EMSpat := false;
    end;
end;

procedure patListInit;
var
    i: integer;
begin
  for i := 0 to MAX_PATTERNS-1 do setPattern(i, 0);
end;

procedure patListDone;
begin
    patListFree;
end;

(* << Patterns *)

end.
