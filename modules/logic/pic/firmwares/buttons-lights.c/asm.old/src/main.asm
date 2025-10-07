;*******************************************************************************
; Buttons and status leds controller
; author: Marco Zaccari info@marcozaccari.com
;
; board: z-
; cpu: PIC 16F886

; Send buttons status to I2C.
; Read from I2C and set status leds.
;
; Protocol:
;    (RX from this device):
;    first byte after I2C address = buffer length to read
;    other bytes = buffer content
;       0xFF = invalid (out of buffer)
;       0x80 + 0..N = Button N is pressed
;       0..N = Button N is depressed
;
;    (TX to this device):
;    siiiiiii where:
;      i = Led index (0..126)
;      s = state (0 = OFF; 1 = ON)
;    0xFF = reset (all leds off)
;
;*******************************************************************************

#include <p16f886.inc>
#include "main.inc"
#include "lib/misc.inc"
#include "lib/txrx.inc"
#include "buttons.inc"
#include "leds.inc"


; FUSE BITS
#ifdef EXTERNAL_CLOCK
    __CONFIG _CONFIG1, _FOSC_HS & _WDTE_ON & _PWRTE_ON & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_ON & _IESO_ON & _FCMEN_OFF & _LVP_OFF & _DEBUG_OFF
#else
    __CONFIG _CONFIG1, _FOSC_INTRC_NOCLKOUT & _WDTE_ON & _PWRTE_ON & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_ON & _IESO_ON & _FCMEN_OFF & _LVP_OFF & _DEBUG_OFF
#endif
    __CONFIG _CONFIG2, _BOR4V_BOR40V & _WRT_OFF


;*******************************************************************************
; VARIABLES
;*******************************************************************************

GRP_MAIN	    UDATA_SHR
TIMER_H		    RES 1
TIMER_L		    RES 1	; global timer (1ms/2ms)

ISR_W_TEMP	    RES 1	; for ISR context switching
ISR_STATUS_TEMP	    RES 1	; for ISR context switching

GLOBAL_FLAGS	    RES 1 ; stores GLOBAL_FLAG_PIN, GLOBAL_FLAG_NOTEOFF_VELOCITY 
	    
GRP_MAIN_MISC	    UDATA
PCLATH_TEMP	    RES 1
FSR_TEMP	    RES 1 
    
 
    GLOBAL TIMER_H
    GLOBAL TIMER_L
    GLOBAL GLOBAL_FLAGS
    
    EXTERN LED_TX_COUNTER
 

;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************
  
	
;*******************************************************************************
; Reset Vector
;*******************************************************************************

RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    INIT                   ; go to beginning of program

    
;*******************************************************************************
; Interrupts
;*******************************************************************************

ISR       CODE    0x0004
    ; save context
    movwf   ISR_W_TEMP		; copy W to TEMP register
    swapf   STATUS, W		; swap status to be saved into W
				; (swaps are used because they do not affect the status bits)
    movwf   ISR_STATUS_TEMP	; save status to bank zero ISR_STATUS_TEMP register

    banksel PCLATH_TEMP		; for gotos and calls
    movf    PCLATH, w
    movwf   PCLATH_TEMP
    movf    FSR, w		; for indirect addressing
    movwf   FSR_TEMP    
    
    ; check for timer1
    banksel PIR1
    btfss   PIR1, TMR2IF
    goto	ISR_TxRx

ISR_timer:    
    ; clear interrupt request flag
    ;banksel PIR1
    bcf	    PIR1, TMR2IF

    ; execute interrupt
    incf    TIMER_L
    btfsc   STATUS, Z
    incf    TIMER_H
    
    ;banksel PORTC
    ;comf    PORTC
    
    led_tx_isr
       
    goto    ISR_End

ISR_TxRx:
    ;banksel PORTC
    ;comf    PORTC
    
    txrx_do_isr
    
ISR_End:    
    
    ; restore context
    banksel PCLATH_TEMP
    movf    PCLATH_TEMP, w
    movwf   PCLATH
    movf    FSR_TEMP, w
    movwf   FSR    
    
    swapf   ISR_STATUS_TEMP, W	; swap ISR_STATUS_TEMP register into W
				; (sets bank to original state)
    movwf   STATUS		; move W into STATUS register
    swapf   ISR_W_TEMP, F	; swap ISR_W_TEMP
    swapf   ISR_W_TEMP, W	; swap ISR_W_TEMP into W
    retfie
       
    
;*******************************************************************************
; MAIN PROGRAM
;*******************************************************************************
MAIN_PROG CODE			; let linker place main program

; Initialize peripherals 
INIT
    clrf    STATUS		; Bank0 as default

    ; set system clock
    #ifndef EXTERNAL_CLOCK    
	banksel OSCCON
	movlw   B'01110001'	; Max clock (8MHz)
	movwf   OSCCON
    #endif

    banksel TRISC
    clrf    TRISC               ; PortC as output
    
    clrf    GLOBAL_FLAGS	; off-velocity off, pin flag off 
    
    ; initialize TXRX
    txrx_init
    
    #ifdef PROGRAMMER
    delay_1sec
    led_on
    delay_5dec
    led_off
    delay_5dec
    led_on
    delay_5dec
    led_off
    delay_5dec
    led_on
    delay_5dec
    led_off
    delay_5dec
    led_on
    #endif
    led_on

    ; initialize outputs
    buttons_init
    leds_init
    
    ;banksel TRISA
    ;bsf	    TRISA, TRISA5	; PortA-5 as input
    
    ; initialize Timer2
    clrf    TIMER_H
    clrf    TIMER_L

    banksel INTCON
    bsf	    INTCON, GIE		; enable all interrupts
    bsf	    INTCON, PEIE	; enable peripheral interrupts
    banksel PIE1
    bsf	    PIE1, TMR2IE	; interrupt on Timer2 overflow
    banksel T2CON
    movlw   b'10000100'		; mask scaler bits
    andwf   T2CON
    movlw   b'00000011'		; prescaler = 16
    iorwf   T2CON
    movlw   b'00000000'		; postscaler = 1 (0.5ms) <-- BEST
    ;movlw   b'00001000'		; postscaler = 2 (1ms)
    ;movlw   b'00011000'		; postscaler = 4 (2ms)
    iorwf   T2CON
    bsf	    T2CON, TMR2ON	; enable timer (counter in TMR2)
    banksel PR2
    movlw   d'155'		; 1ms  <-- BEST
    ;movlw   d'75'		; 0.5ms
    ;movlw   d'100'		
    ;movlw   d'150'		
    movwf   PR2
    ; 1/(Fosc/4/prescaler)*PR2*postscaler
    
    ; set watchdog
    banksel INTCON
    bcf	    INTCON, T0IE	; disable interrupt
    banksel TMR0
    clrwdt
    clrf    TMR0		; clear TMR0 and prescaler
    banksel OPTION_REG
    bsf	    OPTION_REG, PSA	; select WDT
    clrwdt
    bcf	    OPTION_REG, T0CS	; fosc/4 input
    movlw   b'11111000'		; mask prescaler bits
    andwf   OPTION_REG, w
    iorlw   b'00000101'		; 1:32 prescaler
    movwf   OPTION_REG		; 1/(fosc/4/prescaler)*256 = 1,6ms
    
    
START
    leds_set
    
MAIN_LOOP
    
    led_debug_on
    
    buttons_scan
    ;leds_set
    
    led_debug_off

    clrwdt
    
    ; read flag pin
    ;banksel PORTA
    ;bcf	    GLOBAL_FLAGS, GLOBAL_FLAG_PIN
    ;btfsc   PORTA, 5
    ;bsf	    GLOBAL_FLAGS, GLOBAL_FLAG_PIN
    
    ; read and parse rx
    txrx_skip_if_rx_buffer_not_empty
    goto MAIN_LOOP
    txrx_get_rx_w
    ;txrx_tx_w

    movwf   TEMP1
    movlw   0xFF
    subwf   TEMP1, w
    btfss   STATUS, Z
    goto    LedSetValue
    leds_reset
    goto    MAIN_LOOP
    
LedSetValue:
    movf   TEMP1, w
    leds_set_value_w
    
    goto MAIN_LOOP

  END
