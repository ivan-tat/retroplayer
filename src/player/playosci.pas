program playosci;

uses
    hexdigts,
    pascal,
    startup,
    dos,
    ems,
    string_,
    stdio,
    stdlib,
    i86,
    conio,
    debug,
    hwowner,
    pic,
    dma,
    vbios,
    vga,
    sbctl,
    musmod,
    mixchn,
    musmodps,
    fillvars,
    musplay;

(*$I defines.pas*)

(*$L playosci.obj*)
procedure playosci_main; far; external;

begin
    playosci_main;
end.
