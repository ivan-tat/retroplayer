{$M 16000,0,2000}
program example_for_s3mplay;

uses
    types,
    strutils,
    conio,
    stdio,
    string_,
    crt,
    dos,
    dos_,
    ems,
    malloc,
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
    screen;

{$I defines.pas}

const
    __FILE__ = 'plays3m.pas';

{$L w_inf.obj}
{$L w_hlp.obj}
{$L w_chn.obj}
{$L w_pat.obj}
{$L w_ins.obj}
{$L w_smp.obj}
{$L w_dbg.obj}
{$L plays3m.obj}

const
    DEVSEL_AUTO = 1;
    DEVSEL_ENV = 2;
    DEVSEL_MANUAL = 3;

var
    opt_help: Boolean;
    opt_filename: String;
    opt_filename_c: array [0..255] of Char;
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
    i: Byte;
    quit: Boolean;
    c: Char;
    savchn: array [0..MAX_CHANNELS-1] of Byte;
    chnlastinst: array[0..MAX_CHANNELS-1] of Byte;

function getFreeDOSMemory: longint; far; external;
function getFreeEMMMemory: longint; far; external;
procedure memstats; far; external;
procedure display_errormsg; far; external;
procedure display_help; far; external;
procedure display_playercfg; far; external;
function prevorder(nr: Byte): Byte; external;
function nextord(nr: Byte): Byte; external;
procedure write_Note(note: Byte); external;

procedure vbios_set_mode(mode: Byte); assembler;
asm
    mov ah,00h
    mov al,[mode]
    int 10h
end;

procedure vbios_set_cursor_shape(start, stop: Byte); assembler;
asm
    mov ah,01h
    mov cl,[stop]
    mov ch,[start]
    int 10h
end;

procedure channels_save_all;
var
    i: Byte;
    chn: PMIXCHN;
begin
    for i := 0 to MAX_CHANNELS-1 do
    begin
        chn := @Channel[i];
        savchn[i] := mixchn_get_type(chn);
    end;
end;

procedure channels_swap(index: Byte);
var
    chn: PMIXCHN;
begin
    chn := @Channel[index];

    if (mixchn_get_type(chn) = 0) then
        mixchn_set_type(chn, savchn[index])
    else
        mixchn_set_type(chn, 0);
end;

procedure channels_toggle_mixing(index: Byte);
var
    chn: PMIXCHN;
begin
    chn := @Channel[index];
    mixchn_set_mixing(chn, not mixchn_is_mixing(chn));
end;

procedure channels_stop_all;
var
    i: Byte;
    chn: PMIXCHN;
begin
    for i := 0 to UsedChannels - 1 do
    begin
        chn := @Channel[i];
        mixchn_set_flags(chn, mixchn_get_flags(chn) and not MIXCHNFL_PLAYING);
    end;
end;

procedure cursor_hide;
begin
    vbios_set_cursor_shape(32, 32);
end;

procedure cursor_show;
begin
    vbios_set_cursor_shape(15, 16);
end;

procedure check_para(p:string);
var
    pu: string;
    t: string;
    b: byte;
    w: word;
    i: integer;
begin
    pu := upstr(p);

    if (pu[1]<>'-') and (pu[1]<>'/') then
    begin
        opt_filename := p;
        exit;
    end;

    if (pu[2] = 'V') then
    begin
        t := copy(pu, 3, length(pu) - 2);
        val(t, b, i);
        if (i = 0) then
            opt_mvolume := b;
    end;

    if (pu[2] = 'S') then
    begin
        t := copy(pu, 3, length(pu) - 2);
        val(t, w, i);
        if (i = 0) then
        begin
            if (w < 100) then
                w := w * 1000;
            opt_mode_rate := w;
        end;
    end;
    if ((pu[2] = 'H') or (pu[2] = '?')) then
        opt_help := true;

    if (pu[2] = 'M') then
        opt_mode_stereo := false;

    if (pu[2] = '8') then
        opt_mode_16bits := false;

    if (pu[2] = 'C') then
        opt_dumpconf := true;

    if (pu[2] = 'O') then
        opt_st3order := true;

    if (copy(pu, 2, 5) = 'NOEMS') then
        opt_em := false;

    if (copy(pu, 2, 3) = 'ENV') then
        opt_devselect := DEVSEL_ENV;

    if (copy(pu, 2, 3) = 'CFG') then
        opt_devselect := DEVSEL_MANUAL;

    if (copy(pu, 2, 2) = 'LQ') then
        opt_mode_lq := true;

    if (pu[2] = 'B') then
    begin
        t := copy(pu, 3, length(pu));
        val(t, b, i);
        if (i = 0) then
            opt_startpos := b;
    end;

    if (pu[2] = 'F') then
    begin
        t := copy(pu, 3, length(pu) - 2);
        val(t, b, i);
        if (i = 0) then
            playOption_FPS := b;
    end;
end;

procedure desktop_clear;
begin
    window(1, 1, scrWidth, scrHeight);
    textbackground(black);
    clrscr;
end;

(* Information windows *)

(* Pattern window *)

var
    lastrow: Byte;
    startchn: Byte;

(* Window's event router *)

function  winlist_init: Boolean; external;
procedure winlist_select(value: Word); external;
procedure winlist_show_selected; external;
procedure winlist_hide_selected; external;
procedure winlist_show_all; external;
procedure winlist_hide_all; external;
procedure winlist_refresh_all; external;
function  winlist_keypress(c: Char): Boolean; external;
procedure winlist_free; external;

(*** Initialization ***)

procedure register_plays3m; far; external;

begin
    register_plays3m;

    writeln('Simple music player for DOS, version ', PLAYER_VERSION, '.');
    writeln('Originally written by Andre Baresel, 1994-1995.');
    writeln('Modified by Ivan Tatarinov <ivan-tat@ya.ru>, 2016-2017.');
    writeln('This is free and unencumbered software released into the public domain.');
    writeln('For more information, please refer to <http://unlicense.org>.');

    opt_help := false;
    opt_filename := '';
    opt_devselect := DEVSEL_AUTO;
    opt_mode_rate := 45454;
    opt_mode_stereo := true;
    opt_mode_16bits := false;   { not done yet }
    opt_mode_lq := false;
    opt_dumpconf := false;
    opt_mvolume := 0;           { use volume from file }
    opt_st3order := false;
    opt_startpos := 0;
    opt_loop := false;
    opt_em := true;
    opt_fps := playOption_FPS;

    for i := 1 to paramcount do
        check_para(paramstr(i));

    if (length(opt_filename) = 0) then
    begin
        display_help;
        halt(1);
    end;

    UseEMS := opt_em;

    {$IFDEF DEBUGLOAD}
    writeln('Before loading:');
    memstats;
    {$ENDIF}

    memcpy(opt_filename_c, opt_filename[1], Ord(opt_filename[0]));
    opt_filename_c[Ord(opt_filename[0])] := Chr(0);
    if (not player_load_s3m(opt_filename_c)) then
    begin
        display_errormsg;
        halt(1);
    end;

    {$IFDEF DEBUGLOAD}
    writeln('After loading:');
    memstats;
    {$ENDIF}

    writeln('Song "',mod_Title,'" loaded (saved with ', mod_TrackerName, ')');

    if (not player_init) then
    begin
        display_errormsg;
        halt(1);
    end;

    if (not player_init_device(opt_devselect)) then
    begin
        writeln('No sound device found.');
        halt(1);
    end;

    if (opt_dumpconf) then
    begin
        display_playercfg;
        write('Press a key to continue...');
        readkey;
        writeln;
    end;

    if (opt_mvolume > 0) then
        playSetMasterVolume(opt_mvolume);

    if (not player_set_mode(opt_mode_16bits, opt_mode_stereo, opt_mode_rate, opt_mode_lq)) then
    begin
        display_errormsg;
        halt(1);
    end;

    playSetOrder(opt_st3order);
    initState_startOrder := opt_startpos;
    playOption_LoopSong := opt_loop;
    playOption_FPS := opt_fps;

    channels_save_all;

    if (not winlist_init) then
    begin
        DEBUG_FAIL(__FILE__, 'main', 'Failed to initialize information windows.');
        halt(1);
    end;

    if (not playStart) then
    begin
        display_errormsg;
        halt(1);
    end;

    cursor_hide;
    desktop_clear;
    winlist_select(0);
    winlist_show_all;

    quit := false;
    repeat
        winlist_refresh_all;

        if (keypressed) then
        begin
            c := readkey;

            if (not winlist_keypress(c)) then
            begin
                if (c=#27) then
                begin
                    quit := true;
                    c := #0;
                end;
                {if c<>#0 then write(ord(c));}
                if (c>='x') and (c<=chr(ord('x')+16)) then
                begin
                    channels_toggle_mixing(ord(c)-ord('x'));
                    c:=#0;
                end;
                if (ord(c)>=16) and (ord(c)<=19) then
                begin
                    channels_toggle_mixing(ord(c)-4);
                    c:=#0;
                end;
                (* F1-F6 *)
                if (c>=#59) and (c<=#64)  then
                  begin
                    winlist_hide_selected;
                    winlist_select(Ord(c) - 59);
                    winlist_show_selected;
                    c:=#0;
                  end;
                if (upcase(c)='P') then
                  begin
                    pause_play;
                    readkey;
                    continue_play;
                    c:=#0;
                  end;
                if (c='+') then
                    begin
                      lastrow:=0;
                      playState_order:=nextord(playState_order);
                      playState_row:=0;
                      playState_tick:=1;
                      playState_pattern:=order[playState_order];
                      c:=#0
                    end;
                if (c='-') then
                    begin
                      playState_order:=prevorder(playState_order);
                      playState_patDelayCount:=0;
                      playState_patLoopActive:=false;
                      playState_patLoopCount:=0;
                      playState_patLoopStartRow:=0;
                      channels_stop_all;
                      lastrow:=0;
                      playState_row:=0;
                      playState_tick:=1;
                      playState_pattern:=order[playState_order];
                      c:=#0
                    end;
                if upcase(c)='L' then
                begin
                    playOption_LoopSong:=not playOption_LoopSong;
                    c:=#0;
                end;
                if upcase(c)='D' then
                  begin
                    vbios_set_mode(3); { clear screen }
                    writeln(' Return to player with ''EXIT'' ... ');
                    swapvectors;
                    exec(getenv('COMSPEC'),'');
                    swapvectors;
                    c:=#0;
                    vbios_set_mode(3);
                    cursor_hide;
                    if doserror<>0 then
                      begin
                        while keypressed do readkey;
                        writeln(' Doserror ',doserror);
                        writeln(' Hmm somethings going wrong with running a copy of COMMAND.COM ...');
                        writeln(' press any key to continue ... ');
                        readkey;
                      end;
                    desktop_clear;
                    winlist_show_all;
                  end;
            end;
        end;
    until ((sndDMABuf.flags and SNDDMABUFFL_SLOW <> 0) or quit or playState_songEnded);

    textbackground(black);
    textcolor(lightgray);
    clrscr;
    cursor_show;

    {$IFDEF DEBUG}
    if (sndDMABuf.flags and SNDDMABUFFL_SLOW <> 0) then
        DEBUG_FAIL(__FILE__, 'main', 'PC is too slow');
    {$ENDIF}

    player_free;

    {$IFDEF DEBUG}
    writeln('After all:');
    memstats;
    {$ENDIF}
end.
