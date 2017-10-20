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

type
    TS3MLoader = packed record
        f: File;
        buffer: PArray;
        inspara: array [0..MAX_INSTRUMENTS-1] of Word;
        patpara: array [0..MAX_PATTERNS-1] of Word;
        smppara: array [0..MAX_INSTRUMENTS-1] of LongInt;
        pat_EM_pages: Word;
        pat_EM_page: Word;
        pat_EM_page_offset: Word;
        smp_EM_pages: Word;
        smp_EM_page: Word;
    end;
    PS3MLoader = ^TS3MLoader;

function  s3mloader_new: PS3MLoader; forward;
procedure s3mloader_clear(self: PS3MLoader); forward;
procedure s3mloader_delete(var self: PS3MLoader); forward;
function  s3mloader_allocbuf(self: PS3MLoader): Boolean; forward;
function  s3mloader_load_pattern(self: PS3MLoader; index: Byte; pos: LongInt): Boolean; forward;
function  s3mloader_load_instrument(self: PS3MLoader; index: Byte; var smppara: LongInt; pos: LongInt): Boolean; forward;
procedure s3mloader_alloc_samples(self: PS3MLoader); forward;
function  s3mloader_load_sample(self: PS3MLoader; index: Byte; pos: LongInt): Boolean; forward;
procedure s3mloader_free(self: PS3MLoader); forward;

function s3mloader_new: PS3MLoader;
var
    _seg: Word;
begin
    if (_dos_allocmem(_dos_para(sizeof(TS3MLoader)), _seg) = 0) then
        s3mloader_new := ptr(_seg, 0)
    else
        s3mloader_new := nil;
end;

procedure s3mloader_clear(self: PS3MLoader);
begin
    if (self <> nil) then
        memset(self^, 0, sizeof(TS3MLoader));
end;

procedure s3mloader_delete(var self: PS3MLoader);
begin
    if (self <> nil) then
    begin
        _dos_freemem(seg(self^));
        self := nil;
    end;
end;

function s3mloader_allocbuf(self: PS3MLoader): Boolean;
var
    _seg: Word;
begin
    if (self <> nil) then
    begin
        if (_dos_allocmem(_dos_para(10*1024), _seg) = 0) then
        begin
            self^.buffer := ptr(_seg, 0);
            s3mloader_allocbuf := true;
            exit;
        end;
    end;

    s3mloader_allocbuf := false;
end;

procedure s3mloader_free(self: PS3MLoader);
begin
    if (self <> nil) then
    begin
        if (self^.buffer <> Nil) then
        begin
            _dos_freemem(seg(self^.buffer^));
            self^.buffer := nil;
        end;
        close(self^.f);
    end;
end;

function s3mloader_load_pattern(self: PS3MLoader; index: Byte; pos: LongInt): Boolean;
var
    length: Word;
    pat_static: TMUSPAT;
    pat: PMUSPAT;
    _seg: Word;
    p: Pointer;
begin
    s3mloader_load_pattern:=false;

      if (pos = 0) then
      begin
        pat := patList_get(mod_Patterns, index);
        muspat_clear(pat);
        s3mloader_load_pattern := true;
        exit;
      end;

      if (_fsetpos(self^.f, pos) <> 0) then
      begin
          Debug_Err(__FILE__, 's3mloader_load_pattern', 'Failed to read file.');
          load_error := filecorrupt;
          exit;
      end;
      if (_fread(@length, 2, 1, self^.f) <> 1) then
      begin
          Debug_Err(__FILE__, 's3mloader_load_pattern', 'Failed to read pattern.');
          load_error := filecorrupt;
          exit;
      end;
      {$IFDEF LOADINFO}
      write('P',index,'(',length,')');
      {$ENDIF}
      if (length = 0) or (length > 10*1024) then
      begin
          Debug_Err(__FILE__, 's3mloader_load_pattern', 'Bad pattern size.');
          load_error := filecorrupt;
          exit;
      end;
      { read whole packed pattern }
      if (_fread(self^.buffer, length - 2, 1, self^.f) <> 1) then
      begin
          Debug_Err(__FILE__, 's3mloader_load_pattern', 'Failed to read pattern.');
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
        (self^.pat_EM_page_offset < patListGetPatPerPage(mod_Patterns)) then
        begin
          muspat_set_EM_data(pat, true);
          muspat_set_own_EM_handle(pat, false);
          muspat_set_EM_data_handle(pat, patListGetHandle(mod_Patterns));
          muspat_set_EM_data_page(pat, self^.pat_EM_page);
          muspat_set_EM_data_offset(pat, self^.pat_EM_page_offset * 5 * UsedChannels * 64);
          patList_set(mod_Patterns, index, pat);
          p := muspat_map_EM_data(pat);
          if (p = nil) then
          begin
              Debug_Err(__FILE__, 's3mloader_load_pattern', 'Failed to map EM for pattern.');
              load_error := internal_failure;
              exit;
          end;
        end
      else
        begin
            if (_dos_allocmem(_dos_para(longint(64*5)*usedchannels + 15), _seg) <> 0) then
            begin
                Debug_Err(__FILE__, 's3mloader_load_pattern', 'Failed to allocate DOS memory for pattern.');
                load_error := notenoughmem;
                exit;
            end;
            p := ptr(_seg, 0);
            muspat_set_EM_data(pat, false);
            muspat_set_data(pat, p);
            patList_set(mod_Patterns, index, pat);
        end;

      unpackPattern(self^.buffer, p, 64, UsedChannels);

      pat := patList_get(mod_Patterns, index);
      if (muspat_is_EM_data(pat)) then
        begin
          {$IFDEF LOADINFO}
          write('E(',self^.pat_EM_page,',',self^.pat_EM_page_offset,')');
          {$ENDIF}
          { next position in EMS : }
          inc(self^.pat_EM_page_offset);
          if (self^.pat_EM_page_offset = patListGetPatPerPage(mod_Patterns)) and (self^.pat_EM_pages>0) then
            begin
              dec(self^.pat_EM_pages);
              inc(self^.pat_EM_page);
              self^.pat_EM_page_offset:=0;
            end;
        end;
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}

    s3mloader_load_pattern:=true;
end;

function s3mloader_load_instrument(self: PS3MLoader; index: Byte; var smppara: LongInt; pos: LongInt): Boolean;
var
    length:word;
    typ:byte;
    pAr:Parray;
    Psmp:PSmpHeader;
begin
      s3mloader_load_instrument:=false;
      if (_fsetpos(self^.f, pos) <> 0) then
      begin
          Debug_Err(__FILE__, 's3mloader_load_instrument', 'Failed to read file.');
          load_error := filecorrupt;
          exit;
      end;
      {$IFDEF LOADINFO}
      write('I',index);
      {$ENDIF}
      { now read instrument header : }
      pSmp := @(mod_Instruments^[index]);
      if (_fread(pSmp, sizeof(TSmpHeader), 1, self^.f) <> 1) then
      begin
          Debug_Err(__FILE__, 's3mloader_load_instrument', 'Failed to read instrument header.');
          load_error := filecorrupt;
          exit;
      end;
      if pSmp^.typ=1 then { that instrument is a sample }
        begin
          if pSmp^.packinfo <> 0 then
          begin
              Debug_Err(__FILE__, 's3mloader_load_instrument', 'Packed sample is not supported.');
              load_error := packedsamples;
              exit;
          end;
          { calc position in file : }
          smppara:=(longint(256*256)*pSmp^.HI_mempos+pSmp^.mempos);
          pSmp^.mempos:=0;
          {$IFDEF LOADINFO}
          write('!');
          {$ENDIF}
        end
      else
        begin
          smppara:=0;
          {$IFDEF LOADINFO}
          write('$');
          {$ENDIF}
        end;
      {$IFDEF LOADINFO}
      write('*');
      {$ENDIF}
      s3mloader_load_instrument:=true;
end;

procedure s3mloader_alloc_samples(self: PS3MLoader);
var
    w,w0:word;
    i:integer;
    pSmp:PSMPheader;
begin
    if emsGetFreePagesCount=0 then
    begin
        EMSsmp:=false;
        exit;
    end;
    w:=0;
    for i:=0 to MAX_INSTRUMENTS-1 do
    begin
        pSmp := @(mod_Instruments^[i]);
        if pSmp^.typ=1 then { really a sample }
        begin
            if (pSmp^.flags and 1 <> 0) then
                w0:=pSmp^.loopend
            else
                w0:=pSmp^.length;
            w0 := w0 + 1024;
            w := w + w0 div (16*1024) + ord(w0 mod (16*1024)>0);
        end;
    end;
    {$IFDEF DEBUGLOAD}
    writeln(' Instruments to load : ',insnum);
    writeln(' EMS pages are needed for Samples : ',w);
    {$ENDIF}
    { w = number of 16Kb pages in EMS }
    if (w > emsGetFreePagesCount) then
        w := emsGetFreePagesCount;
    smpEMShandle := emsAlloc(w);
    {$IFDEF DEBUGLOAD}
    writeln(' EMS pages allocated for Samples : ',w);
    {$ENDIF}
    self^.smp_EM_pages:=w;
    self^.smp_EM_page:=0;
    EMSsmp:=true;
end;

function s3mloader_load_sample(self: PS3MLoader; index: Byte; pos: LongInt): Boolean;
var
      par:parray;
      pSmp:pSmpHeader;
      z,h:word;
      i:byte;
      smplen:word;
      _seg: word;
begin
    s3mloader_load_sample := false;

      if (_fsetpos(self^.f, pos) <> 0) then
      begin
          Debug_Err(__FILE__, 's3mloader_load_sample', 'Failed to read file.');
          load_error := filecorrupt;
          exit;
      end;
      pSmp := @(mod_Instruments^[index]);
      if (pSmp^.flags and 1)=1 then smplen:=pSmp^.loopend else smplen:=pSmp^.length;
      if smplen>64511 then
      begin
          Debug_Err(__FILE__, 's3mloader_load_sample', 'Sample too large.');
          load_error := sample2large;
          exit;
      end;
      {$IFDEF LOADINFO}
      write('S',index,'(',smplen,')');
      {$ENDIF}
      z:=((smplen+1024) div (16*1024))+ord((smplen+1024) mod (16*1024)>0);
      if useEMS and EMSsmp and (self^.smp_EM_pages>=z) then
        begin
          {$IFDEF LOADINFO}
          write('E(',self^.smp_EM_page,'-',self^.smp_EM_page+z-1,')');
          {$ENDIF}
          pSmp^.mempos:=$f000+self^.smp_EM_page; { and z-1 pages after }
          for i:=0 to z-1 do
            if not emsMap(smpEMShandle,self^.smp_EM_page+i,i) then write('<EMS-ERROR>');
          inc(self^.smp_EM_page,z);
          par := emsFramePtr;
        end
      else { we have to use normal memory (geeee) for this sample }
        begin
            if (_dos_allocmem((smplen+1024+15) shr 4, _seg) <> 0) then
            begin
                Debug_Err(__FILE__, 's3mloader_load_sample', 'Failed to allocate DOS memory for sample data.');
                load_error := notenoughmem;
                exit;
            end;
            par := ptr(_seg, 0);;
            pSmp^.mempos:=seg(par^);
        end;
        if (_fread(par, smplen, 1, self^.f) <> 1) then
        begin
            Debug_Err(__FILE__, 's3mloader_load_sample', 'Failed to read sample data.');
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

    s3mloader_load_sample:=true;
end;

function s3mloader_load(self: PS3MLoader; name: String): Boolean;
var
    header:Theader;
    maxused:byte;
    i,smpnum:byte;
    p:pointer;
    pAr:PArray;
    chn: PMIXCHN;

  BEGIN
    s3mloader_load := FALSE;

    { clear all samples }
    fillchar(mod_Instruments^,MAX_INSTRUMENTS*sizeof(TInstr),0);

    (* Patterns list *)
    mod_Patterns := patList_new;
    if (mod_Patterns = nil) then
    begin
        DEBUG_ERR(__FILE__, 's3mloader_load', 'Failed to initialize patterns.');
        exit;
    end;
    patList_clear(mod_Patterns);

    useEMS:=emsInstalled and useEMS and (emsGetFreePagesCount>1);
    { we need one page for saving mapping while playing }
    load_error := 0;
    assign(self^.f,name);
    reset(self^.f,1);               { open file - 16byte blocks :) }
    IF IORESULT<>0 then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to open file.');
        load_error := filenotexist;
        exit;
    end;

    if (_fread(@header, sizeof(THeader), 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to read file''s header.');
        load_error := wrongformat;
        exit;
    end;

    if (header.filetyp <> 16)
    or (header.SCRM_ID <> $4d524353)
    or ((header.CWTV shr 8) and $ff <> $13)
    or (not (header.ffv in [1, 2])) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Unsupported file format.');
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
        chn := @channel[i];
        if ((header.channelset[i] and 128) = 0) then
            mixchn_set_flags(chn, MIXCHNFL_ENABLED + MIXCHNFL_MIXING)
        else
            mixchn_set_flags(chn, 0);
        mixchn_set_type(chn, getchtyp(header.channelset[i] and 31));
        if  (mixchn_is_enabled(chn))
        and (mixchn_get_type(chn) > 0)
        and (mixchn_get_type(chn) < 3) then
            maxused := i + 1;
    end;
    usedchannels:=maxused;
    {$IFDEF DEBUGLOAD}
    writeln(' Used channels :',usedchannels);
    {$ENDIF}
    { now load arrangment : }
    if (_fread(@Order, ordnum, 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to read patterns order.');
        load_error := filecorrupt;
        exit;
    end;
    { check order if there's one 'real' (playable) entry ... }
    i:=0;
    while (i<ordnum) and (order[i]>=254) do inc(i);
    if i=ordnum then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Playable entry not found.');
        load_error := ordercorrupt;
        exit;
    end;
    if (_fread(@self^.inspara, insnum * 2, 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to read instruments headers.');
        load_error := filecorrupt;
        exit;
    end;

    if (_fread(@self^.patpara, patList_get_count(mod_Patterns) * 2, 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to read patterns offsets.');
        load_error := filecorrupt;
        exit;
    end;

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
        self^.pat_EM_pages :=
            (patList_get_count(mod_Patterns)+
            (patListGetPatPerPage(mod_Patterns)-1)) div
             patListGetPatPerPage(mod_Patterns);

        if (self^.pat_EM_pages > emsGetFreePagesCount) then
            self^.pat_EM_pages := emsGetFreePagesCount;

        patListSetUseEM(mod_Patterns, true);
        patListSetHandle(mod_Patterns, emsAlloc(self^.pat_EM_pages));

        self^.pat_EM_page_offset := 0;
        self^.pat_EM_page := 0;
    end;

    {$IFDEF LOADINFO}
    writeln(#10#13'load report :');
    {$ENDIF}

    if (not s3mloader_allocbuf(self)) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to allocate DOS memory for buffer.');
        load_error := notenoughmem;
        s3mloader_load := false;
        exit;
    end;

    for i := 0 to patList_get_count(mod_Patterns) - 1 do
    begin
        if (not s3mloader_load_pattern(self, i, self^.patpara[i] * 16)) then
        begin
            s3mloader_load := false;
            exit;
        end;
    end;

    for i := 0 to insnum-1 do
    begin
        if (not s3mloader_load_instrument(self, i, self^.smppara[i], self^.inspara[i] * 16)) then
        begin
            s3mloader_load := false;
            exit;
        end;
    end;

    if (UseEMS) then
        s3mloader_alloc_samples(self);

    for i := 0 to insnum-1 do
    begin
        if (self^.smppara[i] <> 0) then
            if (not s3mloader_load_sample(self, i, self^.smppara[i] * 16)) then
            begin
                s3mloader_load := false;
                exit;
            end;
    end;

    {$IFDEF DEBUGLOAD}
    writeln(#10);
    {$ENDIF}
    { Just for fun set names for EMS handles (does only work for EMS>= v4.0) }
    if ( emsVersion.Hi >= 4 ) then setEMSnames;
    mod_isLoaded := true;
    s3mloader_load := true;
end;

function player_load_s3m(name: String): Boolean;
var
    p: PS3MLoader;
begin
    p := s3mloader_new;
    if (p = nil) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to initialize S3M loader.');
        load_error := notenoughmem;
        player_load_s3m := false;
        exit;
    end;
    s3mloader_clear(p);

    if (not s3mloader_load(p, name)) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to load S3M file.');
        s3mloader_free(p);
        s3mloader_delete(p);
        player_free_module;
        player_load_s3m := false;
        exit;
    end;

    s3mloader_free(p);
    s3mloader_delete(p);

    player_load_s3m := true;
end;
