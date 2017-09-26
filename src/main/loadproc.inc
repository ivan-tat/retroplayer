const
    patname: TEMSNAME = 'Pattern'#0;
    smpname: TEMSNAME = 'Samples'#0;
    savname: TEMSNAME = 'saveMAP'#0;

procedure setEMSnames;
begin
    if ( patListUseEM ) then
        emsSetHandleName( patListEMHandle, @patname );
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
    fileposit:longint;
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
      if emsGetFreePagesCount=0 then begin EMSsmp:=false;exit end;
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

  PROCEDURE forget(count:longint);
  var dummy:array[0..511] of byte;
      i:word;
    begin
      for i:=1 to count div 512 do blockread(f,dummy,512);
      if count mod 512 >0 then blockread(f,dummy,(count mod 512));
    end;

  FUNCTION load_instrument:boolean;
  var length:word;
      typ:byte;
      pAr:Parray;
      Psmp:PSmpHeader;
      PAdl:PAdlHeader;
    BEGIN
      load_instrument:=false;
      { first jump to position }
      if (fileposit>nextins*16) then
        { shit tables not sorted - more disk access }
        begin
          reset(f,1);
          seek(f,nextins*16); { <- we start reading from filestart again
                                and read till we are at start of this pattern ... }
          if IOresult<>0 then begin load_error:=filecorrupt;exit end;
          {$IFDEF DEBUGLOAD}
          writeln(#13#10'somethings going wrong with order. position was: ',fileposit,' but we need : ',nextins*16);
          {$ENDIF}
        end
      else
      if fileposit<nextins*16 then
        { that's better - we only have to forget some blocks }
        forget(nextins*16-fileposit);
      fileposit:=nextins*16;
      {$IFDEF LOADINFO}
      write('I',inspos-1);
      {$ENDIF}
      { now read instrument header : }
      blockread(f,Instruments^[inspos-1],5*16);
      inc(fileposit,5*16);
      pSmp:=addr(instruments^[inspos-1]);
      pAdl:=addr(instruments^[inspos-1]);
      if pSmp^.typ=1 then { that instrument is a sample }
        begin
          if pSmp^.packinfo <> 0 then begin load_error:=packedsamples;exit end;
          { calc position in file : }
          smppara[smpnum]:=(longint(256*256)*pSmp^.HI_mempos+pSmp^.mempos);
          pSmp^.mempos:=0;inc(smpnum);
          {$IFDEF LOADINFO}
          write('!');
          {$ENDIF}
        end
      else
        begin
          smppara[smpnum]:=0;inc(smpnum);
          {$IFDEF LOADINFO}
          write('$');
          {$ENDIF}
        end;
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}
      load_instrument:=true;
    END;

  FUNCTION load_sample:boolean;
  var p:pointer;
      par:parray;
      pSmp:pSmpHeader;
      z,h:word;
      i:byte;
      smplen:word;
      _seg: word;
    begin
      load_sample:=false;
      if (fileposit>nextsmp*16) then
        { shit tables not sorted - more disk access }
        begin
          reset(f,1);
          seek(f,nextsmp*16); { <- we start reading from filestart again
                                and read till we are at start of this pattern ... }
          if IOresult<>0 then begin load_error:=filecorrupt;exit end;
          {$IFDEF DEBUGLOAD}
          writeln(#13#10'somethings going wrong with order. position was: ',fileposit,' but we need : ',nextsmp*16);
          {$ENDIF}
        end
      else
      if fileposit<nextsmp*16 then forget(nextsmp*16-fileposit);

      fileposit:=nextsmp*16;
      pSmp:=addr(Instruments^[smppos]);
      if (pSmp^.flags and 1)=1 then smplen:=pSmp^.loopend else smplen:=pSmp^.length;
      if smplen>64511 then begin load_error:=sample2large;exit end;
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
          par:=emsFramePtr;
          blockread(f,par^,smplen);
        end
      else { we have to use normal memory (geeee) for this sample }
        begin
            if (_dos_allocmem((smplen+1024+15) shr 4, _seg) <> 0) then
            begin
                load_error:=notenoughmem;
                exit;
            end;
            p := ptr(_seg, 0);
            blockread(f,p^,smplen);
            pSmp^.mempos:=seg(p^);
            par:=p;
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
                  move(par^[psmp^.loopbeg],par^[smplen+1024-h],h);h:=0;
                end;
            end;
        end
      else fillchar(par^[smplen],1024,128);
      if (pSmp^.flags and 1 = 1) and (pSmp^.loopend<pSmp^.length) then
        forget(pSmp^.length-pSmp^.loopend);
      inc(fileposit,pSmp^.length);
      if IORESULT<>0 then begin write(' Geeee ... (',fileposit,')');load_error:=filecorrupt;exit end;
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}
      load_sample:=true;
    end;

  FUNCTION load_decrunc_pattern:boolean;
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
      if nextpat=0 then
      begin
        load_decrunc_pattern := true;
        pat := @pat_static;
        pat_clear(pat);
        patList_set(patpos-1, pat);
        exit;
      end;
      { first jump to position }
      if (fileposit>nextpat*16) then
        { shit tables not sorted - more dsik access :( }
        begin
          reset(f,1);
          seek(f,nextpat*16); { <- we start reading from filestart again
                                   and read till we are at start of this pattern ... }
          {$IFDEF DEBUGLOAD}
          writeln(#13#10'somethings going wrong with order. position was: ',fileposit,' but we need : ',nextpat*16);
          {$ENDIF}
          if IOresult<>0 then begin load_error:=filecorrupt;exit end;
        end
      else
      if fileposit<nextpat*16 then
        forget(nextpat*16-fileposit);
      fileposit:=nextpat*16;
      blockread(f,length,2); { <- length of packed pattern }
      {$IFDEF LOADINFO}
      write('P',patpos-1,'(',length,')');
      if length>10*1024 then
        begin
          writeln('Packed data longer then 10K - that''s not allowed ...'#7' PROGRAM HALTED.');
          halt;
        end;
      {$ENDIF}
      { read whole packed pattern }
      blockread(f,buffer^,length-2); { length=sizeof(packdata)+(sizeof(length)=2) }
      if IOresult<>0 then begin load_error:=filecorrupt;exit end;
      inc(fileposit,length);
      { first get memory : (if useEMS than try to put it into the EMS ... }
      if useEMS and patListUseEM and (curpart<patListPatPerEMPage) then
        begin
          pat := @pat_static;
          pat_clear(pat);
          patSetDataInEM(pat, curPpage, curpart);
          patList_set(patpos-1, pat);
          p := patMapData(pat);
          if (p = nil) then write('<EMS-ERROR>');
        end
      else
        begin
            if (_dos_allocmem((longint(64*5)*usedchannels + 15) shr 4, _seg) <> 0) then
            begin
                load_error:=notenoughmem;
                exit;
            end;
            p := ptr(_seg, 0);
            pat := @pat_static;
            pat_clear(pat);
            patSetData(pat, p);
            patList_set(patpos-1, pat);
        end;

      unpackPattern(buffer, p, 64, UsedChannels);

      pat := patList_get(patpos-1);
      if (patIsDataInEM(pat)) then
        begin
          {$IFDEF LOADINFO}
          write('E(',curPpage,',',curpart,')');
          {$ENDIF}
          { next position in EMS : }
          inc(curpart);
          if (curpart=patListPatPerEMPage) and (Ppagesleft>0) then
            begin
              dec(Ppagesleft);inc(curPpage);
              curpart:=0;
            end;
        end;
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}
      load_decrunc_pattern:=true;
    END;

  function fileexist(s:string):boolean;
  var f:file;
    begin
      assign(f,s);reset(f,1);fileexist:=ioresult=0;close(f);if ioresult<>0 then;
    end;

var a,b,c:string;
    Inst_done:boolean;
    load_smp_later:boolean;
    firstSMP:boolean;
    _seg: word;

  BEGIN
    player_load_s3m := FALSE;
    useEMS:=emsInstalled and useEMS and (emsGetFreePagesCount>1); { we need one page for saving mapping while playing }
    load_error:=0;buffer:=Nil;
    fsplit(name,a,b,c);
    if not fileexist(a+b+c) then name:=a+b+'.S3M';
    assign(f,name);
    reset(f,1);               { open file - 16byte blocks :) }
    IF IORESULT<>0 THEN begin load_error:=filenotexist;exit end;
    { First read fileheader }
    blockread(f,header,sizeof(THeader));
    IF IORESULT<>0 THEN begin load_error:=wrongformat;exit end;
    { check if it's really a S3M ... }
    IF header.filetyp<>16 then begin load_error:=wrongformat;exit end;
    if ( header.SCRM_ID <> $4d524353 ) then
    begin
        load_error := wrongformat;
        exit;
    end;
    if ( ( header.CWTV shr 8 ) and $ff <> $13 ) then
    begin
        load_error := wrongformat;
        exit;
    end;
    { set some variables : }
    convert2pas(header.name,mod_Title,28);
    ordnum:=header.ordnum;
    insnum:=header.insnum;
    patListCount:=header.patnum;
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
    if not (header.ffv in [1,2]) then begin load_error:=wrongformat;exit end;
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
    blockread(f,Order,ordnum);
    IF IORESULT<>0 THEN begin load_error:=filecorrupt;exit end;
    { check order if there's one 'real' (playable) entry ... }
    i:=0;while (i<ordnum) and (order[i]>=254) do inc(i);
    if i=ordnum then begin load_error:=ordercorrupt;exit end; { playable entry not found :( }
    blockread(f,inspara,insnum*2);
    IF IORESULT<>0 THEN begin load_error:=filecorrupt;exit end;
    blockread(f,patpara,patListCount*2);
    IF IORESULT<>0 THEN begin load_error:=filecorrupt;exit end;
    close(f);
    { Ok now the difficult part ...
      (load patterns/samples/instrumentdata)
      - load them in a row (don't jump through the file, that costs time !
      - problem is that you don't know the order and possibly there's no !
    }
    patListPatLength:=5*64*usedchannels;
    {$IFDEF DEBUGLOAD}
    writeln(' length of Patterns in memory: ',patListPatLength);
    {$ENDIF}
    if useEMS then
      begin
        { we use EMS, then we need a page to save mapping in interrupt ! }
        savHandle:=emsAlloc(1); { 1 page is enough ? }
        { let's continue with loading: }
        patListPatPerEMPage:=(16*1024) div patListPatLength;
        {$IFDEF DEBUGLOAD}
        writeln(' Patterns per Page: ',patListPatPerEMPage);
        {$ENDIF}
        { try to allocate EMS for all patterns : }
        if (emsGetFreePagesCount<(patListCount+(patListPatPerEMPage-1)) div patListPatPerEMPage) then
          begin
            Ppagesleft:=emsGetFreePagesCount;
            patListEMHandle:=emsAlloc(Ppagesleft);
            patListUseEM:=true;
          end
        else
          begin
            patListEMHandle:=emsAlloc((patListCount+(patListPatPerEMPage-1)) div patListPatPerEMPage);
            Ppagesleft:=(patListCount+(patListPatPerEMPage-1)) div patListPatPerEMPage;
            patListUseEM:=true
          end;
      end;
    if useEMS and patListUseEM then
      begin
        curpart:=0;curPpage:=0;
      end;
    { clear all samples }
    fillchar(instruments^,MAX_INSTRUMENTS*sizeof(TInstr),0);
    { Now try to load everything in a row }
    {$IFDEF LOADINFO}
    writeln(#10#13'load report :');
    {$ENDIF}
    reset(f,1);
    fileposit:=0; { at start :) }
    Inst_done:=false;  { Instrument are not loaded yet :) }
    load_smp_later:=false; { load instruments not later (up to now we can say only this) }
    firstSMP:=true; { if we load now an instrument, then it's the first =) }
    { init buffer for fast loading : }
    if (_dos_allocmem((10*1024 + 15) shr 4, _seg) <> 0) then
    begin
        load_error:=notenoughmem;
        exit;
    end;
    buffer := ptr(_seg, 0);
    { init some variables for loading : }
    inspos:=1;patpos:=0;smppos:=0;smpnum:=0;nextpat:=$7fffffff;nextins:=$7fffffff;nextsmp:=$7fffffff;
    while (inspos<insnum+1) or (patpos<patListCount) or (smppos<smpnum)
          or (nextpat<$7fffffff) or (nextins<$7fffffff) or (nextsmp<$7fffffff) do
      begin
        {writeln('--->',inspos,',',patpos,',',smppos);readkey;}
        if (nextpat=$7fffffff) and (patpos<patListCount) then
          begin
            nextpat:=patpara[patpos];inc(patpos)
          end;
        if (nextins=$7fffffff) and (inspos<insnum+1) then
          begin
            nextins:=inspara[inspos];inc(inspos)
          end;
        if (nextsmp=$7fffffff) and (smppos<smpnum) then
          begin
            nextsmp:=smppara[smppos];inc(smppos)
          end;
        if (nextpat<nextins) and (nextpat<nextsmp) then
          begin
            { pattern }
            if (nextpat<$7fffffff) then
              if not load_decrunc_pattern then begin freeallmem;exit end;
            nextpat:=$7fffffff;
          end
        else
        if (nextins<nextsmp) then
          begin
            { instrument }
            if (nextins<$7fffffff) then
              if not load_instrument then begin freeallmem;exit end;
            nextins:=$7fffffff;inst_done:=(inspos=insnum+1);
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
                      if useEMS and firstSMP then begin allocEMSforSamples;firstSMP:=false end;
                      if (nextsmp<$7fffffff) then
                      if not load_sample then begin freeallmem;exit end;
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
        reset(f,1);
        fileposit:=0; { again to start }
        allocEMSforSamples;
        smppos:=0;smpnum:=0;nextpat:=$7fffffff;nextins:=$7fffffff;nextsmp:=$7fffffff;
        while (smppos<smpnum) or (nextsmp<$7fffffff) do
          begin
            if (nextsmp=$7fffffff) and (smppos<smpnum) then
              begin
                nextsmp:=smppara[smppos];inc(smppos)
              end;
            if (nextsmp<$7fffffff) then
              if not load_sample then begin freeallmem;exit end;
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
