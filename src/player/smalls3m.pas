program smalls3m;

uses
    hexdigts,
    watcom,
    types,
    pascal,
    startup,
    strutils,
    i86,
    conio,
    stdio,
    stdlib,
    string_,
    unistd,
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
    smalls3m_main;
end.
