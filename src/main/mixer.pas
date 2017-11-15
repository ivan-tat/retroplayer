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

procedure _MixSampleMono8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word);
procedure _MixSampleStereo8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word);

const
    ST3Periods: array [0..11] of word = (
        1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
    );

function  _calc_sample_step(wPeriod: word): longint;

type
    TMIXBUF = packed record
        buf: Pointer;
        size: Word;
    end;
    PMIXBUF = ^TMIXBUF;

var
    mixBuf: TMIXBUF;
    mixChannels: byte;
    mixSampleRate: word;
    mixBufSamplesPerChannel: word;
    mixBufSamples: word;
    mixTickSamplesPerChannel: word;
    mixTickSamplesPerChannelLeft: word;

procedure mixbuf_init(self: PMIXBUF);
function  mixbuf_alloc(self: PMIXBUF; size: Word): Boolean;
procedure mixbuf_free(self: PMIXBUF);
procedure setMixSampleRate(rate: word);
procedure setMixChannels(channels: byte);
procedure setMixBufSamplesPerChannel(count: word);
procedure setMixMode(channels: byte; rate, count: word);
function  getMixBufOffFromCount(count: word): word;
function  getCountFromMixBufOff(bufOff: word): word;

implementation

uses
    watcom,
    i86,
    string_,
    dos_,
    debug;

(*$l mixer_.obj*)

procedure _MixSampleMono8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word); external;
procedure _MixSampleStereo8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word); external;

(*$l mixer.obj*)

function  _calc_sample_step(wPeriod: word): longint; external;

procedure mixbuf_init(self: PMIXBUF); external;
function  mixbuf_alloc(self: PMIXBUF; size: Word): Boolean; external;
procedure mixbuf_free(self: PMIXBUF); external;
procedure setMixSampleRate(rate: word); external;
procedure setMixChannels(channels: byte); external;
procedure setMixBufSamplesPerChannel(count: word); external;
procedure setMixMode(channels: byte; rate, count: word); external;
function  getMixBufOffFromCount(count: word): word; external;
function  getCountFromMixBufOff(bufOff: word): word; external;

end.
