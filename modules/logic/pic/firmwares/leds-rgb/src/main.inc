;*******************************************************************************
;    GLOBAL CONFIGURATION                                                      *
;*******************************************************************************
#ifndef GLOBAL_INC

    #define TXRX_TX_BUFFER_SIZE   d'1'
    #define TXRX_RX_BUFFER_SIZE   d'64'

    ; TX by interrupt (no lag)
    ; #define TXRX_ISR
    
    ;#define USE_USART
    #define USE_I2C
    
    
    ; 20MHz external oscillator, 115200 baud rate USART (otherwise 9600)
    #define EXTERNAL_CLOCK
    
    ; B6, B7 disabled
    ;#define PROGRAMMER
    
    
    #define GLOBAL_FLAG_PIN		 6
    
#define GLOBAL_INC
#endif

