#include <p16f886.inc>

    
    GLOBAL TEMP1
    GLOBAL TEMP2
    GLOBAL TEMP3

    GLOBAL DELAY_1SEC
    GLOBAL DELAY_5DEC
    
    GLOBAL DELAY_
    
;*******************************************************************************
;    VARIABLES                                                                 *
;*******************************************************************************
    
GRP_MISC    UDATA_SHR
TEMP1	    RES 1
TEMP2	    RES 1
TEMP3	    RES 1

	    
;*******************************************************************************
;    SUBROUTINES                                                               *
;*******************************************************************************
MISC CODE
    
; 1 second delay
DELAY_1SEC
    movlw   d'25'
    movwf   TEMP3

DELAY_1SEC_RESET2    
    movlw   0xFF
    movwf   TEMP2

DELAY_1SEC_RESET1    
    movlw   0xFF
    movwf   TEMP1

    decfsz  TEMP1
    goto    $-1
    clrwdt

    decfsz  TEMP2
    goto    DELAY_1SEC_RESET1
    
    decfsz  TEMP3
    goto    DELAY_1SEC_RESET2
    return

; 0.5 second delay
DELAY_5DEC
    movlw   d'25'
    movwf   TEMP3

DELAY_5DEC_RESET2    
    movlw   0xFF
    movwf   TEMP2

DELAY_5DEC_RESET1
    movlw   0x40
    movwf   TEMP1

    decfsz  TEMP1
    goto    $-1
    clrwdt

    decfsz  TEMP2
    goto    DELAY_5DEC_RESET1
    
    decfsz  TEMP3
    goto    DELAY_5DEC_RESET2
    return

    
DELAY_
    banksel PORTC
    bsf	    PORTC, 2

    movlw   0x2
    movwf   TEMP1
    decfsz  TEMP1
    goto    $-1    

    bcf	    PORTC, 2
    return

    END
