const
    smpname: TEMSNAME = 'Samples'#0;
    savname: TEMSNAME = 'saveMAP'#0;

procedure setEMSnames;
begin
    patListSetHandleName(mod_Patterns);
    if ( EMSsmp ) then
        emsSetHandleName( smpEMShandle, @smpname );
    if ( useEMS ) then
        emsSetHandleName( savhandle, @savname );
end;

procedure convert2pas(var from,topas;maxchars:byte);
var
    i: integer;
    src, dst: parray;
begin
    src := @from;
    dst := @topas;
    i := 0;
    while ( ( i < maxchars ) and ( src^[i] <> 0 ) ) do
    begin
        dst^[ i+1 ] := src^[ i ];
        inc( i );
    end;
    dst^[0] := i;
end;

function getchtyp(b:byte):byte;
begin
    case b of
    0..7: getchtyp := 1; (* left *)
    8..15: getchtyp := 2; (* right *)
    16..23: getchtyp := 3; (* adlib melody *)
    24..31: getchtyp := 4; (* adlib drums *)
    else
        getchtyp := 0;
    end;
end;

(*temporary solution for file I/O*)
(*$I-*)
function _fread(p: Pointer; size: Word; n: Word; var stream: File): Word;
var
    s, actual: Word;
begin
    s := size * n;
    if (s <> 0) then
    begin
        BlockRead(stream, p^, s, actual);
        if (IOResult = 0) then
            _fread := n
        else
            _fread := 0;
    end else
        _fread := 0;
end;
function _fsetpos(var stream: File; pos: LongInt): Integer;
begin
	Seek(stream, pos);
	if (IOResult = 0) then
		_fsetpos := 0
	else
		_fsetpos := -1;
end;
(*$I+*)

FUNCTION  player_load_s3m(name:string):BOOLEAN;
var f:file;
    header:Theader;
    maxused:byte;
    inspara:array[1..MAX_INSTRUMENTS] of word;
    patpara:array[0..MAX_PATTERNS] of word;
    smppara:array[0..MAX_INSTRUMENTS] of longint;
    i:byte;
    inspos,patpos,smppos,smpnum:byte;
    nextins,nextpat,nextsmp:longint;
    wdummy:word;
    p:pointer;
    pAr:PArray;
    buffer:PArray;
    { EMS things: }
    Ppagesleft:byte;  { number of pages left to use for patterns }
    curPpage:byte;    { current logical EMS page we fill with next pattern }
    curpart:byte;     { =0,1,2 -> every page is seperated in 3 parts (one part - one pattern) }
    curSpage:word;    { current logical EMS page we fill with next sample }
    Spagesleft:word;  { number of pages left to use for samples }
    fun:string;
    funptr:pointer;

  PROCEDURE allocEMSforSamples;
  var w,w0:word;
      i:integer;
      pSmp:PSMPheader;
    begin
      if emsGetFreePagesCount=0 then
      begin
        EMSsmp:=false;
        exit;
      end;
      w:=0;
      for i:=1 to 99 do
        begin
          pSmp:=addr(Instruments^[i]);
          if pSmp^.typ=1 then { really a sample }
            begin
              if pSmp^.flags and 1 = 1 then w0:=pSmp^.loopend+1024 else w0:=pSmp^.length+1024;
              w:=w + w0 div (16*1024) + ord(w0 mod (16*1024)>0);
            end;
        end;
      {$IFDEF DEBUGLOAD}
      writeln(' Instruments to load : ',insnum);
      writeln(' EMS pages are needed for Samples : ',w);
      {$ENDIF}
      { w = number of 16Kb pages in EMS }
      if w>emsGetFreePagesCount then { not enough EMS for all samples }
        begin
          { use as many pages as possible :) }
          w:=emsGetFreePagesCount;
          smpEMShandle:=emsAlloc(w);
        end
      else { oh well enough, that's nice }
        begin
          { fine let's load everything into EMS }
          smpEMShandle:=emsAlloc(w);
        end;
      {$IFDEF DEBUGLOAD}
      writeln(' EMS pages allocated for Samples : ',w);
      {$ENDIF}
      Spagesleft:=w;
      EMSsmp:=true;
      curSpage:=0;
    end;

  PROCEDURE freeallmem;
    begin
        if (buffer <> Nil) then
        begin
            _dos_freemem(seg(buffer^));
            buffer := nil;
        end;
        player_free_module;
    end;

  function load_instrument(var f: File; pos: LongInt): Boolean;
  var length:word;
      typ:byte;
      pAr:Parray;
      Psmp:PSmpHeader;
    BEGIN
      load_instrument:=false;
      if (_fsetpos(f, pos) <> 0) then
      begin
          Debug_Err(__FILE__, 'load_instrument', 'Failed to read file.');
          load_error := filecorrupt;
          exit;
      end;
      {$IFDEF LOADINFO}
      write('I',inspos-1);
      {$ENDIF}
      { now read instrument header : }
      if (_fread(@(Instruments^[inspos-1]), sizeof(TSmpHeader), 1, f) <> 1) then
      begin
          Debug_Err(__FILE__, 'load_instrument', 'Failed to read instrument header.');
          load_error := filecorrupt;
          exit;
      end;
      pSmp:=addr(instruments^[inspos-1]);
      if pSmp^.typ=1 then { that instrument is a sample }
        begin
          if pSmp^.packinfo <> 0 then
          begin
              Debug_Err(__FILE__, 'load_instrument', 'Packed sample is not supported.');
              load_error := packedsamples;
              exit;
          end;
          { calc position in file : }
          smppara[smpnum]:=(longint(256*256)*pSmp^.HI_mempos+pSmp^.mempos);
          pSmp^.mempos:=0;
          {$IFDEF LOADINFO}
          write('!');
          {$ENDIF}
        end
      else
        begin
          smppara[smpnum]:=0;
          {$IFDEF LOADINFO}
          write('$');
          {$ENDIF}
        end;
	  inc(smpnum);
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}
      load_instrument:=true;
    END;

  function load_sample(var f: File; pos: LongInt): Boolean;
  var
      par:parray;
      pSmp:pSmpHeader;
      z,h:word;
      i:byte;
      smplen:word;
      _seg: word;
    begin
      load_sample:=false;
      if (_fsetpos(f, pos) <> 0) then
      begin
          Debug_Err(__FILE__, 'load_sample', 'Failed to read file.');
          load_error := filecorrupt;
          exit;
      end;
      pSmp:=addr(Instruments^[smppos]);
      if (pSmp^.flags and 1)=1 then smplen:=pSmp^.loopend else smplen:=pSmp^.length;
      if smplen>64511 then
      begin
          Debug_Err(__FILE__, 'load_sample', 'Sample too large.');
          load_error := sample2large;
          exit;
      end;
      {$IFDEF LOADINFO}
      write('S',smppos,'(',smplen,')');
      {$ENDIF}
      z:=((smplen+1024) div (16*1024))+ord((smplen+1024) mod (16*1024)>0);
      if useEMS and EMSsmp and (Spagesleft>=z) then
        begin
          {$IFDEF LOADINFO}
          write('E(',curSpage,'-',curSpage+z-1,')');
          {$ENDIF}
          pSmp^.mempos:=$f000+curSpage; { and z-1 pages after }
          for i:=0 to z-1 do
            if not emsMap(smpEMShandle,curSpage+i,i) then write('<EMS-ERROR>');
          inc(curSpage,z);
          par := emsFramePtr;
        end
      else { we have to use normal memory (geeee) for this sample }
        begin
            if (_dos_allocmem((smplen+1024+15) shr 4, _seg) <> 0) then
            begin
                Debug_Err(__FILE__, 'load_sample', 'Failed to allocate DOS memory for sample data.');
                load_error := notenoughmem;
                exit;
            end;
            par := ptr(_seg, 0);;
            pSmp^.mempos:=seg(par^);
        end;
        if (_fread(par, smplen, 1, f) <> 1) then
        begin
            Debug_Err(__FILE__, 'load_sample', 'Failed to read sample data.');
            load_error := filecorrupt;
            exit;
        end;
      if (Psmp^.flags and 1)=1 then
        { if loop then copy from loopstart : }
        begin
          h:=1024;
          while h>0 do
            begin
              if h>psmp^.loopend-psmp^.loopbeg+1 then
                begin
                  move(par^[psmp^.loopbeg],par^[smplen+1024-h],psmp^.loopend-psmp^.loopbeg);
                  dec(h,psmp^.loopend-psmp^.loopbeg);
                end
              else
                begin
                  move(par^[psmp^.loopbeg],par^[smplen+1024-h],h);
                  h:=0;
                end;
            end;
        end
      else fillchar(par^[smplen],1024,128);
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}
      load_sample:=true;
    end;

  function load_decrunc_pattern(var f: File; pos: LongInt): Boolean;
  var row:byte;
      crunch:byte;
      chn:byte;
      length:word;
      linecount:byte;
      pat_static: TMUSPAT;
      pat: PMUSPAT;
      _seg: word;
    BEGIN
      load_decrunc_pattern:=false;

      if (pos = 0) then
      begin
        pat := patList_get(mod_Patterns, patpos-1);
        muspat_clear(pat);
        load_decrunc_pattern := true;
        exit;
      end;

      if (_fsetpos(f, pos) <> 0) then
      begin
          Debug_Err(__FILE__, 'load_decrunc_pattern', 'Failed to read file.');
          load_error := filecorrupt;
          exit;
      end;
      if (_fread(@length, 2, 1, f) <> 1) then
      begin
          Debug_Err(__FILE__, 'load_decrunc_pattern', 'Failed to read pattern.');
          load_error := filecorrupt;
          exit;
      end;
      {$IFDEF LOADINFO}
      write('P',patpos-1,'(',length,')');
      {$ENDIF}
      if (length = 0) or (length > 10*1024) then
      begin
          Debug_Err(__FILE__, 'load_decrunc_pattern', 'Bad pattern size.');
          load_error := filecorrupt;
          exit;
      end;
      { read whole packed pattern }
      if (_fread(buffer, length - 2, 1, f) <> 1) then
      begin
          Debug_Err(__FILE__, 'load_decrunc_pattern', 'Failed to read pattern.');
          load_error := filecorrupt;
          exit;
      end;

      pat := @pat_static;
      muspat_clear(pat);
      muspat_set_channels(pat, UsedChannels);
      muspat_set_rows(pat, 64);

      { first get memory : (if useEMS than try to put it into the EMS ... }
      if useEMS and
        patListIsInEM(mod_Patterns) and
        (curpart < patListGetPatPerPage(mod_Patterns)) then
        begin
          muspat_set_EM_data(pat, true);
          muspat_set_own_EM_handle(pat, false);
          muspat_set_EM_data_handle(pat, patListGetHandle(mod_Patterns));
          muspat_set_EM_data_page(pat, curPpage);
          muspat_set_EM_data_offset(pat, curpart * 5 * UsedChannels * 64);
          patList_set(mod_Patterns, patpos-1, pat);
          p := muspat_map_EM_data(pat);
          if (p = nil) then
          begin
              Debug_Err(__FILE__, 'load_decrunc_pattern', 'Failed to map EM for pattern.');
              load_error := internal_failure;
              exit;
          end;
        end
      else
        begin
            if (_dos_allocmem((longint(64*5)*usedchannels + 15) shr 4, _seg) <> 0) then
            begin
                Debug_Err(__FILE__, 'load_decrunc_pattern', 'Failed to allocate DOS memory for pattern.');
                load_error := notenoughmem;
                exit;
            end;
            p := ptr(_seg, 0);
            muspat_set_EM_data(pat, false);
            muspat_set_data(pat, p);
            patList_set(mod_Patterns, patpos-1, pat);
        end;

      unpackPattern(buffer, p, 64, UsedChannels);

      pat := patList_get(mod_Patterns, patpos-1);
      if (muspat_is_EM_data(pat)) then
        begin
          {$IFDEF LOADINFO}
          write('E(',curPpage,',',curpart,')');
          {$ENDIF}
          { next position in EMS : }
          inc(curpart);
          if (curpart = patListGetPatPerPage(mod_Patterns)) and (Ppagesleft>0) then
            begin
              dec(Ppagesleft);
              inc(curPpage);
              curpart:=0;
            end;
        end;
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}
      load_decrunc_pattern:=true;
    END;

var
    Inst_done:boolean;
    load_smp_later:boolean;
    firstSMP:boolean;
    _seg: word;

  BEGIN
    player_load_s3m := FALSE;

    (* Patterns list *)
    mod_Patterns := patList_new;
    if (mod_Patterns = nil) then
    begin
        DEBUG_ERR(__FILE__, 'player_load_s3m', 'Failed to initialize patterns.');
        exit;
    end;
    patList_clear(mod_Patterns);

    useEMS:=emsInstalled and useEMS and (emsGetFreePagesCount>1);
    { we need one page for saving mapping while playing }
    load_error := 0;
    buffer := nil;
    assign(f,name);
    reset(f,1);               { open file - 16byte blocks :) }
    IF IORESULT<>0 then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to open file.');
        load_error := filenotexist;
        exit;
    end;

    if (_fread(@header, sizeof(THeader), 1, f) <> 1) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to read file''s header.');
        load_error := wrongformat;
        exit;
    end;

    if (header.filetyp <> 16)
    or (header.SCRM_ID <> $4d524353)
    or ((header.CWTV shr 8) and $ff <> $13)
    or (not (header.ffv in [1, 2])) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Unsupported file format.');
        load_error := wrongformat;
        exit;
    end;

    { set some variables : }
    convert2pas(header.name,mod_Title,28);
    ordnum:=header.ordnum;
    insnum:=header.insnum;
    patList_set_count(mod_Patterns, header.patnum);
    { setup flags }
    modOption_ST2Vibrato := header.flags and $01 <> 0;
    modOption_ST2Tempo := header.flags and $02 <> 0;
    modOption_AmigaSlides := header.flags and $04 <> 0;
    modOption_VolZeroOptim := header.flags and $08 <> 0;
    modOption_AmigaLimits := header.flags and $10 <> 0;
    modOption_SBFilter := header.flags and $20 <> 0;
    modOption_CostumeFlag := header.flags and $80 <> 0;
    mod_TrackerName := 'Scream Tracker ' +
        chr( ord('0') + ( ( header.cwtv shr 8 ) and $0f ) ) +
        '.' +
        chr( ord('0') + ( ( header.cwtv shr 4 ) and $0f ) ) +
        chr( ord('0') + ( header.cwtv and $0f ) );
    modOption_SignedData := (header.ffv = 1);
    playState_gVolume:=header.gvolume;
    playState_mVolume:=header.mvolume and $7f;
    modOption_Stereo := (header.mvolume and $80 <> 0);
    initState_speed:=header.initialspeed;
    initState_tempo:=header.initialtempo;
    { setup channels : }
    maxused:=0;
    for i:=0 to 31 do
      begin
        channel[i].bEnabled:=(header.channelset[i] and 128=0);
        channel[i].bChannelType:=getchtyp(header.channelset[i] and 31);
        if channel[i].bEnabled and (channel[i].bChannelType>0) and (channel[i].bChannelType<3) then maxused:=i+1;
      end;
    usedchannels:=maxused;
    {$IFDEF DEBUGLOAD}
    writeln(' Used channels :',usedchannels);
    {$ENDIF}
    { now load arrangment : }
    if (_fread(@Order, ordnum, 1, f) <> 1) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to read patterns order.');
        load_error := filecorrupt;
        exit;
    end;
    { check order if there's one 'real' (playable) entry ... }
    i:=0;
    while (i<ordnum) and (order[i]>=254) do inc(i);
    if i=ordnum then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Playable entry not found.');
        load_error := ordercorrupt;
        exit;
    end;
    if (_fread(@inspara, insnum * 2, 1, f) <> 1) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to read instruments headers.');
        load_error := filecorrupt;
        exit;
    end;

    if (_fread(@patpara, patList_get_count(mod_Patterns) * 2, 1, f) <> 1) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to read patterns offsets.');
        load_error := filecorrupt;
        exit;
    end;
    { Ok now the difficult part ...
      (load patterns/samples/instrumentdata)
      - load them in a row (don't jump through the file, that costs time !
      - problem is that you don't know the order and possibly there's no !
    }
    patListSetPatLength(mod_Patterns, 5 * 64 * UsedChannels);
    {$IFDEF DEBUGLOAD}
    writeln(' length of Patterns in memory: ', patListGetPatLength(mod_Patterns));
    {$ENDIF}
    if useEMS then
      begin
        { we use EMS, then we need a page to save mapping in interrupt ! }
        savHandle:=emsAlloc(1); { 1 page is enough ? }
        { let's continue with loading: }
        patListSetPatPerPage(mod_Patterns, (16 * 1024) div patListGetPatLength(mod_Patterns));
        {$IFDEF DEBUGLOAD}
        writeln(' Patterns per Page: ',patListGetPatPerPage(mod_Patterns));
        {$ENDIF}
        { try to allocate EMS for all patterns : }
        if (emsGetFreePagesCount <
            (patList_get_count(mod_Patterns)+(patListGetPatPerPage(mod_Patterns)-1)) div
            patListGetPatPerPage(mod_Patterns)) then
          begin
            Ppagesleft:=emsGetFreePagesCount;
            patListSetHandle(mod_Patterns, emsAlloc(Ppagesleft));
            patListSetUseEM(mod_Patterns, true);
          end
        else
          begin
            patListSetHandle(mod_Patterns,
                emsAlloc(
                    (patList_get_count(mod_Patterns)+
                    (patListGetPatPerPage(mod_Patterns)-1)) div
                    patListGetPatPerPage(mod_Patterns)
                )
            );
            Ppagesleft:=(
                patList_get_count(mod_Patterns)+
                (patListGetPatPerPage(mod_Patterns)-1)
                ) div
                patListGetPatPerPage(mod_Patterns);
            patListSetUseEM(mod_Patterns, true);
          end;
      end;
    if useEMS and patListIsInEM(mod_Patterns) then
      begin
        curpart:=0;
        curPpage:=0;
      end;
    { clear all samples }
    fillchar(instruments^,MAX_INSTRUMENTS*sizeof(TInstr),0);
    { Now try to load everything in a row }
    {$IFDEF LOADINFO}
    writeln(#10#13'load report :');
    {$ENDIF}
    Inst_done:=false;  { Instrument are not loaded yet :) }
    load_smp_later:=false; { load instruments not later (up to now we can say only this) }
    firstSMP:=true; { if we load now an instrument, then it's the first =) }
    { init buffer for fast loading : }
    if (_dos_allocmem((10*1024 + 15) shr 4, _seg) <> 0) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to allocate DOS memory for buffer.');
        load_error := notenoughmem;
        exit;
    end;
    buffer := ptr(_seg, 0);
    { init some variables for loading : }
    inspos:=1;
    patpos:=0;
    smppos:=0;
    smpnum:=0;
    nextpat:=$7fffffff;
    nextins:=$7fffffff;
    nextsmp:=$7fffffff;
    while (inspos<insnum+1)
    or (patpos<patList_get_count(mod_Patterns))
    or (smppos<smpnum)
    or (nextpat<$7fffffff)
    or (nextins<$7fffffff)
    or (nextsmp<$7fffffff) do
      begin
        {writeln('--->',inspos,',',patpos,',',smppos);readkey;}
        if (nextpat=$7fffffff) and (patpos<patList_get_count(mod_Patterns)) then
          begin
            nextpat:=patpara[patpos];
            inc(patpos);
          end;
        if (nextins=$7fffffff) and (inspos<insnum+1) then
          begin
            nextins:=inspara[inspos];
            inc(inspos);
          end;
        if (nextsmp=$7fffffff) and (smppos<smpnum) then
          begin
            nextsmp:=smppara[smppos];
            inc(smppos);
          end;
        if (nextpat<nextins) and (nextpat<nextsmp) then
          begin
            { pattern }
            if (nextpat<$7fffffff) then
                if (not load_decrunc_pattern(f, nextpat * 16)) then
                begin
                    freeallmem;
                    exit;
                end;
            nextpat:=$7fffffff;
          end
        else
        if (nextins<nextsmp) then
          begin
            { instrument }
            if (nextins<$7fffffff) then
                if (not load_instrument(f, nextins * 16)) then
                begin
                    freeallmem;
                    exit;
                end;
            nextins:=$7fffffff;
            inst_done:=(inspos=insnum+1);
          end
        else { sampledata }
          begin
            if (nextsmp>0) and not load_smp_later then
              begin
                if not Inst_done and useEMS then load_smp_later:=true
                { if all instruments are not loaded yet and we want to load into the EMS then
                stop loading here - do it after all Instruments are done ... }
                  else
                    begin
                      if useEMS and firstSMP then
                      begin
                          allocEMSforSamples;
                          firstSMP:=false;
                      end;
                      if (nextsmp<$7fffffff) then
                      if (not load_sample(f, nextsmp*16)) then
                      begin
                          freeallmem;
                          exit;
                      end;
                    end;
              end;
            nextsmp:=$7fffffff;
          end;
        if keypressed then
          if readkey=#27 then
            begin
              writeln(' Somethings going wrong with loading ? Or why do you pressed <ESC> ?');
              writeln(' If loading error - please report me.');
              load_error:=internal_failure;
              freeallmem;
              exit;
            end;
      end;
    { And now for ugly orders :
      if instrumentdata was not fully loaded as the first sampledata starts,
      then we have to wait, coze we don't know how many EMS we should acolate
      now we know it so let's start again at the beginning of the file and
      load the samples in a row ... }
    if UseEMS and load_smp_later then
      begin
        allocEMSforSamples;
        smppos:=0;
        smpnum:=0;
        nextpat:=$7fffffff;
        nextins:=$7fffffff;
        nextsmp:=$7fffffff;
        while (smppos<smpnum) or (nextsmp<$7fffffff) do
          begin
            if (nextsmp=$7fffffff) and (smppos<smpnum) then
            begin
                nextsmp:=smppara[smppos];
                inc(smppos)
            end;
            if (nextsmp<$7fffffff) then
                if (not load_sample(f, nextsmp*16)) then
                begin
                    freeallmem;
                    exit
                end;
            nextsmp:=$7fffffff;
          end;
      end;
    {$IFDEF DEBUGLOAD}
    writeln(#10);
    {$ENDIF}
    { free buffer : }
    _dos_freemem(seg(buffer^));
    buffer := nil;
    { Just for fun set names for EMS handles (does only work for EMS>= v4.0) }
    if ( emsVersion.Hi >= 4 ) then setEMSnames;
    mod_isLoaded := true;
    player_load_s3m :=TRUE;
  END;
