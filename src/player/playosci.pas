{$M 16000,0,1000}
program example_for_s3mplay;

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
    s3mplay;

(*$I defines.pas*)

(*$L playosci.obj*)
procedure playosci_main; far; external;

begin
    playosci_main;
end.
