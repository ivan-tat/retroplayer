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

function  sb_get_name: PChar;
function  sb_get_sample_bits: Byte;
function  sb_is_sample_signed: Boolean;
function  sb_get_channels: Byte;
function  sb_get_rate: Word;

procedure sb_hook_IRQ(p: Pointer);
procedure sb_unhook_IRQ;
procedure sb_set_volume(value: Byte);
procedure sb_set_transfer_buffer(buffer: Pointer; frame_size, frames_count: Word; autoinit: Boolean);
procedure sb_adjust_transfer_mode(var m_rate: Word; var m_channels, m_bits: Byte; var f_sign: Boolean);
procedure sb_set_transfer_mode(m_rate: Word; m_channels, m_bits: Byte; f_sign: Boolean);
function  sb_transfer_start: Boolean;
function  sb_get_DMA_counter: word;
procedure sb_transfer_pause;
procedure sb_transfer_continue;
procedure sb_transfer_stop;

procedure sb_conf_manual(flags: TSBCFGFLAGS; model: TSBMODEL; base: Word; irq, dma8, dma16: Byte);
function  sb_conf_detect: Boolean;
function  sb_conf_env: Boolean;
function  sb_conf_input: Boolean;
procedure sb_conf_dump;

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
    dma,
    pic,
    sbio,
    detisr,
    sndisr,
    crt;

(*$l sbctl.obj*)

function  sb_get_name: PChar; external;
function  sb_get_sample_bits: Byte; external;
function  sb_is_sample_signed: Boolean; external;
function  sb_get_channels: Byte; external;
function  sb_get_rate: Word; external;

procedure sb_hook_IRQ(p: Pointer); external;
procedure sb_unhook_IRQ; external;
procedure sb_set_volume(value: Byte); external;
procedure sb_set_transfer_buffer(buffer: Pointer; frame_size, frames_count: Word; autoinit: Boolean); external;
procedure sb_adjust_transfer_mode(var m_rate: Word; var m_channels, m_bits: Byte; var f_sign: Boolean); external;
procedure sb_set_transfer_mode(m_rate: Word; m_channels, m_bits: Byte; f_sign: Boolean); external;
function  sb_transfer_start: Boolean; external;
function  sb_get_DMA_counter: word; external;
procedure sb_transfer_pause; external;
procedure sb_transfer_continue; external;
procedure sb_transfer_stop; external;

procedure sb_conf_manual(flags: TSBCFGFLAGS; model: TSBMODEL; base: Word; irq, dma8, dma16: Byte); external;
function  sb_conf_detect: Boolean; external;
function  sb_conf_env: Boolean; external;
function  sb_conf_input: Boolean; external;
procedure sb_conf_dump; external;

procedure register_sbctl; far; external;

begin
  register_sbctl;
end.
