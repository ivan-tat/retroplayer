(* pic.pas -- declarations for pic.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit pic;

interface

(*$I defines.pas*)

procedure pic_get_hooked_irq_channels;
procedure pic_get_irq_owner;
procedure pic_get_irq_handler;
procedure pic_get_irq_data;

procedure hwowner_enable_irq;
procedure hwowner_enable_irq_channels;
procedure hwowner_disable_irq;
procedure hwowner_disable_irq_channels;
procedure hwowner_eoi;
procedure hwowner_hook_irq;
procedure hwowner_hook_irq_channels;
procedure hwowner_set_irq_handler;
procedure hwowner_set_irq_channels_handler;
procedure hwowner_clear_irq_handler;
procedure hwowner_clear_irq_channels_handler;
procedure hwowner_release_irq;
procedure hwowner_release_irq_channels;

implementation

uses
    pascal,
    dos_,
    stdio,
    stdlib,
    debug,
    hwowner;

(*$l isr.obj*)

procedure _isr_get; external;

(*$l pic.obj*)

procedure pic_get_hooked_irq_channels; external;
procedure pic_get_irq_owner; external;
procedure pic_get_irq_handler; external;
procedure pic_get_irq_data; external;

procedure hwowner_enable_irq; external;
procedure hwowner_enable_irq_channels; external;
procedure hwowner_disable_irq; external;
procedure hwowner_disable_irq_channels; external;
procedure hwowner_eoi; external;
procedure hwowner_hook_irq; external;
procedure hwowner_hook_irq_channels; external;
procedure hwowner_set_irq_handler; external;
procedure hwowner_set_irq_channels_handler; external;
procedure hwowner_clear_irq_handler; external;
procedure hwowner_clear_irq_channels_handler; external;
procedure hwowner_release_irq; external;
procedure hwowner_release_irq_channels; external;

procedure register_pic; far; external;

begin
    register_pic;
end.
