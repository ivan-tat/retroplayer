{$M 16000,0,2000}
program example_for_s3mplay;

uses
    types,
    strutils,
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
    s3mplay;

{$I defines.pas}

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
    screen_no:byte;  { current info on screen }
    startchn:byte;

procedure display_errormsg;
var
    s: PChar;
begin
    case player_error of
    noerror:
        s := 'No error';
    notenoughmem:
        s := 'Not enough memory';
    Allreadyallocbuffers:
        s := 'Buffers already allocated';
    nota386orhigher:
        s := 'Need a 386 or higher CPU';
    nosounddevice:
        s := 'No sound device is set';
    noS3Minmemory:
        s := 'No music module is loaded';
    internal_failure:
        s := 'Internal failure';
    E_failed_to_load_file:
        s := 'Failed to load file';
    else
        s := 'Unknown error';
    end;

    if (player_error_msg <> nil) then
        writeln('Error: ', s, ' (', player_error_msg, ')')
    else
        writeln('Error: ', s);

    writeln('PROGRAM HALTED.'#7);
    halt(1);
end;

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
    chn := @channel[index];
    mixchn_set_mixing(chn, not mixchn_is_mixing(chn));
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

var oldexit:pointer;

procedure local_exit; far;
  begin
    exitproc:=oldexit;
  end;

function nextord(nr:byte):byte;
  begin
    playState_patDelayCount:=0;playState_patLoopActive:=false;playState_patLoopCount:=0;playState_patLoopStartRow:=0;
    inc(nr);
    while (nr<=lastorder) and (order[nr]>=254) do inc(nr);
    if nr>lastorder then
      if playOption_LoopSong then
        begin
          nr:=0;
          while (nr<=lastorder) and (order[nr]>=254) do inc(nr);
          if nr>lastorder then playState_songEnded:=true; { stupid order ! (no real entry) }
        end
      else begin nr:=0;playState_songEnded:=true end;
    nextord:=nr;
  end;

procedure disable_all;
var i:byte;
  begin
    for i:=0 to usedchannels-1 do
      mixchn_set_flags(@channel[i], 0);
  end;

function prevorder(nr:byte):byte;
  begin
    if nr=0 then begin prevorder:=nr;exit end;
    dec(nr);
    while (nr>0) and (order[nr]>=254) do dec(nr);
    if order[nr]>=254 then { to far - search next playable }
      begin
        while (nr<=lastorder) and (order[nr]>=254) do inc(nr);
        if nr>lastorder then playState_songEnded:=true; { stupid order ! (no real entry) }
      end;
    prevorder:=nr;
  end;

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

procedure display_keys; far; external;
procedure display_help; far; external;
procedure display_playercfg; far; external;

procedure display_helpscreen;
  begin
    textcolor(white);textbackground(blue);
    window(1,8,80,25);clrscr;
    writeln;
    display_keys;
    window(1,1,80,25);
  end;

function getFreeDOSMemory: longint;
begin
    getFreeDOSMemory := _memmax;
end;

function getFreeEMMMemory: longint;
begin
    if emsInstalled then
        getFreeEMMMemory := emsGetFreePagesCount*16
    else
        getFreeEMMMemory := 0;
end;

procedure mainscreen;
CONST SW_order:array[false..true] of string = ('Extended Order','Normal Order');
      SW_stereo:array[false..true] of string = ('Mono','Stereo');
      SW_qual:array[false..true] of string = ('Hiquality','Lowquality');
      sw_res:array[false..true] of string = ('8bit','16bit');
  begin
    textbackground(blue);window(1,1,80,25);clrscr;
    gotoxy(1,7);
    textbackground(yellow);
    clreol;
    writeln('Ch Type Flags Inst Note  Period Step  Vol Effect');
    textbackground(white);textcolor(black);
    gotoxy(1,1);clreol;write('Order:   (  ) Row:    Tick:                  that is Pattern:    ');
    textbackground(green);textcolor(black);gotoxy(1,6);clreol;write(' Title: ',mod_Title);
    gotoxy(50,6);write('EMS usage: ',switch[useEMS],' Loop S3M : ');
    textbackground(blue);textcolor(lightgray);
    gotoxy(1,3);write(' Samplerate: ',playGetSampleRate:5,'  ',sw_stereo[stereo],', ',sw_res[_16bit],
    ', ',sw_order[ST3order],', ',sw_qual[player_is_lq_mode]);
    gotoxy(1,4);write(' Free DOS memory : ',(getFreeDOSMemory shr 10):6,' KiB   Free EMS memory : ',getFreeEMMMemory:5,' KiB');
    gotoxy(1,5);
    write(' Used EMS memory : ', (musinsl_get_used_EM(mod_Instruments) + muspatl_get_used_EM(mod_Patterns)):6,
        ' KiB   <F1> - Help screen          Version : ', PLAYER_VERSION);
  end;

procedure refr_mainscr;
var
    pat: PMUSPAT;
  begin
    pat := muspatl_get(mod_Patterns, playState_pattern);
    textbackground(white);textcolor(black);
    gotoxy(8,1);write(playState_order:2);
    gotoxy(11,1);write(lastorder:2);
    gotoxy(20,1);write(playState_row:2);
    gotoxy(29,1);write(playState_tick:2);
    gotoxy(63,1);write(playState_pattern:2,' (',hexw( seg( muspat_get_data(pat)^ ) ),')');
    textbackground(green);textcolor(black);
    gotoxy(76,6);write(switch[playOption_LoopSong]);
    gotoxy(1,2);
    textbackground(magenta);textcolor(yellow);
    write(' Speed: ', playGetSpeed:3,
        ' '#179' Tempo: ', playGetTempo:3,
        ' '#179' GVol: ', playState_gVolume:2,
        ' '#179' MVol: ', playGetMasterVolume:3,
        ' '#179' Pdelay: ', playGetPatternDelay:2,
        ' '#179' Ploop: ', playState_patLoopStartRow);
    if playState_patLoopActive then
        write('(',playState_patLoopCount,')');
    clreol;
  end;

{$I PREPARE.INC}  { prepare the different screens }
{$I REFRESH.INC}  { refresh the different screens }

var i:byte;

begin
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
  textbackground(black);textcolor(lightgray);
  oldexit:=exitproc;
  exitproc:=@local_exit;
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
      display_errormsg;
  {$IFDEF DEBUGLOAD}
  writeln('Free DOS memory after loading: ',getFreeDOSMemory shr 10, ' KiB');
  writeln('Free EMM memory after loading: ',getFreeEMMMemory, ' KiB');
  {$ENDIF}
  writeln(' ''',mod_Title,''' loaded ... (was saved with ',mod_TrackerName,')');
  if not player_init then display_errormsg;
  if not player_init_device(how2input) then begin writeln(' SoundBlaster not found sorry ... ');halt end;
  if disply_c then
    begin
      display_playercfg;
      write(#13#10' press a key to continue...');readkey;clrscr;gotoxy(1,19);
    end;
  { And here we go :) }
  if volume>0 then playSetMasterVolume(volume);
  if (not player_set_mode(_16bit,stereo,samplerate,_LQ)) then display_errormsg;
  playSetOrder(ST3order);
  save_chntyps;
  playOption_LoopSong:=true;
  screen_no:=1;startchn:=1;
  if (not playStart) then display_errormsg;
  mainscreen;
  hide_cursor;
  repeat
    c:=#0;
    refr_mainscr;
    refresh_scr;
    if keypressed then c:=readkey;
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
        screen_no:=ord(c)-59;
        prepare_scr;c:=#0;
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
          playState_order:=nextord(playState_order);
          lastrow:=0;
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
    if upcase(c)='L' then playOption_LoopSong:=not playOption_LoopSong;
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
        mainscreen;
      end;
    if (c=#77) and (startchn<usedchannels) then begin inc(startchn);if screen_no=2 then prepare_scr; end;
    if (c=#75) and (startchn>1) then begin dec(startchn);if screen_no=2 then prepare_scr; end;
  until ((sndDMABuf.flags and SNDDMABUFFL_SLOW <> 0) or (c=#27) or playState_songEnded);
  if (sndDMABuf.flags and SNDDMABUFFL_SLOW <> 0) then
        writeln(' Sorry your PC is to slow ... ');
  view_cursor;
  player_free;
  gotoxy(1,8);
  textcolor(white);textbackground(blue);
  {$IFDEF DEBUGLOAD}
  writeln('Free DOS memory after all: ',getFreeDOSMemory shr 10, ' KiB');clreol;
  writeln('Free EMM memory after all: ',getFreeEMMMemory, ' KiB');clreol;
  {$ENDIF}
end.
