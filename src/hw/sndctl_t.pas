(* sndctl_t.pas -- type declarations for sndctl.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit sndctl_t;

interface

(*$I defines.pas*)

procedure set_sample_format;
procedure get_sample_format_bits;
procedure is_sample_format_signed;
procedure get_sample_format_channels;
procedure get_sample_format_width;
procedure clear_sample_format;

implementation

(*$l sndctl_t.obj*)

procedure set_sample_format; external;
procedure get_sample_format_bits; external;
procedure is_sample_format_signed; external;
procedure get_sample_format_channels; external;
procedure get_sample_format_width; external;
procedure clear_sample_format; external;

end.
