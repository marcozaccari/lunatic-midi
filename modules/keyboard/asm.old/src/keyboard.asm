#include <p16f886.inc>
#include "main.inc"
#include "lib/misc.inc"
#include "lib/txrx.inc"

    GLOBAL  KEYB_INIT
    GLOBAL  KEYB_SCAN_SWITCH_ON
    GLOBAL  KEYB_SCAN_SWITCH_ONLONG
    
    EXTERN  TIMER_H
    EXTERN  TIMER_L
    EXTERN  GLOBAL_FLAGS
    
;*******************************************************************************
; KA4  KA3   
;  0    0   First half (1..32 keys)
;  0    1   Second half (33..64 keys)
;  1    0   -
;  1    1   - 
;
; KA2  KA1  KA0
;  0    0    0    1..8   keys (KEY ON)
;  0    0    1    1..8   keys (KEY ON2)
;  0    1    0    9..16  keys (KEY ON)
;  0    1    1    9..16  keys (KEY ON2)
;  1    0    0    17..24 keys (KEY ON)
;  1    0    1    17..24 keys (KEY ON2)
;  1    1    0    25..32 keys (KEY ON)
;  1    1    1    25..32 keys (KEY ON2)
;   
;
; Flusso logico:
;
; - Leggi tasto interruttore #1 (con antibounce)
;     - E' stato attivato ora?
;         - SI: resetta il suo contatore
;
; - Leggi tasto interruttore #2 (con antibounce)
;     - E' stato attivato ora?
;         - NO: incrementa il suo contatore
;         - SI: salva il contatore e notifica alla seriale
;     - E' stato rilasciato ora?
;         - SI: notifica alla seriale   
;*******************************************************************************
    
    
;*******************************************************************************
;    VARIABLES                                                                 *
;*******************************************************************************

#define	    KEYB_DEBOUNCE_SCAN_CYCLES	0
    
GRP_KEYB_SHR		UDATA_SHR
; indices
KEYB_CURBANK		RES 1	; bank index (0..7)
KEYB_CURBANK_COUNTER	RES 1	; bank counter index (8..1)
KEYB_CURBANK_ABS	RES 1	; bank absolute index (0, 8, 16, 24, 32, 48, 56)
KEYB_CURADDR		RES 1	; address for PORTA
KEYB_CURADDR_AND	RES 1	; address for PORTA (lower bits - for KEYB_SCAN_BITS)
KEYB_CURKEY 		RES 1
KEYB_CURBITS_NEW	RES 1	; current status of 8 keys (0 -> 1 variations)	
KEYB_CURBITS_OFF	RES 1	; current status of 8 keys (1 -> 0 variations)
	
GRP_KEYB		UDATA
KEYB_CURBITS		RES 1	; current status of !PORTB	
    
GRP_KEYB_BUFFERS	UDATA
; port status (previous and current)		
KEYB_BITS		RES 8   ; raw bits status (by KEYB_SCAN_BITS)
KEYB_ON_BITS_OLD	RES 8
KEYB_ONLONG_BITS_OLD	RES 8
    
; 16bit counters	
GRP_KEYB_COUNTERS1	UDATA
KEYB_TIMERS_H		RES d'64'
GRP_KEYB_COUNTERS2	UDATA
KEYB_TIMERS_L		RES d'64'
	 
; temp variables used for calculate times		
GRP_KEYB_TIMERS_MISC    UDATA
TIMER_SAVE_H		RES 1
TIMER_SAVE_L		RES 1
TIMESTAMP_H		RES 1
TIMESTAMP_L		RES 1
		
;*******************************************************************************
;    SUBROUTINES                                                               *
;*******************************************************************************
KEYBOARD CODE

;*******************************************
; Initialize keyboard ports and arrays 
KEYB_INIT
    ; init ports
    banksel ANSEL
    clrf    ANSEL		; digital I/O (PORTA)
    clrf    ANSELH		; digital I/O (PORTB)
    banksel TRISA
    clrf    TRISA		; PortA as output
    
    movlw   0xFF
    movwf   TRISB               ; PortB as input
    
    banksel OPTION_REG
    bcf	    OPTION_REG, NOT_RBPU ; Enable single pullups on PORT B
    banksel WPUB
    movwf   WPUB		; Enable all pull-up for PORT B

    banksel  PORTA
    clrf    PORTA               ; clear output data latches on Ports
    
    ; flush arrays
    movlw   0x08
    movwf   KEYB_CURBANK_COUNTER
    clrf    KEYB_CURBANK
    clrf    TEMP1
KeybInitFlushBits:    
    set_array KEYB_ON_BITS_OLD, KEYB_CURBANK, TEMP1
    set_array KEYB_ONLONG_BITS_OLD, KEYB_CURBANK, TEMP1

    incf    KEYB_CURBANK
    decfsz  KEYB_CURBANK_COUNTER
    goto    KeybInitFlushBits
    return

    
;*******************************************
; Scan keyboard keys for switch-on    
KEYB_SCAN_SWITCH_ON

    ; scan keyboard bits
    movlw   0xFE
    movwf   KEYB_CURADDR_AND
    call    KEYB_SCAN_BITS
    
    ; prepare bank counter
    movlw   8
    movwf   KEYB_CURBANK_COUNTER
    clrf    KEYB_CURBANK
    clrf    KEYB_CURBANK_ABS

KeybScanSwitchOnBank:    

    ; calculate new variations (0 -> 1)
    get_array KEYB_BITS, KEYB_CURBANK
    banksel KEYB_CURBITS
    movwf   KEYB_CURBITS
    movwf   TEMP1

    get_array KEYB_ON_BITS_OLD, KEYB_CURBANK
    movwf   TEMP3

    xorwf   TEMP1	    ; TEMP1 = KEYB_CURBITS XOR KEYB_ON_BITS_OLD (all variations)
    movf    KEYB_CURBITS, w
    andwf   TEMP1, w	    ; W = TEMP1 AND CURBITS (new variations)
    movwf   KEYB_CURBITS_NEW

    movf    TEMP3, w
    andwf   TEMP1, w		; W = TEMP1 AND KEYB_ON_BITS_OLD (1->0 variations)
    movwf   KEYB_CURBITS_OFF

    ; save bits status
    set_array KEYB_ON_BITS_OLD, KEYB_CURBANK, KEYB_CURBITS

    ; check single keys
    movf    KEYB_CURBITS_NEW, w
    btfsc   GLOBAL_FLAGS, GLOBAL_FLAG_NOTEOFF_VELOCITY
    iorwf   KEYB_CURBITS_OFF, w
    btfsc   STATUS, Z		    ; skip test if no variations
    goto    KeybScanOnNoTest

;txrx_tx_w    
;goto    KeybScanOnNoTest    
    clrf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 0
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 0
    call    KEYB_SET_KEY_OFFLONG
    
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 1
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 1
    call    KEYB_SET_KEY_OFFLONG

    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 2
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 2
    call    KEYB_SET_KEY_OFFLONG
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 3
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 3
    call    KEYB_SET_KEY_OFFLONG
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 4
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 4
    call    KEYB_SET_KEY_OFFLONG
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 5
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 5
    call    KEYB_SET_KEY_OFFLONG
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 6
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 6
    call    KEYB_SET_KEY_OFFLONG
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 7
    call    KEYB_SET_KEY_ON
    btfsc   KEYB_CURBITS_OFF, 7
    call    KEYB_SET_KEY_OFFLONG
    
KeybScanOnNoTest:    

    movlw   8
    addwf   KEYB_CURBANK_ABS
    
    incf    KEYB_CURBANK
    decfsz  KEYB_CURBANK_COUNTER
    goto    KeybScanSwitchOnBank
    return

    
;*******************************************
; Scan keyboard keys for switch-on-long    
KEYB_SCAN_SWITCH_ONLONG

    ; scan keyboard bits
    movlw   0xFF
    movwf   KEYB_CURADDR_AND
    call    KEYB_SCAN_BITS

    ; prepare bank counter
    movlw   8
    movwf   KEYB_CURBANK_COUNTER
    clrf    KEYB_CURBANK
    clrf    KEYB_CURBANK_ABS

KeybScanSwitchOnLongBank:    

    ; calculate new variations (0 -> 1)
    get_array KEYB_BITS, KEYB_CURBANK
    banksel KEYB_CURBITS
    movwf   KEYB_CURBITS
    movwf   TEMP1

    get_array KEYB_ONLONG_BITS_OLD, KEYB_CURBANK
    movwf   TEMP3

    xorwf   TEMP1	    ; TEMP1 = KEYB_CURBITS XOR KEYB_ON_BITS_OLD (all variations)
    movf    KEYB_CURBITS, w
    andwf   TEMP1, w	    ; W = TEMP1 AND TEMP2 (new variations)
    movwf   KEYB_CURBITS_NEW
    
    movf    TEMP3, w
    andwf   TEMP1, w		; W = TEMP1 AND KEYB_ONLONG_BITS_OLD (1->0 variations)
    movwf   KEYB_CURBITS_OFF

    ; save bits status
    set_array KEYB_ONLONG_BITS_OLD, KEYB_CURBANK, KEYB_CURBITS

    ; check single keys
    movf    KEYB_CURBITS_NEW, w
    iorwf   KEYB_CURBITS_OFF, w
    btfsc   STATUS, Z		    ; skip test if no variations
    goto    KeybScanOnLongNoTest    

    clrf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 0
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 0
    call    KEYB_SET_KEY_OFF
    
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 1
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 1
    call    KEYB_SET_KEY_OFF

    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 2
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 2
    call    KEYB_SET_KEY_OFF
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 3
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 3
    call    KEYB_SET_KEY_OFF
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 4
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 4
    call    KEYB_SET_KEY_OFF
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 5
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 5
    call    KEYB_SET_KEY_OFF
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 6
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 6
    call    KEYB_SET_KEY_OFF
 
    incf    KEYB_CURKEY
    btfsc   KEYB_CURBITS_NEW, 7
    call    KEYB_SET_KEY_ONLONG
    btfsc   KEYB_CURBITS_OFF, 7
    call    KEYB_SET_KEY_OFF

KeybScanOnLongNoTest:    
    
    movlw   8
    addwf   KEYB_CURBANK_ABS
    
    incf    KEYB_CURBANK
    decfsz  KEYB_CURBANK_COUNTER
    goto    KeybScanSwitchOnLongBank
    return

 
;*******************************************
; Set single key to ON   
; uses KEYB_CURKEY, KEYB_CURBANK_ABS
KEYB_SET_KEY_ON
    movf    KEYB_CURKEY, w
    addwf   KEYB_CURBANK_ABS, w	    ; w = absolute key index
    movwf   TEMP1

    ;txrx_tx_w
    ;txrx_tx_w_blocking
    
    ; save the timestamp
    set_array KEYB_TIMERS_H, TEMP1, TIMER_H
    set_array KEYB_TIMERS_L, TEMP1, TIMER_L
    
 
    ;get_array KEYB_TIMERS_H, TEMP1
    ;txrx_tx_w
    ;get_array KEYB_TIMERS_L, TEMP1
    ;txrx_tx_w
    return
   
    
;*******************************************
; Set single key to ON-LONG   
; uses KEYB_CURKEY, KEYB_CURBANK_ABS
KEYB_SET_KEY_ONLONG
    movf    KEYB_CURKEY, w
    addwf   KEYB_CURBANK_ABS, w	    ; w = absolute key index
    movwf   TEMP1

    iorlw   b'11000000'		    ; key = 11XXXXXX
    txrx_tx_w
    ;txrx_tx_w_blocking

    ; calculate time
    banksel TIMER_SAVE_H
    movf    TIMER_H, w
    movwf   TIMER_SAVE_H
    movwf   TEMP3
    
    movf    TIMER_L, w
    movwf   TIMER_SAVE_L
    movwf   TEMP2

    get_array KEYB_TIMERS_H, TEMP1
    movwf   TIMESTAMP_H
    get_array KEYB_TIMERS_L, TEMP1
    movwf   TIMESTAMP_L

    sub16 TIMER_SAVE_H, TIMER_SAVE_L, TIMESTAMP_H, TIMESTAMP_L 
    btfsc   STATUS, C		    ; timestamp > timer?  (carry is negated, see sub16)
    goto    KeybSetOnLongNoCarry
    movlw   0xFF		    ; timestamp = 0xffff - timestamp + timer 
    movwf   TIMER_SAVE_H
    movwf   TIMER_SAVE_L
    sub16 TIMER_SAVE_H, TIMER_SAVE_L, TIMESTAMP_H, TIMESTAMP_L
    add16 TIMER_SAVE_H, TIMER_SAVE_L, TEMP3, TEMP2
    
KeybSetOnLongNoCarry:    
    
    movf    TIMER_SAVE_H, w
    movlw   0xFF
    btfss   STATUS, Z		    ; time > 0xff?
    movwf   TIMER_SAVE_L
    
    ; calculated time in TIMER_SAVE_L
    ;bcf	    STATUS, C
    ;rrf	    TIMER_SAVE_L, w

    
    rrf	    TIMER_SAVE_L
    bcf	    TIMER_SAVE_L, 7
;movlw b'01110000'
;andwf	TIMER_SAVE_L
    movf    TIMER_SAVE_L, w
    
    txrx_tx_w
    ;txrx_tx_w_blocking
    led_tx_on
    return

    
;*******************************************
; Set single key to OFF-LONG   
; uses KEYB_CURKEY, KEYB_CURBANK_ABS
KEYB_SET_KEY_OFFLONG
    btfss   GLOBAL_FLAGS, GLOBAL_FLAG_NOTEOFF_VELOCITY
    return
    
    movf    KEYB_CURKEY, w
    addwf   KEYB_CURBANK_ABS, w	    ; w = absolute key index
    movwf   TEMP1

    iorlw   b'10000000'		    ; key = 10XXXXXX
    txrx_tx_w
    ;txrx_tx_w_blocking

    ; calculate time
    banksel TIMER_SAVE_H
    movf    TIMER_H, w
    movwf   TIMER_SAVE_H
    movwf   TEMP3
    
    movf    TIMER_L, w
    movwf   TIMER_SAVE_L
    movwf   TEMP2

    get_array KEYB_TIMERS_H, TEMP1
    movwf   TIMESTAMP_H
    get_array KEYB_TIMERS_L, TEMP1
    movwf   TIMESTAMP_L

    sub16 TIMER_SAVE_H, TIMER_SAVE_L, TIMESTAMP_H, TIMESTAMP_L 
    btfsc   STATUS, C		    ; timestamp > timer?  (carry is negated, see sub16)
    goto    KeybSetOffLongNoCarry
    movlw   0xFF		    ; timestamp = 0xffff - timestamp + timer 
    movwf   TIMER_SAVE_H
    movwf   TIMER_SAVE_L
    sub16 TIMER_SAVE_H, TIMER_SAVE_L, TIMESTAMP_H, TIMESTAMP_L
    add16 TIMER_SAVE_H, TIMER_SAVE_L, TEMP3, TEMP2
    
KeybSetOffLongNoCarry:    
    
    movf    TIMER_SAVE_H, w
    movlw   0xFF
    btfss   STATUS, Z		    ; time > 0xff?
    movwf   TIMER_SAVE_L
    
    ; calculated time in TIMER_SAVE_L
    ;bcf	    STATUS, C
    ;rrf	    TIMER_SAVE_L, w

    
    rrf	    TIMER_SAVE_L
    bcf	    TIMER_SAVE_L, 7
movlw b'01110000'
andwf	TIMER_SAVE_L
    movf    TIMER_SAVE_L, w
    
    txrx_tx_w
    ;txrx_tx_w_blocking
    ;led_tx_on
    return
 
    
;*******************************************
; Set single key to OFF  
; uses KEYB_CURKEY, KEYB_CURBANK_ABS
KEYB_SET_KEY_OFF
    movf    KEYB_CURKEY, w
    addwf   KEYB_CURBANK_ABS, w	    ; w = absolute key index
    movwf   TEMP1
    
    btfss   GLOBAL_FLAGS, GLOBAL_FLAG_NOTEOFF_VELOCITY
    goto    KeybSetKeyOffNoVelocity
    
    ; velocity, save the timestamp
    set_array KEYB_TIMERS_H, TEMP1, TIMER_H
    set_array KEYB_TIMERS_L, TEMP1, TIMER_L
    return
    
KeybSetKeyOffNoVelocity:  
   
    movf    TEMP1, w
    iorlw   b'10000000'		    ; key = 10XXXXXX
    txrx_tx_w
    ;txrx_tx_w_blocking

    txrx_tx 0	

    ;txrx_tx_blocking 0
    ;led_tx_on
    return

    
;*******************************************
; Scan keyboard bits (repeat for antibounce)   
;
; set KEYB_CURADDR_AND according to:
;    0xFE = ON switch
;    0xFF = ON-LONG switch
KEYB_SCAN_BITS
    ; prepare indirect address
    movlw   KEYB_BITS
    movwf   FSR
    bankisel KEYB_BITS

    ; prepare bank counter
    movlw   8
    movwf   KEYB_CURBANK_COUNTER
    clrf    KEYB_CURBANK

KeybScanBitsBank:

    ; set address b0000XXXY
    movf    KEYB_CURBANK, w	
    movwf   KEYB_CURADDR
    rlf	    KEYB_CURADDR
    bsf	    KEYB_CURADDR, 0
    movf    KEYB_CURADDR_AND, w
    andwf   KEYB_CURADDR
    
    ; output address to PORTA
    movf    KEYB_CURADDR, w
    banksel PORTA
    movwf   PORTA

    ; wait 2us in order to discarge the pull-ups
    nop
    nop
    nop
    nop
    nop

    nop
    nop
    nop
    nop
    nop
    
    ; get keys status by PORTB
    comf    PORTB, w	    ; not

    #ifdef PROGRAMMER
	movwf	TEMP1
	movlw	b'00111111'
	andwf	TEMP1, w    ; remove upper 2 bits (used by programmer)
    #endif
    
;movwf TEMP1
;movf  TEMP1
;btfsc STATUS, Z
;goto ZZ
;movf TEMP1, w    
;txrx_tx_w_blocking    
;ZZ

;movf KEYB_CURBANK, w
;txrx_tx_w_blocking    
;delay_5dec    

    ; save bits status
    movwf   INDF
    
    incf    FSR		    ; next KEYB_BITS offset
    
    incf    KEYB_CURBANK
    decfsz  KEYB_CURBANK_COUNTER
    goto    KeybScanBitsBank

    ; Start debouncing
    #if KEYB_DEBOUNCE_SCAN_CYCLES == 0
	return
    #endif
    
    movlw   KEYB_DEBOUNCE_SCAN_CYCLES
    movwf   TEMP2

KeybScanBitsDebounce:

    ; prepare indirect address
    movlw   KEYB_BITS
    movwf   FSR

    ; prepare bank counter
    movlw   8
    movwf   KEYB_CURBANK_COUNTER
    clrf    KEYB_CURBANK
    
KeybScanBitsBankDebounce:

    ; set address b0000XXXY
    movf    KEYB_CURBANK, w	
    movwf   KEYB_CURADDR
    rlf	    KEYB_CURADDR
    bsf	    KEYB_CURADDR, 0
    movf    KEYB_CURADDR_AND, w
    andwf   KEYB_CURADDR
    
    ; output address to PORTA
    movf    KEYB_CURADDR, w
    banksel PORTA
    movwf   PORTA

    ; get keys status by PORTB
    comf    PORTB, w	    ; not

    #ifdef PROGRAMMER
	movwf	TEMP1
	movlw	b'00111111'
	andwf	TEMP1, w    ; remove upper 2 bits (used by programmer)
    #endif

    ; save bits status (OR)
    iorwf   INDF
    
    incf    FSR		    ; next KEYB_BITS offset

    incf    KEYB_CURBANK
    decfsz  KEYB_CURBANK_COUNTER
    goto    KeybScanBitsBankDebounce

    decfsz  TEMP2
    goto    KeybScanBitsDebounce

    return

    END