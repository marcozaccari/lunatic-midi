 #include "lib/txrx_buffer.inc"  

    
    EXTERN USART_INIT
    EXTERN USART_TX_BLOCKING

    EXTERN USART_RX_DO
    EXTERN USART_TX_DO
    EXTERN USART_TX_DO_ISR
    
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

; send a literal character in blocking mode
usart_tx_blocking macro char	    
    movlw	char
    call	USART_TX_BLOCKING
    endm  

; send W value in blocking mode
usart_tx_w_blocking macro
    call	USART_TX_BLOCKING
    endm
	
; send literal character    
usart_tx macro char
    movlw	char
    usart_tx_w

    ;txrx_buffer_add_w

    ;#ifdef USART_ISR
    ;banksel	PIE1
    ;bsf		PIE1, TXIE		    ; enable interrupt for tx
    ;#endif
    endm  

; send W value    
usart_tx_w macro
    txrx_buffer_tx_add_w

    #ifdef USART_ISR
    banksel	PIE1
    bsf		PIE1, TXIE		    ; enable interrupt for tx
    #endif
    endm  

; initialize USART
usart_init macro
    call	USART_INIT
    endm

; (non-ISR) Non blocking TX state machine. To be called once at every main cycle
usart_tx_do macro
    call	USART_TX_DO
    endm
    
; (non-ISR) Non blocking RX state machine. To be called once at every main cycle
usart_rx_do macro
    call        USART_RX_DO
    endm

; (ISR) Non blocking TX state machine. To be called in ISR
usart_tx_do_isr macro
    call	USART_TX_DO_ISR
    endm
    
    