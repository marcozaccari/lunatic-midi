    EXTERN KEYB_INIT
    EXTERN KEYB_SCAN_SWITCH_ON
    EXTERN KEYB_SCAN_SWITCH_ONLONG
    

;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

; initialize keyboard
keyb_init macro
    call	KEYB_INIT
    endm

keyb_scan_on macro
    call	KEYB_SCAN_SWITCH_ON
    endm

keyb_scan_onlong macro
    call	KEYB_SCAN_SWITCH_ONLONG
    endm
