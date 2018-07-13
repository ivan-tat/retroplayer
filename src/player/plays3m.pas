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
    unistd,
    crt,
    dos,
    dos_,
    ems,
    malloc,
    errno_,
    debug,
    sbctl,
    s3mtypes,
    musins,
    muspat,
    s3mvars,
    fillvars,
    mixer,
    mixchn,
    effects,
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

var
    chnlastinst: array[0..MAX_CHANNELS-1] of Byte;
    lastrow: Byte;
    startchn: Byte;

procedure plays3m_main; far; external;

begin
    plays3m_main;
end.
