{$M 16000,0,1000}
program example_for_s3mplay;

uses
    hexdigts,
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
    debug,
    errno_,
    dos,
    dos_,
    ems,
    hwowner,
    pic,
    dma,
    malloc,
    sbctl,
    vga,
    s3mtypes,
    musins,
    muspat,
    musmod,
    mixchn,
    musmodps,
    effects,
    mixer,
    fillvars,
    s3mplay;

(*$I defines.pas*)

(*$L smalls3m.obj*)

procedure smalls3m_main; far; external;

begin
    _cc_startup;
    smalls3m_main;
end.
