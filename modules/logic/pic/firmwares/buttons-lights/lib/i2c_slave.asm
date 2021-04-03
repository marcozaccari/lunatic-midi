#include <p16f886.inc>
#include "src/main.inc"
#include "misc.inc"  
#include "txrx_buffer.inc"

    
    ; I2C ADDRESS = BASE + (PIN2 * 2) + PIN1
    #define I2C_ADDRESS_BASE	    0x30    ; 7bit address
    #define I2C_ADDRESS_ADD_PORT    PORTC
    #define I2C_ADDRESS_ADD_TRIS    TRISC
    #define I2C_ADDRESS_ADD_PIN1    RC0
    #define I2C_ADDRESS_ADD_PIN2    RC5
    
    
    GLOBAL I2C_INIT

    GLOBAL I2C_TXRX_DO
    GLOBAL I2C_TXRX_DO_ISR
    
    EXTERN  GLOBAL_FLAGS
    
	    
;*******************************************************************************
;    VARIABLES                                                                 *
;*******************************************************************************
GRP_I2C   UDATA
I2C_RW_MODE	    RES 1   ; 0 = master wants to read; 1 = master wants to write
    
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

;*******************************************************************************
;    SUBROUTINES                                                               *
;*******************************************************************************
I2C CODE
	
; Initialize USART
I2C_INIT
    ; set gpio
    banksel TRISC
    clrf    TRISC              ; PortC as output
    bsf	    TRISC, TRISC3       ; RC3 as clock input (SCL)
    bsf	    TRISC, TRISC4       ; RC4 as data (SDA)
    banksel  PORTC
    clrf    PORTC		; clear output data latches on port
    
    banksel I2C_ADDRESS_ADD_TRIS
    bsf	    I2C_ADDRESS_ADD_TRIS, I2C_ADDRESS_ADD_PIN2
    bsf	    I2C_ADDRESS_ADD_TRIS, I2C_ADDRESS_ADD_PIN1

    ; configure port
    banksel SSPSTAT
    bcf	    SSPSTAT, SMP	; slew rate enabled for high speed (400kHz)
        
    ; configure address
    movlw   I2C_ADDRESS_BASE
    banksel  I2C_ADDRESS_ADD_PORT
    btfsc    I2C_ADDRESS_ADD_PORT, I2C_ADDRESS_ADD_PIN2
    addlw   2
    btfsc    I2C_ADDRESS_ADD_PORT, I2C_ADDRESS_ADD_PIN1
    addlw   1
    
    banksel SSPADD
    movwf   SSPADD
    rlf	     SSPADD
    
    ; configure and enable the port
    banksel SSPCON2
    bcf	    SSPCON2, GCEN	; general address disable
    
    banksel SSPCON
    ;movlw  b'00001111'
    movlw  b'00000110'
    movwf   SSPCON
    
    ;iorwf  SSPCON
    ;movlw  b'11110110'		; xxxxx0110 = slave mode 7 bit address
    ;movlw  b'11111110'		; xxxxx1110 = slave mode 7 bit address, start and stop interrupt
    ;andwf  SSPCON
    
 ;   bcf	    SSPCON, CKP		; clock stretch
 ;   bcf	    SSPCON, SSPOV	; clear overflow
    bsf	    SSPCON, SSPEN	; enable I2C port
    ; SCL, SDA are open dry now
 
    ; enable interrupts
    #ifdef TXRX_ISR
    banksel PIE1
    bsf	    PIE1, SSPIE
    #endif
    
    banksel PIR1
    bcf     PIR1, SSPIF		; disable interrupt
    return

; (non-ISR) Non blocking TXRX state machine. To be called once at every main cycle
I2C_TXRX_DO
    banksel PIR1
    btfss   PIR1, SSPIF
    return
    
; (ISR) Non blocking TXRX state machine. To be called in ISR
I2C_TXRX_DO_ISR
    ;led_tx_on    

    ;banksel PIR1
    ;bcf     PIR1, SSPIF		; disable interrupt
    
    banksel SSPBUF
    movf    SSPBUF, w		 ; read (discharge buffer)    

    ;goto I2C_TxRx_Tx

    banksel SSPCON
    btfss   SSPCON, SSPOV
    goto    I2C_No_Overflow
I2C_Overflow:    
    bcf	    SSPCON, SSPOV	; clear overflow
    bsf	    SSPCON, CKP		; assure no clock stretch
    ;banksel SSPBUF
    ;movf    SSPBUF, w		 ; read (discharge buffer)    
I2C_No_Overflow:    
        
    banksel SSPSTAT
    btfsc   SSPSTAT, D_NOT_A    ; received data or address?
    goto    I2C_TxRx_Data	 ;  was data
    
I2C_TxRx_Address:
    banksel I2C_RW_MODE
    bsf	    I2C_RW_MODE, 0	; set to master-wants-to-write
    banksel SSPSTAT
    btfss   SSPSTAT, R_NOT_W    ; receive or write request?
    goto I2C_TxRx_End
    
    ; read request
    banksel SSPCON
    btfsc   SSPCON, CKP		 ; discard interrupt if CKP is up (master end)
    goto I2C_TxRx_End
    
    banksel I2C_RW_MODE
    bcf	    I2C_RW_MODE, 0	; set to master-wants-to-read
    
    ; return buffer available size
    txrx_buffer_tx_size_w

    banksel SSPBUF
    movwf   SSPBUF

    banksel SSPCON
    bsf	    SSPCON, CKP		; clock stretch
    
    goto I2C_TxRx_End
    
I2C_TxRx_Data:
    banksel I2C_RW_MODE
    btfss   I2C_RW_MODE, 0
    goto I2C_TxRx_Tx

I2C_TxRx_Rx:
    txrx_buffer_rx_add_w
   
    ;banksel SSPCON
    ;bsf	    SSPCON, SSPOV

    goto I2C_TxRx_End

I2C_TxRx_Tx:
    banksel SSPCON
    btfsc   SSPCON, CKP		 ; discard interrupt if CKP is up (master end)
    goto I2C_TxRx_End
    
    txrx_buffer_tx_skip_if_empty
    goto I2C_TxRx_TxData
    
    movlw   0xFF
    banksel SSPBUF
    movwf   SSPBUF		; buffer empty, send FF

    banksel SSPCON
    bsf	    SSPCON, CKP		; clock stretch
    
    goto I2C_TxRx_End
    
I2C_TxRx_TxData:    
    ; get buffer[head] value
    txrx_buffer_tx_get_w

    banksel SSPBUF
    movwf   SSPBUF

    banksel SSPCON
    bsf	    SSPCON, CKP		; clock stretch

    txrx_buffer_tx_inc_head
    
I2C_TxRx_End:
    banksel PIR1
    bcf     PIR1, SSPIF		; disable interrupt
    return

    END