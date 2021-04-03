    EXTERN BUTTONS_INIT
    EXTERN BUTTONS_SCAN
    

;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

; initialize buttons
buttons_init macro
    call	BUTTONS_INIT
    endm

buttons_scan macro
    call	BUTTONS_SCAN
    endm
