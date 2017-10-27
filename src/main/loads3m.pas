(* loads3m.pas -- Pascal declarations for loads3m.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit loads3m;

interface

uses
    types,
    s3mtypes,
    musins,
    muspat;

(*$I defines.pas*)

const
    S3M_MAX_PATTERNS = 100;

type
    TS3MLoader = packed record
        err: Integer;
        f: File;
        buffer: PArray;
        inspara: array [0..MAX_INSTRUMENTS-1] of Word;
        patpara: array [0..S3M_MAX_PATTERNS-1] of Word;
        smppara: array [0..MAX_INSTRUMENTS-1] of LongInt;
        pat_EM_pages: Word;
        pat_EM_page: Word;
        pat_EM_page_offset: Word;
        smp_EM_pages: Word;
        smp_EM_page: Word;
    end;
    PS3MLoader = ^TS3MLoader;

function  s3mloader_new: PS3MLoader;
procedure s3mloader_clear(self: PS3MLoader);
procedure s3mloader_delete(var self: PS3MLoader);
function  s3mloader_allocbuf(self: PS3MLoader): Boolean;
function  s3mloader_load_pattern(self: PS3MLoader; index: Byte; pos: LongInt): Boolean;
procedure s3mloader_free(self: PS3MLoader);
procedure unpackPattern(src, dst: parray; maxrow, maxchn: byte);
function  s3mloader_load_instrument(self: PS3MLoader; index: Byte; var smppara: LongInt; pos: LongInt): Boolean;
procedure s3mloader_alloc_samples(self: PS3MLoader);
function  s3mloader_load_sample(self: PS3MLoader; index: Byte; pos: LongInt): Boolean;
function  s3mloader_load(self: PS3MLoader; name: String): Boolean;

var
    load_error: Integer;

implementation

uses
    stdio,
    string_,
    debug,
    strutils,
    crt,
    dos,
    dos_,
    ems,
    s3mvars,
    mixchn;

const
    __FILE__ = 'loads3m.pas';

const
    noerror             = 0;
    notenoughmem        = -1;
    wrongformat         = -2;
    filecorrupt         = -3;
    filenotexist        = -4;
    packedsamples       = -5;
    ordercorrupt        = -10;
    internal_failure    = -11;
    sample2large        = -12;

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

(*$l loads3m.obj*)

function  s3mloader_new: PS3MLoader; external;
procedure s3mloader_clear(self: PS3MLoader); external;
procedure s3mloader_delete(var self: PS3MLoader); external;
function  s3mloader_allocbuf(self: PS3MLoader): Boolean; external;
function  s3mloader_load_pattern(self: PS3MLoader; index: Byte; pos: LongInt): Boolean; external;
procedure s3mloader_free(self: PS3MLoader); external;
procedure unpackPattern(src, dst: parray; maxrow, maxchn: byte); external;


function s3mloader_load_instrument(self: PS3MLoader; index: Byte; var smppara: LongInt; pos: LongInt): Boolean;
var
    length:word;
    typ:byte;
    pAr:Parray;
    ins: PMUSINS;
    smp: TSmpHeader;
begin
    if (_fsetpos(self^.f, pos) <> 0) then
    begin
        Debug_Err(__FILE__, 's3mloader_load_instrument', 'Failed to read file.');
        load_error := filecorrupt;
        s3mloader_load_instrument:=false;
        exit;
    end;

    {$IFDEF LOADINFO}
    write('I',index);
    {$ENDIF}

    ins := musinsl_get(mod_Instruments, index);
    musins_clear(ins);

    if (_fread(@smp, sizeof(TSmpHeader), 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load_instrument', 'Failed to read instrument header.');
        load_error := filecorrupt;
        s3mloader_load_instrument:=false;
        exit;
    end;

    if (smp.typ = 1) then
    begin
        if (smp.packinfo <> 0) then
        begin
            Debug_Err(__FILE__, 's3mloader_load_instrument', 'Packed sample is not supported.');
            load_error := packedsamples;
            s3mloader_load_instrument:=false;
            exit;
        end;
        musins_set_type(ins, MUSINST_PCM);
        musins_set_length(ins, smp.length);
        if (smp.flags and 1 <> 0) then
        begin
            musins_set_looped(ins, true);
            musins_set_loop_start(ins, smp.loopbeg);
            musins_set_loop_end(ins, smp.loopend);
        end;
        musins_set_volume(ins, smp.vol);
        musins_set_rate(ins, smp.c2speed);
        musins_set_title(ins, smp.name);
        smppara := (longint(256*256) * smp.HI_mempos + smp.mempos);
        {$IFDEF LOADINFO}
        write('!');
        {$ENDIF}
    end
    else
    begin
        musins_set_type(ins, MUSINST_EMPTY);
        musins_set_title(ins, smp.name);
        smppara:=0;
        {$IFDEF LOADINFO}
        write('$');
        {$ENDIF}
    end;

    s3mloader_load_instrument := true;
end;

procedure s3mloader_alloc_samples(self: PS3MLoader);
var
    w,w0:word;
    i:integer;
    ins: PMUSINS;
    handle: TEMSHDL;
begin
    if emsGetFreePagesCount=0 then
    begin
        DEBUG_WARN(__FILE__, 's3mloader_alloc_samples', 'Not enough EM for samples.');
        musinsl_set_EM_data(mod_Instruments, false);
        exit;
    end;

    w:=0;
    for i:=0 to MAX_INSTRUMENTS-1 do
    begin
        ins := musinsl_get(mod_Instruments, i);
        if (musins_get_type(ins) = MUSINST_PCM) then
        begin
            if (musins_is_looped(ins)) then
                w0 := musins_get_loop_end(ins)
            else
                w0 := musins_get_length(ins);
            w0 := w0 + 1024;
            w := w + w0 div (16*1024) + ord(w0 mod (16*1024)>0);
        end;
    end;

    {$IFDEF DEBUGLOAD}
    writeln(' Instruments to load : ',insnum);
    writeln(' EMS pages are needed for Samples : ',w);
    {$ENDIF}

    if (w > emsGetFreePagesCount) then
        w := emsGetFreePagesCount;

    handle := emsAlloc(w);
    if (emsEC <> E_EMS_SUCCESS) then
    begin
        DEBUG_ERR(__FILE__, 's3mloader_alloc_samples', 'Failed to allocate EM for samples');
        musinsl_set_EM_data(mod_Instruments, false);
        exit;
    end;
    musinsl_set_EM_data(mod_Instruments, true);
    musinsl_set_EM_data_handle(mod_Instruments, handle);

    self^.smp_EM_pages:=w;
    self^.smp_EM_page:=0;

    {$IFDEF DEBUGLOAD}
    writeln(' EMS pages allocated for Samples : ',w);
    {$ENDIF}
end;

function s3mloader_load_sample(self: PS3MLoader; index: Byte; pos: LongInt): Boolean;
var
      par:parray;
      ins: PMUSINS;
      z,h,dh: Word;
      i:byte;
      smplen:word;
      _seg: word;
      loopstart: Pointer;
      loopsize: Word;
begin
    s3mloader_load_sample := false;

    if (_fsetpos(self^.f, pos) <> 0) then
    begin
        Debug_Err(__FILE__, 's3mloader_load_sample', 'Failed to read file.');
        load_error := filecorrupt;
        exit;
    end;

    ins := musinsl_get(mod_Instruments, index);

    if (musins_is_looped(ins)) then
        smplen := musins_get_loop_end(ins)
    else
        smplen := musins_get_length(ins);

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

    if useEMS and musinsl_is_EM_data(mod_Instruments) and (self^.smp_EM_pages>=z) then
        begin
          {$IFDEF LOADINFO}
          write('E(',self^.smp_EM_page,'-',self^.smp_EM_page+z-1,')');
          {$ENDIF}
          musins_set_EM_data(ins, true);
          musins_set_EM_data_page(ins, self^.smp_EM_page);
          for i:=0 to z-1 do
            if not emsMap(smpEMShandle,self^.smp_EM_page+i,i) then write('<EMS-ERROR>');
          inc(self^.smp_EM_page,z);
          par := emsFramePtr;
        end
      else { we have to use normal memory (geeee) for this sample }
        begin
            if (_dos_allocmem(_dos_para(smplen+1024), _seg) <> 0) then
            begin
                Debug_Err(__FILE__, 's3mloader_load_sample', 'Failed to allocate DOS memory for sample data.');
                load_error := notenoughmem;
                exit;
            end;
            par := ptr(_seg, 0);
            musins_set_EM_data(ins, false);
            musins_set_data(ins, par);
        end;
        if (_fread(par, smplen, 1, self^.f) <> 1) then
        begin
            Debug_Err(__FILE__, 's3mloader_load_sample', 'Failed to read sample data.');
            load_error := filecorrupt;
            exit;
        end;

    if (musins_is_looped(ins)) then
    begin
        h := 1024;
        loopstart := @(par^[musins_get_loop_start(ins)]);
        loopsize := musins_get_loop_end(ins) - musins_get_loop_start(ins);
        while (h > 0) do
        begin
            if (h > loopsize) then
                dh := loopsize
            else
                dh := h;
            memcpy(par^[smplen + 1024 - h], loopstart^, dh);
            dec(h, dh);
        end;
    end
    else
        fillchar(par^[smplen], 1024, 128);

    {$IFDEF LOADINFO}
    write('*');
    {$ENDIF}

    s3mloader_load_sample:=true;
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

function s3mloader_load(self: PS3MLoader; name: String): Boolean;
var
    header:Theader;
    maxused:byte;
    i,smpnum:byte;
    p:pointer;
    pAr:PArray;
    chn: PMIXCHN;
    patsize: Word;
    patperpage: Word;

begin
    mod_Instruments := musinsl_new;
    if (mod_Instruments = nil) then
    begin
        DEBUG_ERR(__FILE__, 's3mloader_load', 'Failed to initialize instruments.');
        s3mloader_load := false;
        exit;
    end;
    musinsl_clear(mod_Instruments);

    mod_Patterns := muspatl_new;
    if (mod_Patterns = nil) then
    begin
        DEBUG_ERR(__FILE__, 's3mloader_load', 'Failed to initialize patterns.');
        s3mloader_load := false;
        exit;
    end;
    muspatl_clear(mod_Patterns);

    useEMS:=emsInstalled and useEMS and (emsGetFreePagesCount>1);
    { we need one page for saving mapping while playing }
    load_error := 0;
    assign(self^.f,name);
    reset(self^.f,1);               { open file - 16byte blocks :) }
    IF IORESULT<>0 then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to open file.');
        load_error := filenotexist;
        s3mloader_load := false;
        exit;
    end;

    if (_fread(@header, sizeof(THeader), 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to read file''s header.');
        load_error := wrongformat;
        s3mloader_load := false;
        exit;
    end;

    if (header.filetyp <> 16)
    or (header.SCRM_ID <> $4d524353)
    or ((header.CWTV shr 8) and $ff <> $13)
    or (not (header.ffv in [1, 2])) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Unsupported file format.');
        load_error := wrongformat;
        s3mloader_load := false;
        exit;
    end;

    { set some variables : }
    convert2pas(header.name,mod_Title,28);
    ordnum:=header.ordnum;
    insnum:=header.insnum;
    muspatl_set_count(mod_Patterns, header.patnum);
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
        s3mloader_load := false;
        exit;
    end;
    { check order if there's one 'real' (playable) entry ... }
    i:=0;
    while (i<ordnum) and (order[i]>=254) do inc(i);
    if i=ordnum then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Playable entry not found.');
        load_error := ordercorrupt;
        s3mloader_load := false;
        exit;
    end;
    if (_fread(@self^.inspara, insnum * 2, 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to read instruments headers.');
        load_error := filecorrupt;
        s3mloader_load := false;
        exit;
    end;

    if (_fread(@self^.patpara, muspatl_get_count(mod_Patterns) * 2, 1, self^.f) <> 1) then
    begin
        Debug_Err(__FILE__, 's3mloader_load', 'Failed to read patterns offsets.');
        load_error := filecorrupt;
        s3mloader_load := false;
        exit;
    end;

    patsize := UsedChannels * 64 * 5;
    {$IFDEF DEBUGLOAD}
    writeln('Pattern memory size: ', patsize);
    {$ENDIF}

    if useEMS then
    begin
        { let's continue with loading: }
        patperpage := 16 * 1024 div patsize;
        {$IFDEF DEBUGLOAD}
        writeln('Patterns per EM page: ', patperpage);
        {$ENDIF}
        self^.pat_EM_pages := (muspatl_get_count(mod_Patterns) + patperpage - 1) div patperpage;

        if (self^.pat_EM_pages > emsGetFreePagesCount) then
            self^.pat_EM_pages := emsGetFreePagesCount;

        muspatl_set_EM_data(mod_Patterns, true);
        muspatl_set_own_EM_handle(mod_Patterns, true);
        muspatl_set_EM_handle(mod_Patterns, emsAlloc(self^.pat_EM_pages));

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

    for i := 0 to muspatl_get_count(mod_Patterns) - 1 do
    begin
        if (not s3mloader_load_pattern(self, i, self^.patpara[i] * 16)) then
        begin
            load_error := self^.err;
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

    if (musinsl_is_EM_data(mod_Instruments)) then
        musinsl_set_EM_handle_name(mod_Instruments);
    if (muspatl_is_EM_data(mod_Patterns)) then
        muspatl_set_EM_handle_name(mod_Patterns);

    mod_isLoaded := true;
    s3mloader_load := true;
end;

end.
