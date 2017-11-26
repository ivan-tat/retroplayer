(* sbctl.pas -- Pascal declarations for sbctl.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

{$A+,B-,D+,E-,F-,G+,I-,L+,N-,O-,P-,Q-,R-,S-,T-,V-,X+,Y+}
{$M 16384,0,655360}

unit sbctl;

interface

(*$I defines.pas*)

type
    PSBDEV = Pointer;

type
    TSBMODEL = Byte;

const
    SBMODEL_UNKNOWN = 0;
    SBMODEL_SB1     = 1;
    SBMODEL_SB2     = 2;
    SBMODEL_SBPRO   = 3;
    SBMODEL_SB16    = 4;

type
    TSBCFGFLAGS = Byte;

const
    SBCFGFL_TYPE     = 1 shl 0;
    SBCFGFL_BASE     = 1 shl 1;
    SBCFGFL_IRQ      = 1 shl 2;
    SBCFGFL_DMA8     = 1 shl 3;
    SBCFGFL_DMA16    = 1 shl 4;

const
    SBDEV_REF_FIXME = nil;

function  sb_get_name(self: PSBDEV): PChar;
function  sb_get_sample_bits(self: PSBDEV): Byte;
function  sb_is_sample_signed(self: PSBDEV): Boolean;
function  sb_get_channels(self: PSBDEV): Byte;
function  sb_get_rate(self: PSBDEV): Word;

procedure sb_hook_IRQ(self: PSBDEV; p: Pointer);
procedure sb_unhook_IRQ(self: PSBDEV);
procedure sb_set_volume(self: PSBDEV; value: Byte);
procedure sb_set_transfer_buffer(self: PSBDEV; buffer: Pointer; frame_size, frames_count: Word; autoinit: Boolean);
procedure sb_adjust_transfer_mode(self: PSBDEV; var m_rate: Word; var m_channels, m_bits: Byte; var f_sign: Boolean);
procedure sb_set_transfer_mode(self: PSBDEV; m_rate: Word; m_channels, m_bits: Byte; f_sign: Boolean);
function  sb_transfer_start(self: PSBDEV): Boolean;
function  sb_get_DMA_counter(self: PSBDEV): word;
procedure sb_transfer_pause(self: PSBDEV);
procedure sb_transfer_continue(self: PSBDEV);
procedure sb_transfer_stop(self: PSBDEV);

procedure sb_conf_manual(self: PSBDEV; flags: TSBCFGFLAGS; model: TSBMODEL; base: Word; irq, dma8, dma16: Byte);
function  sb_conf_detect(self: PSBDEV): Boolean;
function  sb_conf_env(self: PSBDEV): Boolean;
function  sb_conf_input(self: PSBDEV): Boolean;
procedure sb_conf_dump(self: PSBDEV);

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
    errno_,
    dos,
    debug,
    hwowner,
    dma,
    pic,
    sndctl_t,
    sbio,
    crt;

(*$l sbctl.obj*)

function  sb_get_name(self: PSBDEV): PChar; external;
function  sb_get_sample_bits(self: PSBDEV): Byte; external;
function  sb_is_sample_signed(self: PSBDEV): Boolean; external;
function  sb_get_channels(self: PSBDEV): Byte; external;
function  sb_get_rate(self: PSBDEV): Word; external;

procedure sb_hook_IRQ(self: PSBDEV; p: Pointer); external;
procedure sb_unhook_IRQ(self: PSBDEV); external;
procedure sb_set_volume(self: PSBDEV; value: Byte); external;
procedure sb_set_transfer_buffer(self: PSBDEV; buffer: Pointer; frame_size, frames_count: Word; autoinit: Boolean); external;
procedure sb_adjust_transfer_mode(self: PSBDEV; var m_rate: Word; var m_channels, m_bits: Byte; var f_sign: Boolean); external;
procedure sb_set_transfer_mode(self: PSBDEV; m_rate: Word; m_channels, m_bits: Byte; f_sign: Boolean); external;
function  sb_transfer_start(self: PSBDEV): Boolean; external;
function  sb_get_DMA_counter(self: PSBDEV): word; external;
procedure sb_transfer_pause(self: PSBDEV); external;
procedure sb_transfer_continue(self: PSBDEV); external;
procedure sb_transfer_stop(self: PSBDEV); external;

procedure sb_conf_manual(self: PSBDEV; flags: TSBCFGFLAGS; model: TSBMODEL; base: Word; irq, dma8, dma16: Byte); external;
function  sb_conf_detect(self: PSBDEV): Boolean; external;
function  sb_conf_env(self: PSBDEV): Boolean; external;
function  sb_conf_input(self: PSBDEV): Boolean; external;
procedure sb_conf_dump(self: PSBDEV); external;

procedure register_sbctl; far; external;

begin
  register_sbctl;
end.
