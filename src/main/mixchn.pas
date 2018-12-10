(* mixchn.pas -- Pascal declarations for mixchn.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixchn;

interface

uses
    types,
    musins;

(*$I defines.pas*)

type
    TMIXCHNFLAGS = Byte;

const
    MIXCHNFL_ENABLED = $01;
    MIXCHNFL_PLAYING = $02;
    MIXCHNFL_MIXING  = $04;

const
    EFFFLAG_CONTINUE = $01;

type
    TChannel = packed record
        bChannelFlags: TMIXCHNFLAGS;
        bChannelType:   byte;
        pMusIns:        PMUSINS;
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
    mod_Channels: TChannelArray;
    mod_ChannelsCount: byte;

procedure mixchn_set_flags;
procedure mixchn_get_flags;
procedure mixchn_set_enabled;
procedure mixchn_is_enabled;
procedure mixchn_set_playing;
procedure mixchn_is_playing;
procedure mixchn_set_mixing;
procedure mixchn_is_mixing;
procedure mixchn_set_type;
procedure mixchn_get_type;
procedure mixchn_set_instrument_num;
procedure mixchn_get_instrument_num;
procedure mixchn_set_instrument;
procedure mixchn_get_instrument;
procedure mixchn_set_sample_volume;
procedure mixchn_get_sample_volume;
procedure mixchn_set_sample_period_limits;
procedure mixchn_check_sample_period;
procedure mixchn_set_sample_period;
procedure mixchn_get_sample_period;
procedure mixchn_set_sample_step;
procedure mixchn_get_sample_step;
procedure mixchn_setup_sample_period;
procedure mixchn_set_sample_data;
procedure mixchn_get_sample_data;
procedure mixchn_set_command;
procedure mixchn_get_command;
procedure mixchn_set_sub_command;
procedure mixchn_get_sub_command;
procedure mixchn_set_command_parameter;
procedure mixchn_get_command_parameter;
procedure mixchn_reset_wave_tables;
procedure chn_setupInstrument;
procedure chn_calcNotePeriod;
procedure chn_calcNoteStep;
procedure chn_setupNote;

implementation

uses
    watcom,
    effvars,
    s3mvars,
    mixer;

(*$l mixchn.obj*)

procedure mixchn_set_flags; external;
procedure mixchn_get_flags; external;
procedure mixchn_set_enabled; external;
procedure mixchn_is_enabled; external;
procedure mixchn_set_playing; external;
procedure mixchn_is_playing; external;
procedure mixchn_set_mixing; external;
procedure mixchn_is_mixing; external;
procedure mixchn_set_type; external;
procedure mixchn_get_type; external;
procedure mixchn_set_instrument_num; external;
procedure mixchn_get_instrument_num; external;
procedure mixchn_set_instrument; external;
procedure mixchn_get_instrument; external;
procedure mixchn_set_sample_volume; external;
procedure mixchn_get_sample_volume; external;
procedure mixchn_set_sample_period_limits; external;
procedure mixchn_check_sample_period; external;
procedure mixchn_set_sample_period; external;
procedure mixchn_get_sample_period; external;
procedure mixchn_set_sample_step; external;
procedure mixchn_get_sample_step; external;
procedure mixchn_setup_sample_period; external;
procedure mixchn_set_sample_data; external;
procedure mixchn_get_sample_data; external;
procedure mixchn_set_command; external;
procedure mixchn_get_command; external;
procedure mixchn_set_sub_command; external;
procedure mixchn_get_sub_command; external;
procedure mixchn_set_command_parameter; external;
procedure mixchn_get_command_parameter; external;
procedure mixchn_reset_wave_tables; external;
procedure chn_setupInstrument; external;
procedure chn_calcNotePeriod; external;
procedure chn_calcNoteStep; external;
procedure chn_setupNote; external;

end.
