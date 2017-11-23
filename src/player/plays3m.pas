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

const stereo_calc=true;
      _16bit_calc=false;
      switch:array[false..true] of string[3] = ('off','on ');

var samplerate:word;
    Stereo:Boolean;
    _16bit:Boolean;
    _LQ:boolean;
    ST3order:Boolean;
    opt_help:boolean;
    volume:byte;
    how2input:byte; { 1-autodetect,2-read blaster enviroment,3-input by hand }
    disply_c:boolean;

function getFreeDOSMemory: longint; far; external;
function getFreeEMMMemory: longint; far; external;

procedure display_errormsg; far; external;

var filename:string;
    name: array [0..255] of Char;
    c:char;
    savchn:array[0..15] of byte;

procedure save_chntyps;
var i:byte;
begin
    for i:=0 to 15 do
        savchn[i] := mixchn_get_type(@channel[i]);
end;

procedure revers(n:byte);
var
    chn: PMIXCHN;
begin
    chn := @channel[n];

    if (mixchn_get_type(chn) = 0) then
        mixchn_set_type(chn, savchn[n])
    else
        mixchn_set_type(chn, 0);
end;

procedure toggle_channel_mixing(index: Byte);
var
    chn: PMIXCHN;
begin
    chn := @Channel[index];
    mixchn_set_mixing(chn, not mixchn_is_mixing(chn));
end;

procedure disable_all;
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

procedure hide_cursor; assembler;
 asm
   mov  ah,01
   mov  cx,32*256+32
   int  10h
 end;

procedure view_cursor; assembler;
 asm
   mov  ah,01
   mov  cx,15*256+16
   int  10h
 end;

function prevorder(nr: Byte): Byte; external;
function nextord(nr: Byte): Byte; external;

procedure check_para(p:string);
var t:string;
    b:byte;
    w:word;
    i:integer;
  begin
    if (p[1]<>'-') and (p[1]<>'/') then
      begin
        filename:=p;
        exit;
      end;
    if upcase(p[2])='V' then { Volume }
      begin
        t:=copy(p,3,length(p)-2);
        val(t,b,i);
        if i=0 then volume:=b;
      end;
    if upcase(p[2])='S' then { Samplerate }
      begin
        t:=copy(p,3,length(p)-2);
        val(t,w,i);
        if i=0 then
          begin
            if w<100 then w:=w*1000;
            SampleRate:=w;
          end;
      end;
    if (upcase(p[2])='H') or (p[2]='?') then { help } opt_help:=true;
    if upcase(p[2])='M' then { Mono - because default is stereo } stereo:=false;
    if p[2]='8' then { 8bit - default is 16bit } _16bit:=false;
    if upcase(p[2])='C' then { display SB config } disply_c:=true;
    if upcase(p[2])='O' then { use ST3 order } ST3order:=true;
    if upstr(copy(p,2,5))='NOEMS' then { don't use EMS } useEMS:=false;
    if upstr(copy(p,2,3))='ENV' then { read Blaster enviroment } how2input:=2;
    if upstr(copy(p,2,3))='CFG' then { input SB config by hand } how2input:=3;
    if upstr(copy(p,2,2))='LQ' then { mix in low quality mode } _LQ:=true;
    if upcase(p[2])='B' then
      begin
        t:=copy(p,3,length(p));
        val(t,b,i);
        if i=0 then initState_startOrder:=b;
      end;
    if upcase(p[2])='F' then { set frame rate }
      begin
        t:=copy(p,3,length(p)-2);
        val(t,b,i);
        if i=0 then playOption_FPS:=b;
      end;
  end;

procedure display_help; far; external;
procedure display_playercfg; far; external;

procedure desktop_clear;
begin
    window(1, 1, scrWidth, scrHeight);
    textbackground(black);
    clrscr;
end;

var chnlastinst:array[0..max_channels-1] of byte;

procedure write_Note(note: Byte); external;

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

var i:byte;
    quit: Boolean;
begin
    register_plays3m;

  writeln('S3M-PLAYER for SoundBlasters Version ',PLAYER_VERSION);
  writeln('Originally written by Cyder of Green Apple (Andre'' Baresel)');
  { setup defaults: }
  Samplerate:=45454;
  Stereo:=stereo_calc;
  _16bit:=_16bit_calc;
  _LQ:=false;
  opt_help:=false;
  volume:=0; { use volume given in S3M ... }
  how2input:=1; { autodetect SB }
  disply_c:=false;
  filename:='';
  ST3order:=false;
  initState_startOrder:=0;
  { end of default ... }
  for i:=1 to paramcount do
    check_para(paramstr(i));
    if (length(filename) = 0) then
    begin
        display_help;
        halt(1);
    end;
  writeln;
  {$IFDEF DEBUGLOAD}
  writeln('Free DOS memory before loading: ',getFreeDOSMemory shr 10, ' KiB');
  writeln('Free EMM memory before loading: ',getFreeEMMMemory, ' KiB');
  {$ENDIF}
  memcpy(name, filename[1], Ord(filename[0]));
  name[Ord(filename[0])] := Chr(0);
    if (not player_load_s3m(name)) then
    begin
        display_errormsg;
        halt(1);
    end;
  {$IFDEF DEBUGLOAD}
  writeln('Free DOS memory after loading: ',getFreeDOSMemory shr 10, ' KiB');
  writeln('Free EMM memory after loading: ',getFreeEMMMemory, ' KiB');
  {$ENDIF}
  writeln(' ''',mod_Title,''' loaded ... (was saved with ',mod_TrackerName,')');
    if (not player_init) then
    begin
        display_errormsg;
        halt(1);
    end;
  if not player_init_device(how2input) then begin writeln(' SoundBlaster not found sorry ... ');halt end;
  if disply_c then
    begin
      display_playercfg;
      write(#13#10' press a key to continue...');readkey;clrscr;gotoxy(1,19);
    end;
  { And here we go :) }
  if volume>0 then playSetMasterVolume(volume);
    if (not player_set_mode(_16bit,stereo,samplerate,_LQ)) then
    begin
        display_errormsg;
        halt(1);
    end;
  playSetOrder(ST3order);
  save_chntyps;
  playOption_LoopSong:=true;

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

    hide_cursor;
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
                    toggle_channel_mixing(ord(c)-ord('x'));
                    c:=#0;
                end;
                if (ord(c)>=16) and (ord(c)<=19) then
                begin
                    toggle_channel_mixing(ord(c)-4);
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
                      disable_all;
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
                    asm
                      mov ax,3
                      int 10h    { clear screen }
                    end;
                    writeln(' Return to player with ''EXIT'' ... ');
                    swapvectors;
                    exec(getenv('COMSPEC'),'');
                    swapvectors;
                    c:=#0;
                    asm
                      mov ax,3
                      int 10h
                    end;
                    hide_cursor;
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
  view_cursor;

  {$IFDEF DEBUG}
  if (sndDMABuf.flags and SNDDMABUFFL_SLOW <> 0) then
        DEBUG_FAIL(__FILE__, 'main', 'PC is too slow');
  {$ENDIF}

  player_free;

  {$IFDEF DEBUG}
  writeln('Free DOS memory after all: ',getFreeDOSMemory shr 10, ' KiB');clreol;
  writeln('Free EMM memory after all: ',getFreeEMMMemory, ' KiB');clreol;
  {$ENDIF}
end.
