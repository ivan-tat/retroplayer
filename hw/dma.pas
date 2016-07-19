(* Declarations for DMA.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

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

procedure dmaMaskMulti( mask: byte );
procedure dmaMask( ch: byte );
procedure dmaEnableMulti( mask: byte );
procedure dmaEnable( ch: byte );
function dmaGetLinearAddress( p: pointer ): longint;
procedure dmaSetup( ch: byte; mode: TDMAMode; p: pointer; count: word );
function dmaGetCounter( ch: byte ): word;

implementation

(*$l dma.obj*)

procedure dmaMaskMulti( mask: byte ); external;
procedure dmaMask( ch: byte ); external;
procedure dmaEnableMulti( mask: byte ); external;
procedure dmaEnable( ch: byte ); external;
function dmaGetLinearAddress( p: pointer ): longint; external;
procedure dmaSetup( ch: byte; mode: TDMAMode; p: pointer; count: word ); external;
function dmaGetCounter( ch: byte ): word; external;

end.
