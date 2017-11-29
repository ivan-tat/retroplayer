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

{
const
    DEVSEL_AUTO = 1;
    DEVSEL_ENV = 2;
    DEVSEL_MANUAL = 3;
}

var
{
    opt_help: Boolean;
    opt_filename: array [0..255] of Char;
    opt_devselect: Byte;
    opt_mode_rate: Word;
    opt_mode_stereo: Boolean;
    opt_mode_16bits: Boolean;
    opt_mode_lq: Boolean;
    opt_dumpconf: Boolean;
    opt_mvolume: Byte;
    opt_st3order: Boolean;
    opt_startpos: Byte;
    opt_loop: Boolean;
    opt_em: Boolean;
    opt_fps: Byte;
}
    chnlastinst: array[0..MAX_CHANNELS-1] of Byte;
    lastrow: Byte;
    startchn: Byte;

procedure plays3m_main; near; external;

begin
    plays3m_main;
end.
