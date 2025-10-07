    EXTERN TEMP1
    EXTERN TEMP2
    EXTERN TEMP3
    
    EXTERN DELAY_1SEC
    EXTERN DELAY_5DEC
    
    EXTERN DELAY_

    EXTERN LED_TX_COUNTER
	
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

set_array   macro   address, address_offset, address_value
    bankisel address
    movlw   address
    movwf   FSR
    movf    address_offset, w
    addwf   FSR
    
    movf    address_value, w
    movwf   INDF
    endm

	; value to w
get_array   macro   address, address_offset
    bankisel address
    movlw   address
    movwf   FSR
    movf    address_offset, w
    addwf   FSR
    movf    INDF, w
    endm	

led_on   macro
    banksel PORTC
    bsf	    PORTC, 2
    endm

led_off   macro
    banksel PORTC
    bcf	    PORTC, 2
    endm
    
led_debug_on   macro
    #ifdef PROGRAMMER
    banksel PORTC
    bsf	    PORTC, 2
    #endif
    endm

led_debug_off   macro
    #ifdef PROGRAMMER
    banksel PORTC
    bcf	    PORTC, 2
    #endif
    endm

led_tx_on macro   
    #ifndef PROGRAMMER
    movlw   b'00011111'
    banksel  LED_TX_COUNTER
    movwf   LED_TX_COUNTER
    banksel PORTC
    bcf	    PORTC, 2
    #endif
    endm

; to be called at every ISR timer tick     
led_tx_isr macro
    #ifndef PROGRAMMER
    banksel LED_TX_COUNTER
    decf    LED_TX_COUNTER
    btfsc   STATUS, Z
    bsf	    PORTC, 2
    #endif
    endm
    
    
delay_1sec macro	    
    call    DELAY_1SEC
    endm  

delay_5dec macro	    
    call    DELAY_5DEC
    endm  

; 16-bit Subtraction with Borrow
; dest = dest - source, WITH VALID CARRY (although the Z flag is not valid)
; Input:
;       DestH:DestL = Number to be subtracted FROM
;       SourceH:SourceL = Number to be subtracted
;       Carry = NOT (Borrow to be subtracted)
; Output:
;       DestH:DestL = Result
;       Carry = NOT (Borrow result)
sub16	macro	destH, destL, sourceH, sourceL
        movf    sourceL, w
        subwf   destL
        movf    sourceH, w
        btfss   STATUS, C
        incfsz  sourceH, w
        subwf   destH
	endm

; 16-bit Subtraction with Carry
; dest = dest + source, WITH VALID CARRY
; Input:
;       DestH:DestL = Number to be added FROM
;       SourceH:SourceL = Number to be added
; Output:
;       DestH:DestL = Result
;       Carry = NOT
add16	macro	destH, destL, sourceH, sourceL
	movf	sourceL, w
        addwf   destL		; add lsb
        btfsc	STATUS, C       ; add in carry
        incf    destH
        movf    sourceH, w
        addwf   destH		; add msb
        endm
