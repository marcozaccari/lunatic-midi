#include "src/main.inc"  

#ifdef USE_USART
    #include "lib/usart.inc"
#endif
#ifdef USE_I2C
    #include "lib/i2c_slave.inc"
#endif
    
    
#ifndef TXRX_SELF_INCLUDED
    
    EXTERN TXRX_TEMP
    
    
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

; initialize TXRX module
txrx_init macro
    #ifdef USE_USART
    usart_init
    #endif
    
    #ifdef USE_I2C
    i2c_init
    #endif
    
    ; init buffer vars
    txrx_buffer_init
    endm

; check rx buffer, skip if there are data to read (with txrx_get_buffer)    
txrx_skip_if_rx_buffer_not_empty macro
    txrx_buffer_rx_skip_if_not_empty
    endm
    
; check rx buffer, skip if there are not data to read    
txrx_skip_if_rx_buffer_empty macro
    txrx_buffer_rx_skip_if_empty
    endm
    
; get last byte from rx buffer and put in w (w = buffer[head]), updating buffer head (head++)
; NOTE: this macro does not check for buffer empty!
txrx_get_rx_w macro
    txrx_buffer_rx_get_w
    movwf   TXRX_TEMP
    txrx_buffer_rx_inc_head
    movf    TXRX_TEMP, w
    endm

    
txrx_do_isr macro    
    #ifdef USE_USART
    usart_tx_do_isr
    #endif
    #ifdef USE_I2C
    i2c_txrx_do_isr
    #endif
    endm

txrx_do macro
    #ifdef USE_USART
    usart_rx_do
    #endif
    #ifdef USE_I2C
    i2c_txrx_do
    #endif
    endm
    
; send a literal character in blocking mode
txrx_tx_blocking macro char	    
    #ifdef USE_USART
    usart_tx_blocking char
    #endif
    endm  
    
; send w value in blocking mode
txrx_tx_w_blocking macro
    #ifdef USE_USART
    usart_tx_w_blocking
    #endif
    endm  

; send literal character    
txrx_tx macro char
    #ifdef USE_USART
    usart_tx char
    #endif
    #ifdef USE_I2C
    i2c_tx char
    #endif
    endm
    
; send w value    
txrx_tx_w macro
    #ifdef USE_USART
    usart_tx_w
    #endif
    #ifdef USE_I2C
    i2c_tx_w
    #endif
    endm
    
#endif