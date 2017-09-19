(* sndctl_t.pas -- type declarations for sndctl.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit sndctl_t;

interface

type
    THWSMPFMTFLAGS = byte;

const
    HWSMPFMTFL_BITS_MASK = $7f;
    HWSMPFMTFL_SIGNED    = $80;

type
    THWSMPFMT = record
        flags: THWSMPFMTFLAGS;
        channels: byte;
    end;
    PHWSMPFMT = ^THWSMPFMT;

function  set_sample_format(p: PHWSMPFMT; b: byte; s: boolean; c: byte): boolean;
function  get_sample_format_bits(p: PHWSMPFMT): byte;
function  is_sample_format_signed(p: PHWSMPFMT): boolean;
function  get_sample_format_channels(p: PHWSMPFMT): byte;
function  get_sample_format_width(p: PHWSMPFMT): word;
procedure clear_sample_format(p: PHWSMPFMT);

implementation

(*$l sndctl_t.obj*)

function  set_sample_format(p: PHWSMPFMT; b: byte; s: boolean; c: byte): boolean; external;
function  get_sample_format_bits(p: PHWSMPFMT): byte; external;
function  is_sample_format_signed(p: PHWSMPFMT): boolean; external;
function  get_sample_format_channels(p: PHWSMPFMT): byte; external;
function  get_sample_format_width(p: PHWSMPFMT): word; external;
procedure clear_sample_format(p: PHWSMPFMT); external;

end.
