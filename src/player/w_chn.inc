(* Channels *)

procedure prep_channels;
begin
    scrOpenInfoBackground(white, blue);
    scrLeave;
end;

{$I EFX_NAME.INC}

procedure refr_channels;
const types:array[0..4] of string[2] = ('--','LT','RT','AM','AD');
var i:byte;
    j:byte;
    chn: PMIXCHN;
    chtype: Byte;
    chflags: TMIXCHNFLAGS;
  begin
    gotoxy(1,8);
    textbackground(blue);
    textcolor(white);
    i:=0;
    j:=0;
    while (i < usedchannels) do
    begin
        chn := @channel[i];
        chtype := mixchn_get_type(chn);
        chflags := mixchn_get_flags(chn);
        if  (chtype < 3)
        and ((usedchannels < 17)
        or  (chtype > 0)) then
        begin
            inc(j);
            write((i+1):2, ' (', types[chtype], ') ');
            if (chflags and MIXCHNFL_ENABLED <> 0) then
                write('e')
            else
                write('-');
            if (chflags and MIXCHNFL_PLAYING <> 0) then
                write('p')
            else
                write('-');
            if (chflags and MIXCHNFL_MIXING <> 0) then
                write('m')
            else
                write('-');
            if (chn^.bSmpFlags and SMPFLAG_LOOP <> 0) then
                write('l')
            else
                write('-');
            if (chn^.bEffFlags and EFFFLAG_CONTINUE <> 0) then
                write('c')
            else
                write('-');
            write(' (',mixchn_get_instrument_num(chn):2,') [');
            if (mixchn_is_enabled(chn)) then
              begin
                write_note(chn^.bNote);
                { Arpeggio test : }
                {write(',');write_note(chn^.bArpNotes[0]);write(',');write_note(chn^.bArpNotes[1]);}
              end
            else write('...');
            write('] ', mixchn_get_sample_period(chn):5, ' ', mixchn_get_sample_step(chn)/64/1024:7:4);
            { Arpeggio test : }
            {write(' ', chn^.dArpSmpSteps[1].DD/64/1024:7:4);
            write(' ', chn^.dArpSmpSteps[2].DD/64/1024:7:4);}
            write(' ', mixchn_get_sample_volume(chn):2);
            if (mixchn_get_command(chn) <= MAXEFF) then
                write(' ',get_effectname(i,
                    mixchn_get_command(chn),
                    mixchn_get_sub_command(chn),
                    mixchn_get_command_parameter(chn)));
            clreol;
            write(#13#10);
          end;
        inc(i);
      end;
    if j=0 then writeln(' No digital channels to mix ...     :( ');
  end;

