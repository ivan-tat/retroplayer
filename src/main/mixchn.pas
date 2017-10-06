(* mixchn.pas -- Pascal declarations for mixchn.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixchn;

interface

uses types;

(*$I defines.pas*)

const
    EFFFLAG_CONTINUE = $01;

type
    TChannel = record
        bEnabled:       boolean;
        bChannelType:   byte;
        wInsSeg:        word;
        wSmpSeg:        word;
        bIns:           byte;
        bNote:          byte;
        bSmpVol:        byte;
        bSmpFlags:      byte;
        wSmpStart:      word;
        wSmpLoopStart:  word;
        wSmpLoopEnd:    word;
        dSmpPos:        dword;
        dSmpStep:       dword;
        wSmpPeriod:     word;
        wSmpPeriodLow:  word;
        wSmpPeriodHigh: word;
        bCommand:       byte;
        bCommand2:      byte;
        bParameter:     byte;
        bEffFlags:      byte;
        wVibTab:        word;
        wTrmTab:        word;
        bTabPos:        byte;
        bVibParam:      byte;
        bPortParam:     byte;
        wSmpPeriodOld:  word;
        bSmpVolOld:     byte;
        wSmpPeriodDest: word;
        bArpPos:        byte;
        bArpNotes:      array [0..1] of byte;
        dArpSmpSteps:   array [0..2] of dword;
        bRetrigTicks:   byte;
        bSavNote:       byte;
        bSavIns:        byte;
        bSavVol:        byte;
        bDelayTicks:    byte;
    end;

const
    MAX_CHANNELS = 32; (* 0..31 channels *)

type
    TChannelArray = array [0..MAX_CHANNELS-1] of TChannel;
        (* all public/private data for every channel *)

var
    Channel: TChannelArray;
    UsedChannels: byte;

procedure chn_setSampleVolume(var chn: TChannel; vol: integer);
function  chn_checkSamplePeriod(var chn: TChannel; period: longint): word;
procedure chn_setSamplePeriod(var chn: TChannel; period: longint);
procedure chn_setPeriodLimits(var chn: TChannel; rate: word; amiga: boolean);

implementation

uses
	watcom,
	mixvars,
	mixer;

(*$l mixchn.obj*)

procedure chn_setSampleVolume(var chn: TChannel; vol: integer); external;
function  chn_checkSamplePeriod(var chn: TChannel; period: longint): word; external;
procedure chn_setSamplePeriod(var chn: TChannel; period: longint); external;
procedure chn_setPeriodLimits(var chn: TChannel; rate: word; amiga: boolean); external;

end.
