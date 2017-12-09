(* mixer.pas -- Pascal declarations for mixer.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixer;

interface

(*$I defines.pas*)

const
    MIXSMPFLAG_16BITS = $01;
    MIXSMPFLAG_LOOP   = $02;

type
    TPlaySampleInfo = packed record
        dData: pointer;
        dPos: longint;
        dStep: longint;
        wLen: word;
        wLoopStart: word;
        wLoopEnd: word;
        wFlags: word;
    end;

procedure _MixSampleMono8;
procedure _MixSampleStereo8;

procedure fill_8;
procedure fill_16;
procedure fill_32;

const
    ST3Periods: array [0..11] of word = (
        1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
    );

procedure _calc_sample_step;

type
    TMIXBUF = packed record
        buf: Pointer;
        size: Word;
        channels: Byte;
        samples_per_channel: Word;
    end;
    PMIXBUF = ^TMIXBUF;

var
    mixBuf: TMIXBUF;

procedure mixbuf_init;
procedure mixbuf_alloc;
procedure mixbuf_set_channels;
procedure mixbuf_set_samples_per_channel;
procedure mixbuf_set_mode;
procedure mixbuf_get_offset_from_count;
procedure mixbuf_get_count_from_offset;
procedure mixbuf_free;

implementation

uses
    watcom,
    i86,
    string_,
    dos_,
    debug;

(*$l mixer_.obj*)

procedure _MixSampleMono8; external;
procedure _MixSampleStereo8; external;

(*$l mixer.obj*)

procedure fill_8; external;
procedure fill_16; external;
procedure fill_32; external;

procedure _calc_sample_step; external;

procedure mixbuf_init; external;
procedure mixbuf_alloc; external;
procedure mixbuf_set_channels; external;
procedure mixbuf_set_samples_per_channel; external;
procedure mixbuf_set_mode; external;
procedure mixbuf_get_offset_from_count; external;
procedure mixbuf_get_count_from_offset; external;
procedure mixbuf_free; external;

end.
