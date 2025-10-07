#include <p16f886.inc>
#include "src/main.inc"
#include "misc.inc"  
#include "txrx_buffer.inc"

    GLOBAL USART_INIT

    GLOBAL USART_TX_BLOCKING

    GLOBAL USART_TX_DO
    GLOBAL USART_RX_DO
    GLOBAL USART_TX_DO_ISR
	    
;*******************************************************************************
;    VARIABLES                                                                 *
;*******************************************************************************
    
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

;*******************************************************************************
;    SUBROUTINES                                                               *
;*******************************************************************************
USART CODE
	
; Initialize USART
USART_INIT
    ; init port
    banksel TRISC
    clrf    TRISC               ; PortC as output
    bsf	    TRISC, TRISC7       ; RC7 as RX input
    banksel  PORTC
    clrf    PORTC		; clear output data latches on port
    
    ; Set baud rate
    #ifdef EXTERNAL_CLOCK
	; 116.300 baud rate @ 20MHz
	#define SPBRGH_VALUE 0
	#define SPBRG_VALUE d'42'
	#define BRGH_SET bsf	; high speed
	#define BRG16_SET bsf	; 16bit baud generator
    #else
	; 19.230 baud rate @ 8MHz
	#define SPBRGH_VALUE 0
	#define SPBRG_VALUE d'25'
	#define BRGH_SET bcf	; low speed
	#define BRG16_SET bsf	; 16bit baud generator
    #endif

    banksel SPBRGH
    movlw   SPBRGH_VALUE
    movwf   SPBRGH
    ;banksel SPBRGL
    movlw   SPBRG_VALUE
    movwf   SPBRG
    ;banksel TXSTA
    BRGH_SET TXSTA, BRGH
    banksel BAUDCTL
    BRG16_SET BAUDCTL, BRG16

    ; configure and enable the port
    banksel TXSTA
    bcf	    TXSTA, SYNC		; enable async mode
    bsf	    TXSTA, TXEN		; enable transmission
    banksel RCSTA
    bsf     RCSTA, CREN         ; enable receive    
    bsf	    RCSTA, SPEN		; enable serial port
    return

    
; (non-ISR) Non blocking TX state machine. To be called once at every main cycle
USART_TX_DO
    ; if already sending return
    banksel PIR1
    btfss   PIR1, TXIF		    ; txreg is empty
    return			    ; (0 = no) return

    ; if head == tail then return
    txrx_buffer_tx_skip_if_not_empty
    return
    
    ; get buffer[head] value
    txrx_buffer_tx_get_w
    
    ; send char to port
    banksel TXREG
    movwf   TXREG    

    txrx_buffer_tx_inc_head    
    return

    
; (non-ISR) Non blocking RX state machine. To be called once at every main cycle
USART_RX_DO
    banksel PIR1
    btfss   PIR1, RCIF
    return
    
    btfsc   RCSTA, OERR         ; if overrun error occurred
    goto    ErrSerialOverr      ; then go handle error
    btfsc   RCSTA, FERR         ; if framing error occurred
    goto    ErrSerialFrame      ; then go handle error
    
    movf    RCREG, w            ; received byte
    txrx_buffer_rx_add_w
    return
    
ErrSerialOverr:
    bcf     RCSTA, CREN         ; reset the receiver logic
    bsf     RCSTA, CREN         ; enable reception again
    txrx_buffer_rx_add_w
    return
    
ErrSerialFrame:
    movf    RCREG, w            ; discard received data that has error
    txrx_buffer_rx_add_w
    return
    
    
; (ISR) Non blocking TX state machine. To be called in ISR
USART_TX_DO_ISR
    ; if head == tail disable interrupt (*)
    txrx_buffer_tx_skip_if_not_empty
    goto DoISRNoINT

    ; get buffer[head] value
    txrx_buffer_tx_get_w
    
    ; send char to port
    banksel TXREG
    movwf   TXREG    

    txrx_buffer_tx_inc_head
    
    ; if head == tail disable interrupt
    txrx_buffer_tx_skip_if_empty
    return

DoISRNoINT:    
    banksel PIE1
    bcf     PIE1, TXIE		    ; disable interrupt
    return

    
; Send character stored to W to USART
USART_TX_BLOCKING  
    banksel TXREG
    movwf   TXREG
    nop
    banksel PIR1
    btfss   PIR1, TXIF		; txreg is empty?
    goto    $-1			; (0 = no) goto prev instruction
    return


    END