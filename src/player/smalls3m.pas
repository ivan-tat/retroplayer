{$M 16000,0,1000}
program example_for_s3mplay;

uses
    crt,
    dos,
    sbctl,
    blaster,
    s3mvars,
    s3mplay;

(*$I defines.pas*)

const stereo_calc=true;
      _16bit_calc=false;        { 16bit play not yet possible }

var samplerate:word;
    Stereo:Boolean;
    _16bit:Boolean;
    filename:string;

  procedure init;
    begin
      { setup defaults: }
      Samplerate:=45454;
      Stereo:=stereo_calc;
      _16bit:=_16bit_calc;
      { end of default ... }
      if not load_S3M(filename) then halt;
      writeln(' ''',mod_Title,''' loaded ... (was saved with ',mod_TrackerName,')');
      if not Init_S3Mplayer then halt;
      if not init_device(1) then begin writeln(' SoundBlaster not found sorry ... ');halt end;
      playSetMode(_16bit,stereo,samplerate);
      set_ST3order(true);
      playOption_LoopSong:=true;
    end;

begin
  textbackground(black);textcolor(lightgray);
  clrscr;
  writeln(' SMART-S3M-PLAYER for SoundBlasters written by Cyder of Green Apple');
  writeln(' ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~');
  writeln(' Version : ',PLAYER_VERSION);
  filename:=paramstr(1);
  if (filename='') then halt;
  writeln;
  Init;
  if not startplaying(stereo,_16bit,false) then halt;
  writeln(#13#10' Return to player and stop playing with ''EXIT'' ... ');
  swapvectors;
  exec(getenv('COMSPEC'),'');
  swapvectors;
  writeln('DOSerror :',doserror,' (if 8 -> not enough memory for COMMAND.COM)');
  stop_play;
  done_module;
  done_S3Mplayer;
end.
