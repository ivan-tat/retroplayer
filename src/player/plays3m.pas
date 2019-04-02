{$M 16000,0,2000}
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
    dynarray,
    unistd,
    crt,
    dos,
    dos_,
    ems,
    malloc,
    errno_,
    debug,
    common,
    hwowner,
    pic,
    dma,
    sbctl,
    sndctl_t,
    s3mtypes,
    musins,
    muspat,
    musmod,
    mixchn,
    musmodps,
    effects,
    mixer,
    fillvars,
    s3mplay,
    vbios,
    vga,
    screen;

{$I defines.pas}

{$L w_inf.obj}
{$L w_hlp.obj}
{$L w_chn.obj}
{$L w_pat.obj}
{$L w_ins.obj}
{$L w_smp.obj}
{$L w_dbg.obj}
{$L plays3m.obj}

procedure plays3m_main; far; external;

begin
    _cc_startup;
    plays3m_main;
end.
