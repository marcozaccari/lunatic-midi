#include <p16f886.inc>
#include "misc.inc"  

#ifndef TXRX_BUFFER_SELF_INCLUDED
    #define  TXRX_BUFFER_SELF_INCLUDED 
    #include "txrx_buffer.inc"  
#endif    

    
    GLOBAL TXRX_TX_BUFFER
    GLOBAL TXRX_TX_BUFFER_HEAD
    GLOBAL TXRX_TX_BUFFER_TAIL    

    GLOBAL TXRX_RX_BUFFER
    GLOBAL TXRX_RX_BUFFER_HEAD
    GLOBAL TXRX_RX_BUFFER_TAIL    

    GLOBAL TXRX_TEMP
	    
;*******************************************************************************
;    VARIABLES                                                                 *
;*******************************************************************************
    
GRP_TXRX   UDATA

TXRX_TX_BUFFER	    RES TXRX_TX_BUFFER_SIZE
TXRX_TX_BUFFER_HEAD    RES 1
TXRX_TX_BUFFER_TAIL    RES 1

TXRX_RX_BUFFER	    RES TXRX_RX_BUFFER_SIZE
TXRX_RX_BUFFER_HEAD    RES 1
TXRX_RX_BUFFER_TAIL    RES 1

TXRX_TEMP	    RES 1

	    
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************
	
;*******************************************************************************
;    SUBROUTINES                                                               *
;*******************************************************************************
TXRX CODE
	
; add w to buffer (old code)
;TXRX_BUFFER_ADD
    ; set buffer[tail] value
;    banksel TXRX_TEMP
;    movwf   TXRX_TEMP

;    set_array TXRX_BUFFER, TXRX_BUFFER_TAIL, TXRX_TEMP
    ;movlw   TXRX_BUFFER
    ;movwf   FSR
    ;movf    TXRX_BUFFER_TAIL, w
    ;addwf   FSR
    ;movf    TXRX_TEMP, w
    ;movwf   INDF
    
    ; increment tail
;    incf    TXRX_BUFFER_TAIL

;    movlw   TXRX_BUFFER_SIZE
;    subwf   TXRX_BUFFER_TAIL, w
;    btfsc   STATUS, Z		    ; tail == buffer_size ?
;    clrf    TXRX_BUFFER_TAIL  
;    return
    

    END