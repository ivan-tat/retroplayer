(* dma.pas -- declarations for dma.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit dma;

interface

type
    TDMAMode = byte;

const
    DMA_MODE_CHAN_MASK    = $03;
    DMA_MODE_TRAN_MASK    = $0c;
    DMA_MODE_TRAN_VERIFY  = $00;
    DMA_MODE_TRAN_WRITE   = $04;
    DMA_MODE_TRAN_READ    = $08;
    DMA_MODE_TRAN_ILLEGAL = $0c;
    DMA_MODE_INIT_MASK    = $10;
    DMA_MODE_INIT_SINGLE  = $00;
    DMA_MODE_INIT_AUTO    = $10;
    DMA_MODE_ADDR_MASK    = $20;
    DMA_MODE_ADDR_INCR    = $00;
    DMA_MODE_ADDR_DECR    = $20;
    DMA_MODE_MASK         = $c0;
    DMA_MODE_DEMAND       = $00;
    DMA_MODE_SINGLE       = $40;
    DMA_MODE_BLOCK        = $80;
    DMA_MODE_CASCADE      = $c0;

type
    TDMAMask = byte;

type
    PDMAOwner = pointer;

procedure dmaMaskSingleChannel(ch: byte);
procedure dmaMaskChannels(mask: TDMAMask);
procedure dmaEnableSingleChannel(ch: byte);
procedure dmaEnableChannels(mask: TDMAMask);
function  dmaGetLinearAddress(p: pointer): longint;
procedure dmaSetupSingleChannel(ch: byte; mode: TDMAMode; l: longint; count: word);
function  dmaGetCounter(ch: byte): word;

function  dmaIsAvailableSingleChannel(ch: byte): boolean;
function  dmaGetAvailableChannels: TDMAMask;
function  dmaGetSingleChannelOwner(ch: byte): PDMAOwner;
procedure dmaHookSingleChannel(ch: byte; owner: PDMAOwner);
procedure dmaHookChannels(mask: TDMAMask; owner: PDMAOwner);
procedure dmaReleaseSingleChannel(ch: byte);
procedure dmaReleaseChannels(mask: TDMAMask);

type
    TDMABUF = record
        data: pointer;
        size: longint;
        unaligned: pointer;
    end;
    PDMABUF = ^TDMABUF;

function  dmaBuf_new: PDMABUF;
procedure dmaBuf_delete(var buf: PDMABUF);
function  dmaBufAlloc(buf: PDMABUF; size: longint): boolean;
procedure dmaBufFree(buf: PDMABUF);

procedure dmaBufInit(buf: PDMABUF);
procedure dmaBufDone(buf: PDMABUF);

implementation

uses
    stdio,
    dosproc;

(*$l dma.obj*)

procedure dmaMaskSingleChannel(ch: byte); external;
procedure dmaMaskChannels(mask: TDMAMask); external;
procedure dmaEnableSingleChannel(ch: byte); external;
procedure dmaEnableChannels(mask: TDMAMask); external;
function  dmaGetLinearAddress(p: pointer): longint; external;
procedure dmaSetupSingleChannel(ch: byte; mode: TDMAMode; l: longint; count: word); external;
function  dmaGetCounter(ch: byte): word; external;

function  dmaIsAvailableSingleChannel(ch: byte): boolean; external;
function  dmaGetAvailableChannels: TDMAMask; external;
function  dmaGetSingleChannelOwner(ch: byte): PDMAOwner; external;
procedure dmaHookSingleChannel(ch: byte; owner: PDMAOwner); external;
procedure dmaHookChannels(mask: TDMAMask; owner: PDMAOwner); external;
procedure dmaReleaseSingleChannel(ch: byte); external;
procedure dmaReleaseChannels(mask: TDMAMask); external;

function  dmaBuf_new: PDMABUF; external;
procedure dmaBuf_delete(var buf: PDMABUF); external;
function  dmaBufAlloc(buf: PDMABUF; size: longint): boolean; external;
procedure dmaBufFree(buf: PDMABUF); external;

procedure dmaBufInit(buf: PDMABUF); external;
procedure dmaBufDone(buf: PDMABUF); external;

procedure register_dma; far; external;

begin
    register_dma;
end.
