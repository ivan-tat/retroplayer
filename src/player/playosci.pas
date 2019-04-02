{$M 16000,0,1000}
program example_for_s3mplay;

uses
    pascal,
    startup,
    crt,
    dos,
    ems,
    string_,
    stdio,
    stdlib,
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
    s3mplay;

(*$I defines.pas*)

(*$L playosci.obj*)
procedure playosci_main; far; external;

begin
    _cc_startup;
    playosci_main;
end.
