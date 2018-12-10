{$M 16000,0,1000}
program example_for_s3mplay;

uses
    watcom,
    types,
    pascal,
    startup,
    strutils,
    conio,
    stdio,
    stdlib,
    string_,
    unistd,
    crt,
    dos,
    dos_,
    ems,
    malloc,
    errno_,
    debug,
    sbctl,
    vga,
    s3mtypes,
    musins,
    muspat,
    s3mvars,
    fillvars,
    mixer,
    mixchn,
    effects,
    s3mplay;

(*$I defines.pas*)

(*$L smalls3m.obj*)

procedure smalls3m_main; far; external;

begin
    _cc_startup;
    smalls3m_main;
end.
