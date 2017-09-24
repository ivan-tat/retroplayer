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
      if not player_load_s3m(filename) then halt;
      writeln(' ''',mod_Title,''' loaded ... (was saved with ',mod_TrackerName,')');
      if not player_init then halt;
      if not player_init_device(1) then begin writeln(' SoundBlaster not found sorry ... ');halt end;
      playSetMode(_16bit,stereo,samplerate);
      playSetOrder(true);
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
  if not playStart(stereo,_16bit,false) then halt;
  writeln(#13#10' Return to player and stop playing with ''EXIT'' ... ');
  swapvectors;
  exec(getenv('COMSPEC'),'');
  swapvectors;
  writeln('DOSerror :',doserror,' (if 8 -> not enough memory for COMMAND.COM)');
  stop_play;
  player_free;
end.
