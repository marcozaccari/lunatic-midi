    EXTERN LEDS_INIT
    EXTERN LEDS_RESET
    EXTERN LEDS_SET
    EXTERN LEDS_SET_VALUE
    

;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

; initialize leds
leds_init macro
    call	LEDS_INIT
    endm

leds_set macro
    call	LEDS_SET
    endm

leds_set_value_w macro
    call	LEDS_SET_VALUE
    endm
    
leds_reset macro
    call	LEDS_RESET
    endm