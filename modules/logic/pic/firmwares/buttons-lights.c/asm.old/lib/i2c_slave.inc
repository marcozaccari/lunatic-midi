 #include "lib/txrx_buffer.inc"  

    
    EXTERN I2C_INIT

    EXTERN I2C_TXRX_DO
    EXTERN I2C_TXRX_DO_ISR
    
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

; send literal character    
i2c_tx macro char
    movlw	char
    i2c_tx_w
    endm  

; send W value    
i2c_tx_w macro
    txrx_buffer_tx_add_w
    endm  

; initialize USART
i2c_init macro
    call	I2C_INIT
    endm

; (non-ISR) Non blocking TXRX state machine. To be called once at every main cycle
i2c_txrx_do macro
    call        I2C_TXRX_DO
    endm

; (ISR) Non blocking TXRX state machine. To be called in ISR
i2c_txrx_do_isr macro
    call	I2C_TXRX_DO_ISR
    endm
    
    