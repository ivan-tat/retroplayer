{$M 16000,0,1000}
program example_for_s3mplay;

uses
    crt,
    dos,
    string_,
    sbctl,
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
  var
      name: array [0..255] of Char;
    begin
      { setup defaults: }
      Samplerate:=45454;
      Stereo:=stereo_calc;
      _16bit:=_16bit_calc;
      { end of default ... }
      memcpy(name, filename[1], Ord(filename[0]));
      name[Ord(filename[0])] := Chr(0);
      if (not player_load_s3m(name)) then
          halt;
      writeln(' ''',mod_Title,''' loaded ... (was saved with ',mod_TrackerName,')');
      if not player_init then halt;
      if not player_init_device(1) then begin writeln(' SoundBlaster not found sorry ... ');halt end;
      player_set_mode(_16bit,stereo,samplerate,false);
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
  if (not player_set_mode(_16bit,stereo,samplerate,false)) then halt;
  if (not playStart) then halt;
  writeln(#13#10' Return to player and stop playing with ''EXIT'' ... ');
  swapvectors;
  exec(getenv('COMSPEC'),'');
  swapvectors;
  writeln('DOSerror :',doserror,' (if 8 -> not enough memory for COMMAND.COM)');
  player_free;
end.
