program plays3m;

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
    dynarray,
    unistd,
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
    musdefs,
    musins,
    muspat,
    musmod,
    mixchn,
    musmodps,
    effects,
    mixer,
    fillvars,
    musplay,
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
    plays3m_main;
end.
