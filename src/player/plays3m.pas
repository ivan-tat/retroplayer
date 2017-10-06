{$M 16000,0,2000}
program example_for_s3mplay;

uses
    types,
    strutils,
    crt,
    dos,
    dos_,
    ems,
    malloc,
    debug,
    sbctl,
    blaster,
    s3mtypes,
    muspat,
    s3mvars,
    fillvars,
    mixvars,
    mixchn,
    effects,
    s3mplay;

{$I defines.pas}

const stereo_calc=true;
      _16bit_calc=false;
      switch:array[false..true] of string[3] = ('off','on ');

var samplerate:word;
    Stereo:Boolean;
    _16bit:Boolean;
    _LQ:boolean;
    ST3order:Boolean;
    help:boolean;
    volume:byte;
    how2input:byte; { 1-autodetect,2-read blaster enviroment,3-input by hand }
    disply_c:boolean;
    screen_no:byte;  { current info on screen }
    startchn:byte;

procedure display_errormsg(err:integer);
  begin
    { I know case is stupid - like my code allways is :) }
    case err of
       0: write(' Hmm no error what''s wrong ? ');
      -1: begin
          if load_error=-1 then write(' Not enough memory for this module. ') else
          if player_error=-1 then write(' Not enough memory for internal buffers. ');
          write('PROGRAMMERS INFO: Try to lower Pascal heap or DMA buffer. ');
          end;
      -2: write(' Wrong file format. Not a S3M ? ');
      -3: write(' File corrupt. ');
      -4: write(' File does not exist. ');
      -7: write(' Need a 386 or higher. ');
      -8: write(' No sounddevice set. (wrong code - shame on you programmer) ');
    {$ifdef DEBUGLOAD}
      -11: write(' Loading stoped by user!');
    {$endif}
    else write(' Somethings going wrong, but I dounno about that errorcode: ',err,'  ');
    end;
    writeln('PROGRAM HALTED.'#7);
    halt;
  end;

var filename:string;
    c:char;
    savchn:array[0..15] of byte;

procedure save_chntyps;
var i:byte;
  begin
    for i:=0 to 15 do savchn[i]:=channel[i].bChannelType;
  end;

procedure revers(n:byte);
  begin
    if channel[n].bChannelType=0 then channel[n].bChannelType:=savchn[n]
    else channel[n].bChannelType:=0
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
      channel[i].bEnabled:=false;    { <- use this if you jump to previous order ... }
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

function upstr(s:string):string;
var i:byte;
  begin
    for i:=1 to length(s) do s[i]:=upcase(s[i]);
    upstr:=s;
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
    if (upcase(p[2])='H') or (p[2]='?') then { help } help:=true;
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

procedure display_keys;
  begin
    writeln(' Keys while playing : '#13#10);
    writeln(' <P> ... Pause (only on SB16)');
    writeln(' <L> ... enable/disable loopflag');
    writeln(' <D> ... doshelling :)');
    writeln(' <Alt> <1>..<''>,<Q>..<R> - Switch On/Off channel 1..16 ');
    writeln(' <+> ... Jump to next pattern');
    writeln(' <-> ... Jump to previous pattern');
    writeln(' <ESC> ... Stop playing');
    writeln(' <F1> ... help screen');
    writeln(' <F2> ... Display channel infos');
    writeln(' <F3> ... Display current pattern');
    writeln(' <F4> ... Display instrument infos');
    writeln(' <F5> ... Display sample memory positions');
    writeln(' <F6> ... Display debug information');
  end;

procedure display_help;
  begin
    writeln(' Usage :');
    writeln('  PLAYS3M <options> <S3M Filename> '#13#10);
    writeln('    þ Order does not matter');
    writeln('    þ if no extension then ''.S3M'' is added');
    writeln('    þ Options:  (use prefixes ''/'' or ''-'' to mark it as option)');
    writeln('         /Vxxx    ... set master volume 0..255 ');
    writeln('                      (default=0 - use master volume is specified in S3M)');
    writeln('         /Sxxxxx  ... set samplerate ''4000...45454'' or ''4..46''(*1000)');
    writeln('                      (higher SampleRate -> better quality !)');
    writeln('         /H or /? ... Show this screen ');
    writeln('                      (funny eh - yo you get it easier with no parameter)');
    writeln('         /M       ... use mono mixing');
    writeln('                      (default is stereo if it''s possible on your SB)');
    writeln('         /8       ... use 8bit mixing');
    writeln('                      (default is 16bit if it''s possible on your SB)');
    writeln('         /C       ... display configuration after detecting');
    writeln('                      (default is display not)');
    writeln('         /ENV     ... use informations of blaster envirment');
    writeln('         /CFG     ... input SB config by hand');
    writeln('                      (default is SB hardware autodetect)');
    writeln('         /O       ... handle order like ST3 does');
    writeln('                      (default is my own way - play ALL patterns are defined');
    writeln('                      in Order)');
    writeln('         /NOEMS   ... don''t use EMS for playing (player won''t use any EMS ');
    writeln('                      after this) - if there''s no free EMS, player''ll set');
    writeln('                      also <don''t use EMS>');
    writeln('         /LQ      ... use low quality mode');
    writeln('         /Bxx     ... start at order xx (default is 0)');
    writeln('         /Fxx     ... set Frames Per Second (default is 70Hz)');
    if not help then writeln('Gimme a filename :)');
    halt(1);
  end;

procedure display_playercfg;
  begin
    writelnSBconfig;
  end;

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
    gotoxy(1,7);textbackground(yellow);clreol;writeln('Channel  Stereo ELC Inst Note  Period  Step  Vol Effect');
    textbackground(white);textcolor(black);
    gotoxy(1,1);clreol;write('Order:   (  ) Row:    Tick:                  that is Pattern:    ');
    textbackground(green);textcolor(black);gotoxy(1,6);clreol;write(' Title: ',mod_Title);
    gotoxy(50,6);write('EMS usage: ',switch[useEMS],' Loop S3M : ');
    textbackground(blue);textcolor(lightgray);
    gotoxy(1,3);write(' Samplerate: ',playGetSampleRate:5,'  ',sw_stereo[stereo],', ',sw_res[_16bit],
    ', ',sw_order[ST3order],', ',sw_qual[playOption_LowQuality]);
    gotoxy(1,4);write(' Free DOS memory : ',(getFreeDOSMemory shr 10):6,' KiB   Free EMS memory : ',getFreeEMMMemory:5,' KiB');
    gotoxy(1,5);write(' Used EMS memory : ',(smpListGetUsedEM+patListGetUsedEM(mod_Patterns)):6,
        ' KiB   <F1> - Help screen',
        '':10,'Version : ',PLAYER_VERSION);
  end;

procedure refr_mainscr;
var
    pat: PMUSPAT;
  begin
    pat := patList_get(mod_Patterns, playState_pattern);
    textbackground(white);textcolor(black);
    gotoxy(8,1);write(playState_order:2);
    gotoxy(11,1);write(lastorder:2);
    gotoxy(20,1);write(playState_row:2);
    gotoxy(29,1);write(playState_tick:2);
    gotoxy(63,1);write(playState_pattern:2,' (',hexw( seg( patGetData(pat)^ ) ),')');
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
  help:=false;
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
  if (filename='') then display_help;
  writeln;
  {$IFDEF DEBUGLOAD}
  writeln('Free DOS memory before loading: ',getFreeDOSMemory shr 10, ' KiB');
  writeln('Free EMM memory before loading: ',getFreeEMMMemory, ' KiB');
  {$ENDIF}
  if not player_load_s3m(filename) then display_errormsg(load_error);
  {$IFDEF DEBUGLOAD}
  writeln('Free DOS memory after loading: ',getFreeDOSMemory shr 10, ' KiB');
  writeln('Free EMM memory after loading: ',getFreeEMMMemory, ' KiB');
  {$ENDIF}
  writeln(' ''',mod_Title,''' loaded ... (was saved with ',mod_TrackerName,')');
  if not player_init then display_errormsg(player_error);
  if not player_init_device(how2input) then begin writeln(' SoundBlaster not found sorry ... ');halt end;
  if disply_c then
    begin
      display_playercfg;
      write(#13#10' press a key to continue...');readkey;clrscr;gotoxy(1,19);
    end;
  { And here we go :) }
  if volume>0 then playSetMasterVolume(volume);
  playSetMode(_16bit,stereo,samplerate);
  playSetOrder(ST3order);
  save_chntyps;
  playOption_LoopSong:=true;
  screen_no:=1;startchn:=1;
  if not playStart(stereo,_16bit,_LQ) then display_errormsg(player_error);
  mainscreen;
  hide_cursor;
  repeat
    c:=#0;
    refr_mainscr;
    refresh_scr;
    if keypressed then c:=readkey;
    {if c<>#0 then write(ord(c));}
    if (c>='x') and (c<=chr(ord('x')+16)) then begin revers(ord(c)-ord('x'));c:=#0 end;
    if (ord(c)>=16) and (ord(c)<=19) then begin revers(ord(c)-4);c:=#0 end;
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
  until (sndDMABuf.flags_Slow or (c=#27) or playState_songEnded);
  if (sndDMABuf.flags_Slow) then writeln(' Sorry your PC is to slow ... ');
  view_cursor;
  stop_play;
  player_free;
  gotoxy(1,8);
  textcolor(white);textbackground(blue);
  {$IFDEF DEBUGLOAD}
  writeln('Free DOS memory after all: ',getFreeDOSMemory shr 10, ' KiB');clreol;
  writeln('Free EMM memory after all: ',getFreeEMMMemory, ' KiB');clreol;
  {$ENDIF}
end.
