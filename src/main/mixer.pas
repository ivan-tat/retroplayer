(* mixer.pas -- Pascal declarations for mixer.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixer;

interface

(*$I defines.pas*)

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

procedure smpbuf_init;
procedure smpbuf_alloc;
procedure smpbuf_get;
procedure smpbuf_get_length;
procedure smpbuf_free;

procedure mixbuf_init;
procedure mixbuf_alloc;
procedure mixbuf_get;
procedure mixbuf_get_length;
procedure mixbuf_set_channels;
procedure mixbuf_get_channels;
procedure mixbuf_set_samples_per_channel;
procedure mixbuf_get_samples_per_channel;
procedure mixbuf_set_mode;
procedure mixbuf_get_offset_from_count;
procedure mixbuf_get_count_from_offset;
procedure mixbuf_free;

procedure __mixer_set_flags;
procedure mixer_init;
procedure mixer_alloc_buffers;
procedure mixer_free_buffers;
procedure mixer_free;

implementation

uses
    watcom,
    i86,
    string_,
    dos_,
    common,
    debug;

(*$l asm\mixer_.obj*)

procedure _MixSampleMono8; external;
procedure _MixSampleStereo8; external;

(*$l mixer.obj*)

procedure fill_8; external;
procedure fill_16; external;
procedure fill_32; external;

procedure _calc_sample_step; external;

procedure smpbuf_init; external;
procedure smpbuf_alloc; external;
procedure smpbuf_get; external;
procedure smpbuf_get_length; external;
procedure smpbuf_free; external;

procedure mixbuf_init; external;
procedure mixbuf_alloc; external;
procedure mixbuf_get; external;
procedure mixbuf_get_length; external;
procedure mixbuf_set_channels; external;
procedure mixbuf_get_channels; external;
procedure mixbuf_set_samples_per_channel; external;
procedure mixbuf_get_samples_per_channel; external;
procedure mixbuf_set_mode; external;
procedure mixbuf_get_offset_from_count; external;
procedure mixbuf_get_count_from_offset; external;
procedure mixbuf_free; external;

procedure __mixer_set_flags; external;
procedure mixer_init; external;
procedure mixer_alloc_buffers; external;
procedure mixer_free_buffers; external;
procedure mixer_free; external;

end.
