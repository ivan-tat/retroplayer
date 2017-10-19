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
    PMIXCHN = ^TChannel;

const
    MAX_CHANNELS = 32; (* 0..31 channels *)

type
    TChannelArray = array [0..MAX_CHANNELS-1] of TChannel;
        (* all public/private data for every channel *)

var
    Channel: TChannelArray;
    UsedChannels: byte;

procedure mixchn_set_enabled(self: PMIXCHN; value: Boolean);
function  mixchn_is_enabled(self: PMIXCHN): Boolean;
procedure mixchn_set_type(self: PMIXCHN; value: Byte);
function  mixchn_get_type(self: PMIXCHN): Byte;
procedure mixchn_set_instrument_num(self: PMIXCHN; value: Byte);
function  mixchn_get_instrument_num(self: PMIXCHN): Byte;
procedure mixchn_set_instrument(self: PMIXCHN; value: Pointer);
function  mixchn_get_instrument(self: PMIXCHN): Pointer;
procedure mixchn_set_sample_volume(self: PMIXCHN; value: Integer);
function  mixchn_get_sample_volume(self: PMIXCHN): Byte;
procedure mixchn_set_sample_period_limits(self: PMIXCHN; rate: Word; amiga: Boolean);
function  mixchn_check_sample_period(self: PMIXCHN; value: LongInt): Word;
procedure mixchn_set_sample_period(self: PMIXCHN; value: Word);
function  mixchn_get_sample_period(self: PMIXCHN): Word;
procedure mixchn_set_sample_step(self: PMIXCHN; value: LongInt);
function  mixchn_get_sample_step(self: PMIXCHN): LongInt;
procedure mixchn_setup_sample_period(self: PMIXCHN; value: LongInt);
procedure mixchn_set_sample_data(self: PMIXCHN; value: Pointer);
function  mixchn_get_sample_data(self: PMIXCHN): Pointer;
procedure mixchn_set_command(self: PMIXCHN; value: Byte);
function  mixchn_get_command(self: PMIXCHN): Byte;
procedure mixchn_set_sub_command(self: PMIXCHN; value: Byte);
function  mixchn_get_sub_command(self: PMIXCHN): Byte;
procedure mixchn_set_command_parameter(self: PMIXCHN; value: Byte);
function  mixchn_get_command_parameter(self: PMIXCHN): Byte;

implementation

uses
	watcom,
	mixvars,
	mixer;

(*$l mixchn.obj*)

procedure mixchn_set_enabled(self: PMIXCHN; value: Boolean); external;
function  mixchn_is_enabled(self: PMIXCHN): Boolean; external;
procedure mixchn_set_type(self: PMIXCHN; value: Byte); external;
function  mixchn_get_type(self: PMIXCHN): Byte; external;
procedure mixchn_set_instrument_num(self: PMIXCHN; value: Byte); external;
function  mixchn_get_instrument_num(self: PMIXCHN): Byte; external;
procedure mixchn_set_instrument(self: PMIXCHN; value: Pointer); external;
function  mixchn_get_instrument(self: PMIXCHN): Pointer; external;
procedure mixchn_set_sample_volume(self: PMIXCHN; value: Integer); external;
function  mixchn_get_sample_volume(self: PMIXCHN): Byte; external;
procedure mixchn_set_sample_period_limits(self: PMIXCHN; rate: Word; amiga: Boolean); external;
function  mixchn_check_sample_period(self: PMIXCHN; value: LongInt): Word; external;
procedure mixchn_set_sample_period(self: PMIXCHN; value: Word); external;
function  mixchn_get_sample_period(self: PMIXCHN): Word; external;
procedure mixchn_set_sample_step(self: PMIXCHN; value: LongInt); external;
function  mixchn_get_sample_step(self: PMIXCHN): LongInt; external;
procedure mixchn_setup_sample_period(self: PMIXCHN; value: LongInt); external;
procedure mixchn_set_sample_data(self: PMIXCHN; value: Pointer); external;
function  mixchn_get_sample_data(self: PMIXCHN): Pointer; external;
procedure mixchn_set_command(self: PMIXCHN; value: Byte); external;
function  mixchn_get_command(self: PMIXCHN): Byte; external;
procedure mixchn_set_sub_command(self: PMIXCHN; value: Byte); external;
function  mixchn_get_sub_command(self: PMIXCHN): Byte; external;
procedure mixchn_set_command_parameter(self: PMIXCHN; value: Byte); external;
function  mixchn_get_command_parameter(self: PMIXCHN): Byte; external;

end.
