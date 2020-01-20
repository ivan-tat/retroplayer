(* dma.pas -- declarations for dma.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit dma;

interface

(*$I defines.pas*)

procedure dma_get_linear_address;

procedure dma_get_hooked_channels;
procedure dma_get_owner;

procedure hwowner_hook_dma;
procedure hwowner_hook_dma_channels;
procedure hwowner_mask_dma;
procedure hwowner_mask_dma_channels;
procedure hwowner_enable_dma;
procedure hwowner_enable_dma_channels;
procedure hwowner_setup_dma_transfer;
procedure hwowner_get_dma_counter;
procedure hwowner_release_dma;
procedure hwowner_release_dma_channels;

procedure dmaBuf_init;
procedure dmaBuf_alloc;
procedure dmaBuf_free;

procedure register_dma;

implementation

uses
    dos_,
    stdio,
    stdlib,
    string_,
    debug,
    hwowner;

(*$l dma.obj*)

procedure dma_get_linear_address; external;

procedure dma_get_hooked_channels; external;
procedure dma_get_owner; external;

procedure hwowner_hook_dma; external;
procedure hwowner_hook_dma_channels; external;
procedure hwowner_mask_dma; external;
procedure hwowner_mask_dma_channels; external;
procedure hwowner_enable_dma; external;
procedure hwowner_enable_dma_channels; external;
procedure hwowner_setup_dma_transfer; external;
procedure hwowner_get_dma_counter; external;
procedure hwowner_release_dma; external;
procedure hwowner_release_dma_channels; external;

procedure dmaBuf_init; external;
procedure dmaBuf_alloc; external;
procedure dmaBuf_free; external;

procedure register_dma; external;

end.
