(* sbctl.pas -- Pascal declarations for sbctl.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

{$B-,D+,E-,F-,G+,L+,O-,Q-,R-,S-,Y+}
{$M 16384,0,655360}

unit sbctl;

interface

(*$I defines.pas*)

procedure sb_new;
procedure sb_init;
procedure sb_conf_manual;
procedure sb_conf_detect;
procedure sb_conf_env;
procedure sb_conf_input;
procedure sb_conf_dump;
procedure sb_get_name;
procedure sb_set_volume;
procedure sb_set_transfer_buffer;
procedure sb_adjust_transfer_mode;
procedure sb_set_transfer_mode;
procedure sb_mode_get_bits;
procedure sb_mode_is_signed;
procedure sb_mode_get_channels;
procedure sb_mode_get_rate;
procedure sb_transfer_start;
procedure sb_get_buffer_pos;
procedure sb_transfer_pause;
procedure sb_transfer_continue;
procedure sb_transfer_stop;
procedure sb_free;
procedure sb_delete;

procedure register_sbctl;

implementation

uses
    pascal,
    watcom,
    strutils,
    i86,
    stdio,
    conio,
    stdlib,
    string_,
    dos_,
    errno_,
    dos,
    debug,
    hwowner,
    dma,
    pic,
    sndctl_t,
    sbio;

(*$l sbctl.obj*)

procedure sb_new; external;
procedure sb_init; external;
procedure sb_conf_manual; external;
procedure sb_conf_detect; external;
procedure sb_conf_env; external;
procedure sb_conf_input; external;
procedure sb_conf_dump; external;
procedure sb_get_name; external;
procedure sb_set_volume; external;
procedure sb_set_transfer_buffer; external;
procedure sb_adjust_transfer_mode; external;
procedure sb_set_transfer_mode; external;
procedure sb_mode_get_bits; external;
procedure sb_mode_is_signed; external;
procedure sb_mode_get_channels; external;
procedure sb_mode_get_rate; external;
procedure sb_transfer_start; external;
procedure sb_get_buffer_pos; external;
procedure sb_transfer_pause; external;
procedure sb_transfer_continue; external;
procedure sb_transfer_stop; external;
procedure sb_free; external;
procedure sb_delete; external;

procedure register_sbctl; external;

end.
