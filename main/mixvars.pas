(* mixvars -- Pascal declarations for mixvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixvars;

interface

const
    ST3Periods: array [0..11] of word = (
        1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
    );

var
    mixBuf: pointer;
    mixChannels: byte;
    mixSampleRate: word;
    mixBufSamplesPerChannel: word;
    mixBufSamples: word;
    mixTickSamplesPerChannel: word;
    mixTickSamplesPerChannelLeft: word;

procedure setMixSampleRate(rate: word);
procedure setMixChannels(channels: byte);
procedure setMixBufSamplesPerChannel(count: word);
procedure setMixMode(channels: byte; rate, count: word);
function  getMixBufOffFromCount(count: word): word;
function  getCountFromMixBufOff(bufOff: word): word;

implementation

(*$l mixvars.obj*)
procedure setMixSampleRate(rate: word); external;
procedure setMixChannels(channels: byte); external;
procedure setMixBufSamplesPerChannel(count: word); external;
procedure setMixMode(channels: byte; rate, count: word); external;
function  getMixBufOffFromCount(count: word): word; external;
function  getCountFromMixBufOff(bufOff: word): word; external;

end.
