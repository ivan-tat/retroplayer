(* s3mtypes.pas -- Pascal declarations for s3mtypes.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mtypes;

interface

uses types;

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
    MAX_PATTERNS = 100; (* 0..99 patterns *)

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

(* patterns *)

type
    TPattern = word;
        (* segment for pattern's data *)

type
    TPatternsArray = array [0..MAX_PATTERNS-1] of TPattern;
        (* segment for every pattern *)

(* song arrangement *)

type
    TOrderEntry = byte;

type
    TOrderArray = array [0..MAX_ORDERS] of TOrderEntry;
        (* song arrangement *)

(* channels *)

const
    MAX_CHANNELS = 32; (* 0..31 channels *)

type
    TChannel = record
        { general switches : }
        enabled     :boolean;  { byte: flag if =0 then nothing to mix at the moment }
        channeltyp  :byte;     { 0=off,1=left,2=right,3,4=adlib ... if 0,3,4 -> everything ignored ! }
        { current sampledata : }
        InstrSEG    :word;     { pointer to current instrument data }
        SampleSEG   :word;     { DOS segment of current sampledata }
        InstrNo    :byte;      { number of instrument is currently playing }
        Note        :byte;     { Notenumber is currently playing (except if effects change it...) }
        { copy of sampledata (maybe it differs a bit): }
        SampleVol   :byte;     { current sample volume }
        sLoopflag   :boolean;  { flag if we have to loop sample }
        sSmpstart   :word;     { default is 0, but if there's a set sample offset, it changes }
        sLoopstart  :word;     { loop start of current sample =0ffffh if no loop }
        sLoopend    :word;     { loop end of current sample }
        sCurPos     :dword;    { fixed point value for current position in sample }
        sStep       :dword;    { fixed point value of frequency step (distance of one step
                                depends on period we play currently) }
        sPeriod     :word;     { st3 period ... you know these amiga values (look at tech.doc of ST3) }
                              { period does no influence playing a sample direct, but it's for sliding etc. }
        lower_border:word;     { B-7 or B-5 period for current instrument to check limits }
        upper_border:word;     { C-0 or C-3 period for current instrument to check limits }
        { effect info : }
        command     :word;     { 2 times effectnumber (for using a jmptable) }
        cmd2nd      :word;     { 2 times additional command for multiple effects }
        parameter   :byte;     { just the parameters }
        { data for handling effects : }
        continueEf  :boolean;  { Flag if we should continue effect - vibrato,tremolo }
        VibtabOfs   :word;     { yo for each channel its own choise (default = OFS sinuswave) }
        TrmtabOfs   :word;     { = Offset of wavetable for tremolo }
        tablepos    :byte;     { <- we reset this if a effect starts uses such a table }
        VibPara     :byte;     { <- for dual command Vib + Vol }
        PortPara    :byte;     { <- for dual command Port + Vol }
        OldPeriod   :word;     { save that value for some effects }
        Oldvolume   :byte;     { save that value for tremolo }
        WantedPeri  :word;     { <- period to slide to with Portamento }
        ArpegPos    :byte;     { which of those 3 notes we currently play ... }
        note1       :byte;     { \ }
        note2       :byte;     { -+ note : 3 notes we do arpeggio between }
        Step0       :dword;    { \ }
        Step1       :dword;    {  |- the 3 step values we switch between in arpeggio effect (0 is start value
                                 <- we have to refesh after arpeggio) }
        Step2       :dword;    { / }
        ctick       :byte;     { ticks left to retrigg not }
        savNote     :byte;     { \  }
        savInst     :byte;     {  | - new values for notedelay ... }
        SavVol      :byte;     {  | }
        ndTick      :byte;     { /  }
    end;

type
    TChannelArray = array [0..MAX_CHANNELS-1] of TChannel;
        (* all public/private data for every channel *)

implementation

end.
