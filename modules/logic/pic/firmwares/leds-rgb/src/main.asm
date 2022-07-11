;*******************************************************************************
; LED WS2812b controller
; by Marco Zaccari info@marcozaccari.com
;
;    Read from USART, write to 60 leds                                         
;
;    Protocol:                                                                 
;    1xxxxxxx Command:                                                         
;     1111111 Reset (all leds off, invalidate led index)                       
;     1xxxxxx Tune RGB:                                                        
;      00nnnn  R will be set to nnnn0000                                       
;      01nnnn  G will be set to nnnn0000                                       
;      10nnnn  B will be set to nnnn0000                                       
;     0iiiiii Set led index to i (00..59)                                      
;                                                                              
;    0----xxx Set led by index:                                                
;         B    B bit on                                                        
;          G   G bit on                                                        
;           R  R bit on   
;    
;    01------ Repaint request (confirm updates)
;             NB: please wait at least 2.5ms after repaint request
;
;
; LEGAL NOTICE:
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;*******************************************************************************

#include <p16f886.inc>
#include "main.inc"
#include "lib/misc.inc"    
#include "lib/txrx.inc"
#include "led.inc"

    
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

GRP_MAIN    UDATA_SHR
;TIMER_H	    RES 1
;TIMER_L	    RES 1	; global timer (1ms/2ms)

;W_TEMP	    RES 1	; for ISR context switching
;STATUS_TEMP RES 1	; for ISR context switching

NEED_REPAINT	RES 1
    
    
GLOBAL_FLAGS	    RES 1 
	    
;GRP_MAIN_MISC    UDATA
;PCLATH_TEMP RES 1
;FSR_TEMP    RES 1 
CMD_LED_INDEX	RES 1
COMMAND		RES 1
 
CMD_TUNE_VALUE	RES 1
	    
USART_RX_BYTE	RES 1	
 
;    GLOBAL TIMER_H
;    GLOBAL TIMER_L
    GLOBAL GLOBAL_FLAGS
 

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
    
    ; initialize USART
    txrx_init

    ; initialize leds
    led_init
    
    ; assure output leds to off
    #ifdef PROGRAMMER
    delay_1sec
    led_on
    led_output
    delay_5dec
    led_off
    led_output
    delay_5dec
    led_on
    led_output
    delay_5dec
    led_off
    led_output
    delay_5dec
    led_on
    led_output
    delay_5dec
    led_off
    led_output
    delay_5dec
    led_on
    led_output
    #endif
    led_on

    ; set watchdog
    banksel INTCON
    bcf	    INTCON, T0IE	; disable interrupt
    banksel TMR0
    clrwdt
    clrf    TMR0		; clear TMR0 and prescaler
    banksel OPTION_REG
    bsf     OPTION_REG, PSA	; select WDT
    clrwdt
    bcf     OPTION_REG, T0CS	; fosc/4 input
    movlw   b'11111000'		; mask prescaler bits
    andwf   OPTION_REG, w
    iorlw   b'00000111'		; 1:128 prescaler
    movwf   OPTION_REG		; 1/(fosc/4/prescaler)*256 = 6,5ms
    
    
START
    movlw   0x3C
    movwf   CMD_LED_INDEX	; led index to led 60 (null led)
    movlw   0			; all leds off
    led_init_palette_w
    bsf	    NEED_REPAINT, 0
        
    #ifdef PROGRAMMER
    goto    DemoEnd
    #endif
DEMO
    clrwdt
    led_demo_do
    
DemoDelay:
    movlw   0x20
    movwf   TEMP2
DemoDelayReset:
    movlw   0x40
    movwf   TEMP1
DemoDelayLoop:
    txrx_do
    txrx_skip_if_rx_buffer_empty
    goto DemoEnd
    
    decfsz  TEMP1
    goto    DemoDelayLoop
    clrwdt
    decfsz  TEMP2
    goto    DemoDelayReset
    goto DEMO
DemoEnd:    
    movlw   0			; all leds off
    led_init_palette_w
    bsf	    NEED_REPAINT, 0
    
MAIN_LOOP
    
    clrwdt

    txrx_do
    
    txrx_skip_if_rx_buffer_not_empty
    goto    END_RX_DATA

    txrx_get_rx_w
    movwf   USART_RX_BYTE

    
;    led_init_palette_w
;    goto    END_RX_DATA
    
    
    
    btfss   USART_RX_BYTE, 7	; check if command or value (commands >= 0x80)
    goto    PARSE_VALUE

PARSE_COMMAND
    movf    USART_RX_BYTE, w
    andlw   b'01111111'
    movwf   COMMAND

PARSE_COMMAND_LED_INDEX		; check for led index (0x80 + index)
    btfsc   COMMAND, 6		
    goto    PARSE_COMMAND_RESET
    ; set led index
    movf    COMMAND, w		
    movwf   CMD_LED_INDEX
    goto PARSE_END

PARSE_COMMAND_RESET		; check for reset 0xFF
    movlw   0x7F
    subwf   COMMAND, w
    btfss   STATUS, Z
    goto    PARSE_COMMAND_TUNE_RGB
    ; perform reset
    movlw   0			; all leds off
    led_init_palette_w
    bsf	    NEED_REPAINT, 0
    movlw   0x3C
    movwf   CMD_LED_INDEX	; led index to led 60 (null led)
    goto    PARSE_END
    
PARSE_COMMAND_TUNE_RGB		; check for RGB tuning
    movlw   b'00111111'
    andwf   COMMAND, f
    
    movf    COMMAND, w		; parse value (move to high nibble)
    movwf   CMD_TUNE_VALUE
    movlw   b'00001111'		
    andwf   CMD_TUNE_VALUE, f
    rlf     CMD_TUNE_VALUE, f
    rlf     CMD_TUNE_VALUE, f
    rlf     CMD_TUNE_VALUE, f
    rlf     CMD_TUNE_VALUE, f
    ;movlw   b'11110000'
    ;andwf   CMD_TUNE_VALUE, f
    movlw   b'00001111'
    iorwf   CMD_TUNE_VALUE, f
    
    ; check for R/G/B
    movlw   b'00110000'
    andwf   COMMAND, f
    btfsc   STATUS, Z
    goto    PARSE_COMMAND_TUNE_R
    
    movlw   b'00010000'
    subwf   COMMAND, w
    btfsc   STATUS, Z
    goto    PARSE_COMMAND_TUNE_G
    
PARSE_COMMAND_TUNE_B		; tune B
    banksel BLUE_ON_VALUE
    movf    CMD_TUNE_VALUE, w
    movwf   BLUE_ON_VALUE
    goto    PARSE_END
    
PARSE_COMMAND_TUNE_G		; tune G
    banksel GREEN_ON_VALUE
    movf    CMD_TUNE_VALUE, w
    movwf   GREEN_ON_VALUE
    goto    PARSE_END
    
PARSE_COMMAND_TUNE_R		; tune R
    banksel RED_ON_VALUE
    movf    CMD_TUNE_VALUE, w
    movwf   RED_ON_VALUE
    goto    PARSE_END
    
PARSE_VALUE    
    btfsc   USART_RX_BYTE, 6	; check if repaint value
    goto    PARSE_REPAINT
    
    led_set_index_value CMD_LED_INDEX, USART_RX_BYTE
    goto PARSE_END

PARSE_REPAINT
    bsf	    NEED_REPAINT, 0
    goto    END_RX_DATA
    
PARSE_END    
    goto    MAIN_LOOP		; check for more data
    
END_RX_DATA    

    btfss   NEED_REPAINT, 0
    goto MAIN_LOOP
    
    ;led_debug_on
    led_output		; 2.2ms
    ;led_debug_off    
    bcf	    NEED_REPAINT, 0

 ;   banksel PORTA
 ;   led_debug_on
 ;   delay_5dec
 ;   led_debug_off
 ;   delay_5dec
    
 ;   led_debug_on
 ;   led_debug_off
    
    goto MAIN_LOOP

  END
