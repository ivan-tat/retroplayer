(* s3mtypes.pas -- Pascal declarations for s3mtypes.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mtypes;

interface

uses types;

(*$I defines.pas*)

(* generic module information *)

const
    MOD_MAX_TITLE_LENGTH = 28;

type
    TModTitle = string [MOD_MAX_TITLE_LENGTH];
    PModTitle = ^TModTitle;

const
    MOD_MAX_TRACKER_NAME_LENGTH = 31;

type
    TModTrackerName = string [MOD_MAX_TRACKER_NAME_LENGTH];
    PModTrackerName = ^TModTrackerName;

const
    MAX_ORDERS = 255; (* 0..254 orders *)

type
    THeader = record name:array[0..27] of char;
        charEOF:char;  { should be 1Ah }
        filetyp:byte;
        dummy1 :word;
        ordnum :word;
        Insnum :word;
        Patnum :word;
        Flags  :word;
        CWTV   :word;   (* "Created With Tracker Version" *)
            (* bit  12    = always 1 -> created with Scream Tracker;
               bits 11..8 = major tracker version;
                     7..0 = minor tracker version. *)
        FFv    :word; { fileformatversion }
        SCRM_ID:longint; { should be 'SCRM' }
        gvolume:byte;       { global volume }
        initialspeed:byte;
        initialtempo:byte;
        mvolume:byte;       { mastervolume }
        dummy2 :array[0..9] of byte;
        special:word;       { not used up2now }
        channelset:array[0..31] of byte;
    end;

(* instrument / sample *)

const
    SMPFLAG_LOOP = $01;

type
    PSmpHeader = ^TSmpHeader;
    TSmpHeader = record typ:byte;
        dosname:array[0..11] of char;
        hi_mempos:byte;
        mempos:word;
        length:longint;
        loopbeg:longint;
        loopend:longint;
        vol:byte;
        dummy1:byte;
        packinfo:byte;
        Flags:byte;
        c2speed:longint;
        dummy2:longint;
        GUS_addr:word;
        SB_Flags:word;
        SB_last:longint;
        name:array[0..27] of char;
        SCRS_ID:array[0..3] of char;
    end;

(* adlib instrument *)

type
    PAdlHeader = ^TAdlHeader;
    TAdlHeader = record typ:byte;
        dosname:array[0..11] of char;
        dummy1:array[0..2] of byte;
        Data:array[0..11] of byte;
        Vol:byte;
        Dsk:byte;
        dummy2:word;
        C2spd:longint;
        dummy3:array[0..11] of byte;
        name:array[0..27] of char;
        SCRI_ID:array[0..3] of char;
    end;

(* instruments *)

const
    MAX_INSTRUMENTS = 99; (* 1..99 instruments *)

type
    TInstr = array [0..16*5-1] of byte;
    TInstrArray = array [1..MAX_INSTRUMENTS] of TInstr;

(* song arrangement *)

type
    TOrderEntry = byte;

type
    TOrderArray = array [0..MAX_ORDERS] of TOrderEntry;
        (* song arrangement *)

(* channels *)

const
    MAX_CHANNELS = 32; (* 0..31 channels *)

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

type
    TChannelArray = array [0..MAX_CHANNELS-1] of TChannel;
        (* all public/private data for every channel *)

implementation

end.
