; chkio.asm -- part of custom "startup" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large

include "startup.def"

_DATA segment word public use16 'DATA'
_DATA ends

DGROUP group _DATA

CHKIO_TEXT segment word public use16 'CODE'
assume cs:CHKIO_TEXT,ds:DGROUP

public _cc_CheckInOutRes
_cc_CheckInOutRes:
    ; Stack:
    ;   SS:[SP+0] (dword) = void __far *return_addr
    cmp     cc_InOutRes,0
    jne     short @error
    retf
@error:
    push    cc_InOutRes
    ; Stack:
    ;   SS:[SP+2] (dword) = void __far *addr
    ;   SS:[SP+0] (word)  = int status
    call    far ptr _cc_ExitWithError

CHKIO_TEXT ends

end
