#include "src/main.inc"

#ifndef TXRX_TX_BUFFER_SIZE
    #define TXRX_TX_BUFFER_SIZE   d'60'
#endif

#ifndef TXRX_RX_BUFFER_SIZE   
    #define TXRX_RX_BUFFER_SIZE   d'4'
#endif

#ifndef TXRX_BUFFER_SELF_INCLUDED
    
    EXTERN TXRX_TX_BUFFER
    EXTERN TXRX_TX_BUFFER_HEAD
    EXTERN TXRX_TX_BUFFER_TAIL

    EXTERN TXRX_RX_BUFFER
    EXTERN TXRX_RX_BUFFER_HEAD
    EXTERN TXRX_RX_BUFFER_TAIL
    
    EXTERN TXRX_TEMP
    
    
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

; initialize buffers
txrx_buffer_init macro
    banksel TXRX_TX_BUFFER
    clrf    TXRX_TX_BUFFER_HEAD
    clrf    TXRX_TX_BUFFER_TAIL
    clrf    TXRX_RX_BUFFER_HEAD
    clrf    TXRX_RX_BUFFER_TAIL
    endm

; check tx buffer, skip if there are data to send (with txrx_get_tx_buffer)    
txrx_buffer_tx_skip_if_not_empty macro
    banksel TXRX_TX_BUFFER
    movf    TXRX_TX_BUFFER_HEAD, w
    subwf   TXRX_TX_BUFFER_TAIL, w
    btfsc   STATUS, Z               ; head != tail ? (yes => skip)
    endm

; check tx buffer, skip if there are data to read (with txrx_get_rx_buffer)    
txrx_buffer_rx_skip_if_not_empty macro
    banksel TXRX_RX_BUFFER
    movf    TXRX_RX_BUFFER_HEAD, w
    subwf   TXRX_RX_BUFFER_TAIL, w
    btfsc   STATUS, Z               ; head != tail ? (yes => skip)
    endm
    
; check tx buffer, skip if there are not data to send    
txrx_buffer_tx_skip_if_empty macro
    banksel TXRX_TX_BUFFER
    movf    TXRX_TX_BUFFER_HEAD, w
    subwf   TXRX_TX_BUFFER_TAIL, w
    btfss   STATUS, Z               ; head == tail ? (yes => skip)
    endm
    
; check rx buffer, skip if there are not data to read    
txrx_buffer_rx_skip_if_empty macro
    banksel TXRX_RX_BUFFER
    movf    TXRX_RX_BUFFER_HEAD, w
    subwf   TXRX_RX_BUFFER_TAIL, w
    btfss   STATUS, Z               ; head == tail ? (yes => skip)
    endm

; get tx buffer (available) size, in bytes. 
; put result to w
txrx_buffer_tx_size_w macro
    local   tx_size_no_carry

    banksel TXRX_TX_BUFFER_TAIL
    movf    TXRX_TX_BUFFER_HEAD, w
    subwf   TXRX_TX_BUFFER_TAIL, w ; tail - head
    btfsc   STATUS, C		    ; head > tail?  (carry is negated)
    goto    tx_size_no_carry	    ; no, avoid next three instructions
    movf    TXRX_TX_BUFFER_HEAD, w
    sublw   TXRX_TX_BUFFER_SIZE	    ; w = size - head
    addwf   TXRX_TX_BUFFER_TAIL, w  ; w += tail
tx_size_no_carry:
    endm
    
; get last byte from tx buffer and put in w (w = buffer[head])
; NOTE: this macro does not check for buffer empty!
; please use txrx_buffer_tx_inc_head in order to consume the byte
txrx_buffer_tx_get_w macro
    banksel TXRX_TX_BUFFER
    get_array TXRX_TX_BUFFER, TXRX_TX_BUFFER_HEAD
    ;movlw   TXRX_BUFFER
    ;movwf   FSR
    ;movf    TXRX_BUFFER_HEAD, w
    ;addwf   FSR
    ;movf    INDF, w
    endm

; get last byte from rx buffer and put in w (w = buffer[head])
; NOTE: this macro does not check for buffer empty!
; please use txrx_buffer_rx_inc_head in order to consume the byte
txrx_buffer_rx_get_w macro
    banksel TXRX_RX_BUFFER
    get_array TXRX_RX_BUFFER, TXRX_RX_BUFFER_HEAD
    endm
    
; increment tx buffer head
txrx_buffer_tx_inc_head macro
    banksel TXRX_TX_BUFFER
    incf    TXRX_TX_BUFFER_HEAD

    movlw   TXRX_TX_BUFFER_SIZE
    subwf   TXRX_TX_BUFFER_HEAD, w
    btfsc   STATUS, Z		    ; head == buffer_size ?
    clrf    TXRX_TX_BUFFER_HEAD        ;  yes => reset head
    endm	    

; increment rx buffer head
txrx_buffer_rx_inc_head macro
    banksel TXRX_RX_BUFFER
    incf    TXRX_RX_BUFFER_HEAD

    movlw   TXRX_RX_BUFFER_SIZE
    subwf   TXRX_RX_BUFFER_HEAD, w
    btfsc   STATUS, Z		    ; head == buffer_size ?
    clrf    TXRX_RX_BUFFER_HEAD        ;  yes => reset head
    endm	    
    
; add w to tx buffer (ISR safe)
txrx_buffer_tx_add_w macro   
    ; set buffer[tail] value
    banksel TXRX_TEMP
    movwf   TXRX_TEMP

    set_array TXRX_TX_BUFFER, TXRX_TX_BUFFER_TAIL, TXRX_TEMP
    ;movlw   TXRX_BUFFER
    ;movwf   FSR
    ;movf    TXRX_BUFFER_TAIL, w
    ;addwf   FSR
    ;movf    TXRX_TEMP, w
    ;movwf   INDF
    
    ; increment tail
    incf    TXRX_TX_BUFFER_TAIL, w    ; use TXRX_TEMP avoiding ISR
    movwf   TXRX_TEMP
    
    movlw   TXRX_TX_BUFFER_SIZE
    subwf   TXRX_TEMP, w
    btfsc   STATUS, Z		    ; tail == buffer_size ?
    clrf    TXRX_TEMP
    
    movf    TXRX_TEMP, w    
    movwf   TXRX_TX_BUFFER_TAIL
    ;(*)
    endm

; add w to rx buffer (ISR safe)
txrx_buffer_rx_add_w macro   
    ; set buffer[tail] value
    banksel TXRX_TEMP
    movwf   TXRX_TEMP

    set_array TXRX_RX_BUFFER, TXRX_RX_BUFFER_TAIL, TXRX_TEMP
    ;movlw   TXRX_BUFFER
    ;movwf   FSR
    ;movf    TXRX_BUFFER_TAIL, w
    ;addwf   FSR
    ;movf    TXRX_TEMP, w
    ;movwf   INDF
    
    ; increment tail
    incf    TXRX_RX_BUFFER_TAIL, w    ; use TXRX_TEMP avoiding ISR
    movwf   TXRX_TEMP
    
    movlw   TXRX_RX_BUFFER_SIZE
    subwf   TXRX_TEMP, w
    btfsc   STATUS, Z		    ; tail == buffer_size ?
    clrf    TXRX_TEMP
    
    movf    TXRX_TEMP, w    
    movwf   TXRX_RX_BUFFER_TAIL
    ;(*)
    endm
    
#endif