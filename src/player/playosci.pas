{$M 16000,0,1000}
program example_for_s3mplay;

uses
    pascal,
    startup,
    crt,
    dos,
    string_,
    stdio,
    stdlib,
    conio,
    sbctl,
    vga,
    s3mvars,
    fillvars,
    mixchn,
    s3mplay;

(*$I defines.pas*)

(*$L playosci.obj*)
procedure playosci_main; far; external;

begin
    playosci_main;
end.
