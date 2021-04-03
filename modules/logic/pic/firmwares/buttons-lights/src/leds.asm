#include <p16f886.inc>
#include "main.inc"
#include "lib/misc.inc"

    GLOBAL  LEDS_INIT
    GLOBAL  LEDS_RESET
    GLOBAL  LEDS_SET
    GLOBAL  LEDS_SET_VALUE
    
    EXTERN  GLOBAL_FLAGS
    

;*******************************************************************************
;    VARIABLES                                                                 *
;*******************************************************************************

GRP_LEDS		UDATA
; indices
LEDS_CURBANK		RES 1	; bank index (0..LEDS_BANKS)
LEDS_CURBANK_COUNTER	RES 1	; bank counter index (BUTTONS_BANKS..1)
    
GRP_LEDS_BUFFERS	UDATA
; port status (previous and current)		
LEDS_BITS		RES d'16'  ; raw bits status
    
;*******************************************************************************
;    SUBROUTINES                                                               *
;*******************************************************************************
LEDS CODE

;*******************************************
; Initialize ports and arrays (all leds on)
LEDS_INIT
    ; flush arrays
    movlw   d'16'
    movwf   LEDS_CURBANK_COUNTER
    clrf    LEDS_CURBANK
    movlw   0xFF
    movwf   TEMP1
LedsInitFlushBits:    
    set_array LEDS_BITS, LEDS_CURBANK, TEMP1
    
    incf    LEDS_CURBANK
    decfsz  LEDS_CURBANK_COUNTER
    goto    LedsInitFlushBits
    
    call    LEDS_SET
    return
    

;*******************************************
; All leds off 
LEDS_RESET    
    ; flush arrays
    movlw   d'16'
    movwf   LEDS_CURBANK_COUNTER
    clrf    LEDS_CURBANK
    clrf    TEMP1
LedsResetFlushBits:    
    set_array LEDS_BITS, LEDS_CURBANK, TEMP1
    
    incf    LEDS_CURBANK
    decfsz  LEDS_CURBANK_COUNTER
    goto    LedsResetFlushBits
    
    call    LEDS_SET
    return
    
    
;*******************************************
; Set all leds   
;
LEDS_SET
    banksel PORTA
    movlw   b'00010000'		; enable leds / disable buttons
    iorlw   b'00100000'		; LEDS latch high
    movwf   PORTA
    banksel  TRISB
    clrf    TRISB		; PortB as output
    
    ; prepare indirect address
    movlw   LEDS_BITS
    movwf   FSR
    bankisel LEDS_BITS

    ; prepare bank counter
    banksel  LEDS_CURBANK
    movlw   LEDS_BANKS
    movwf   LEDS_CURBANK_COUNTER
    clrf    LEDS_CURBANK

LedBitsBank:

    banksel LEDS_CURBANK
    movf    LEDS_CURBANK, w
    iorlw   b'00010000'		; enable leds / disable buttons, LEDS latch low
    
    ; output address to PORTA
    banksel PORTA
    movwf   PORTA
    ;bcf	    PORTA, 5	    ; latch off
    
    ; get bits status
    movf    INDF, w

    #ifdef PROGRAMMER
	andlw	b'00111111'  ; remove upper 2 bits (used by programmer)
    #endif

    ; set status to PORTB
;movlw   b'00000111'
    movwf    PORTB
    comf    PORTB
    
;    bcf	    PORTA, 5	    ; latch off

    ; wait 2us
    nop
    nop
    nop
    nop
    nop

    nop
    nop
    nop
    nop
    nop

    bsf	    PORTA, 5	    ; latch on
    
    ; wait 2us
    nop
    nop
    nop
    nop
    nop

    nop
    nop
    nop
    nop
    nop

    incf    FSR		    ; next LEDS_BITS offset

    banksel  LEDS_CURBANK
    incf    LEDS_CURBANK
    decfsz  LEDS_CURBANK_COUNTER
    goto    LedBitsBank
    
    banksel TRISB
    movlw   0xFF
    movwf   TRISB	    ; PORTB as input               
    banksel PORTA
    bcf	    PORTA, 4	    ; disable led / enable buttons
    return

    
;*******************************************
; Set single led value
; w = siiiiiii where:
;    i = Led index (0..127)
;    s = state (0 = OFF; 1 = ON)
;
LEDS_SET_VALUE
    banksel LEDS_CURBANK
    movwf   TEMP3
    movwf   LEDS_CURBANK
   
    ; LEDS_CURBANK = w(7bits) div 8
    bcf	    LEDS_CURBANK, 7
    bcf	    STATUS, C	 
    rrf	    LEDS_CURBANK
    bcf	    STATUS, C
    rrf	    LEDS_CURBANK
    bcf	    STATUS, C
    rrf	    LEDS_CURBANK

    ; TEMP1 = led bit value
    movlw   1
    movwf   TEMP1
    btfss   TEMP3, 2
    goto    SetLedBit1 
    
    bcf	    STATUS, C
    rlf	    TEMP1
    bcf	    STATUS, C
    rlf	    TEMP1
    bcf	    STATUS, C
    rlf	    TEMP1
    bcf	    STATUS, C
    rlf	    TEMP1
    
SetLedBit1:   
    btfss   TEMP3, 1
    goto    SetLedBit0 
    bcf	    STATUS, C
    rlf	    TEMP1
    bcf	    STATUS, C
    rlf	    TEMP1
    
SetLedBit0:   
    bcf	    STATUS, C
    btfsc   TEMP3, 0
    rlf	    TEMP1

    btfss   TEMP3, 7
    goto    SetLedInverted
  
    get_array LEDS_BITS, LEDS_CURBANK
    iorwf   TEMP1
    set_array LEDS_BITS, LEDS_CURBANK, TEMP1
    goto SetLedEnd

SetLedInverted:    
    comf    TEMP1
    get_array LEDS_BITS, LEDS_CURBANK
    andwf   TEMP1
    set_array LEDS_BITS, LEDS_CURBANK, TEMP1
    
SetLedEnd:    
    goto    LEDS_SET
    return
    
    END