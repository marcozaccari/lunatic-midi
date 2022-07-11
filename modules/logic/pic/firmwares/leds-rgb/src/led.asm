#include <p16f886.inc>
#include "lib/misc.inc"
#include "lib/txrx.inc"

    
#define WS2812_PORTA_PIN 0   
    
;#define RED_DEFAULT_VALUE 0x40    
;#define GREEN_DEFAULT_VALUE 0x40    
;#define BLUE_DEFAULT_VALUE 0x30    

#define RED_DEFAULT_VALUE 0xff    
#define GREEN_DEFAULT_VALUE 0xff    
#define BLUE_DEFAULT_VALUE 0xff   
    
#define STRIP_LEDS_COUNT d'60'    
    
    GLOBAL LED_INIT
    GLOBAL LED_OUTPUT
    GLOBAL INIT_LEDS_TO_PALETTE_IDX
    
    GLOBAL LED_DEMO_DO

    GLOBAL RED_ON_VALUE
    GLOBAL GREEN_ON_VALUE
    GLOBAL BLUE_ON_VALUE
    
    GLOBAL LEDS_PALETTE
    
    
;*******************************************************************************
;    VARIABLES                                                                 *
;*******************************************************************************
    
GRP_LED		UDATA_SHR

CUR_LED_IDX	RES 1

RED_VALUE	RES 1
GREEN_VALUE	RES 1
BLUE_VALUE	RES 1  

GRP_LED_PALETTE    UDATA
	
RED_ON_VALUE	RES 1	
GREEN_ON_VALUE	RES 1	
BLUE_ON_VALUE	RES 1	
PALETTE_VALUE	RES 1	
	
LEDS_PALETTE	RES d'61'	; byte 60 = safe index (for protocol overflow)
	
	
GRP_LED_DEMO	UDATA
CUR_DEMO_IDX	RES 1
CUR_DEMO_COL	RES 1	


;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

ws2812_output_0_long macro
    bsf PORTA, WS2812_PORTA_PIN
    bcf PORTA, WS2812_PORTA_PIN
    nop
    nop
    nop
    nop
    endm

ws2812_output_1_long macro
    bsf PORTA, WS2812_PORTA_PIN
    nop
    nop
    bcf PORTA, WS2812_PORTA_PIN
    nop
    nop
    endm
    
ws2812_output_0 macro
    bsf PORTA, WS2812_PORTA_PIN
    bcf PORTA, WS2812_PORTA_PIN
    endm

ws2812_output_1 macro
    bsf PORTA, WS2812_PORTA_PIN
    nop
    nop
    bcf PORTA, WS2812_PORTA_PIN
    endm
    
    ; convert PALETTE_VALUE (RGB bits) to RED_VALUE, GREEN_VALUE, BLUE_VALUE
    ; according to *_ON_VALUE
palette_to_values macro
    ;banksel PALETTE_VALUE
    
    clrf    RED_VALUE
    movf   RED_ON_VALUE, w
    btfsc   PALETTE_VALUE, 0	; test R
    movwf   RED_VALUE 
    
    clrf    GREEN_VALUE
    movf    GREEN_ON_VALUE, w
    btfsc   PALETTE_VALUE, 1	; test G
    movwf   GREEN_VALUE 

    clrf    BLUE_VALUE
    movf    BLUE_ON_VALUE, w
    btfsc   PALETTE_VALUE, 2	; test B
    movwf   BLUE_VALUE 
    endm

    
;*******************************************************************************
;    SUBROUTINES                                                               *
;*******************************************************************************
LED CODE
    
LED_INIT
    banksel TRISA
    bcf	    TRISA, WS2812_PORTA_PIN
    
    banksel PORTA
    bcf	    PORTA, WS2812_PORTA_PIN
    
    ; load default values
    banksel RED_ON_VALUE
    
    movlw   RED_DEFAULT_VALUE
    movwf   RED_ON_VALUE
    
    movlw   GREEN_DEFAULT_VALUE
    movwf   GREEN_ON_VALUE
    
    movlw   BLUE_DEFAULT_VALUE
    movwf   BLUE_ON_VALUE
    
    ; init palette indeces to zero
    movlw   0
    call    INIT_LEDS_TO_PALETTE_IDX
    
    clrf    CUR_DEMO_IDX
    movlw   1
    movwf   CUR_DEMO_COL
    
    ;call    LED_OUTPUT
    ;return

LED_OUTPUT
    ; ws2812 reset
    movlw   0x35          ; ~50 us
    ;led_debug_on
LED_RESET_DELAY
    nop

    addlw   -1
    btfss   STATUS, Z
    goto    LED_RESET_DELAY
    ;led_debug_off

    ; init leds index
    movlw   STRIP_LEDS_COUNT
    movwf   CUR_LED_IDX
    decf    CUR_LED_IDX
    
SEND_NEXT_LED
    ; get current led palette index
    get_array LEDS_PALETTE, CUR_LED_IDX
    banksel PALETTE_VALUE
    movwf   PALETTE_VALUE
    ; convert palette index to raw values
    palette_to_values
    
    ; send raw values
    banksel PORTA
    call    SEND_RGB
    
    #ifdef USE_USART
    txrx_do
    #endif
    
    ; update counters
    ;bcf	    STATUS, C
    decf    CUR_LED_IDX
    btfss   STATUS, C
    goto    SEND_NEXT_LED
    return
    
SEND_RGB
    ; test *_VALUES and send to ws2812

SEND_BIT23
    btfsc   GREEN_VALUE, 7
    goto    SEND_BIT23_1
SEND_BIT23_0
    ws2812_output_0
    goto    SEND_BIT22
SEND_BIT23_1
    ws2812_output_1

SEND_BIT22
    btfsc   GREEN_VALUE, 6
    goto    SEND_BIT22_1
SEND_BIT22_0
    ws2812_output_0
    goto    SEND_BIT21
SEND_BIT22_1
    ws2812_output_1

SEND_BIT21
    btfsc   GREEN_VALUE, 5
    goto    SEND_BIT21_1
SEND_BIT21_0
    ws2812_output_0
    goto    SEND_BIT20
SEND_BIT21_1
    ws2812_output_1

SEND_BIT20
    btfsc   GREEN_VALUE, 4
    goto    SEND_BIT20_1
SEND_BIT20_0
    ws2812_output_0
    goto    SEND_BIT19
SEND_BIT20_1
    ws2812_output_1

SEND_BIT19
    btfsc   GREEN_VALUE, 3
    goto    SEND_BIT19_1
SEND_BIT19_0
    ws2812_output_0
    goto    SEND_BIT18
SEND_BIT19_1
    ws2812_output_1
    
SEND_BIT18
    btfsc   GREEN_VALUE, 2
    goto    SEND_BIT18_1
SEND_BIT18_0
    ws2812_output_0
    goto    SEND_BIT17
SEND_BIT18_1
    ws2812_output_1

SEND_BIT17
    btfsc   GREEN_VALUE, 1
    goto    SEND_BIT17_1
SEND_BIT17_0
    ws2812_output_0
    goto    SEND_BIT16
SEND_BIT17_1
    ws2812_output_1

SEND_BIT16
    btfsc   GREEN_VALUE, 0
    goto    SEND_BIT16_1
SEND_BIT16_0
    ws2812_output_0
    goto    SEND_BIT15
SEND_BIT16_1
    ws2812_output_1

SEND_BIT15
    btfsc   RED_VALUE, 7
    goto    SEND_BIT15_1
SEND_BIT15_0
    ws2812_output_0
    goto    SEND_BIT14
SEND_BIT15_1
    ws2812_output_1

SEND_BIT14
    btfsc   RED_VALUE, 6
    goto    SEND_BIT14_1
SEND_BIT14_0
    ws2812_output_0
    goto    SEND_BIT13
SEND_BIT14_1
    ws2812_output_1

SEND_BIT13
    btfsc   RED_VALUE, 5
    goto    SEND_BIT13_1
SEND_BIT13_0
    ws2812_output_0
    goto    SEND_BIT12
SEND_BIT13_1
    ws2812_output_1

SEND_BIT12
    btfsc   RED_VALUE, 4
    goto    SEND_BIT12_1
SEND_BIT12_0
    ws2812_output_0
    goto    SEND_BIT11
SEND_BIT12_1
    ws2812_output_1

SEND_BIT11
    btfsc   RED_VALUE, 3
    goto    SEND_BIT11_1
SEND_BIT11_0
    ws2812_output_0
    goto    SEND_BIT10
SEND_BIT11_1
    ws2812_output_1

SEND_BIT10
    btfsc   RED_VALUE, 2
    goto    SEND_BIT10_1
SEND_BIT10_0
    ws2812_output_0
    goto    SEND_BIT9
SEND_BIT10_1
    ws2812_output_1

SEND_BIT9
    btfsc   RED_VALUE, 1
    goto    SEND_BIT9_1
SEND_BIT9_0
    ws2812_output_0
    goto    SEND_BIT8
SEND_BIT9_1
    ws2812_output_1

SEND_BIT8
    btfsc   RED_VALUE, 0
    goto    SEND_BIT8_1
SEND_BIT8_0
    ws2812_output_0
    goto    SEND_BIT7
SEND_BIT8_1
    ws2812_output_1

SEND_BIT7
    btfsc   BLUE_VALUE, 7
    goto    SEND_BIT7_1
SEND_BIT7_0
    ws2812_output_0
    goto    SEND_BIT6
SEND_BIT7_1
    ws2812_output_1

SEND_BIT6
    btfsc   BLUE_VALUE, 6
    goto    SEND_BIT6_1
SEND_BIT6_0
    ws2812_output_0
    goto    SEND_BIT5
SEND_BIT6_1
    ws2812_output_1

SEND_BIT5
    btfsc   BLUE_VALUE, 5
    goto    SEND_BIT5_1
SEND_BIT5_0
    ws2812_output_0
    goto    SEND_BIT4
SEND_BIT5_1
    ws2812_output_1

SEND_BIT4
    btfsc   BLUE_VALUE, 4
    goto    SEND_BIT4_1
SEND_BIT4_0
    ws2812_output_0
    goto    SEND_BIT3
SEND_BIT4_1
    ws2812_output_1

SEND_BIT3
    btfsc   BLUE_VALUE, 3
    goto    SEND_BIT3_1
SEND_BIT3_0
    ws2812_output_0
    goto    SEND_BIT2
SEND_BIT3_1
    ws2812_output_1

SEND_BIT2
    btfsc   BLUE_VALUE, 2
    goto    SEND_BIT2_1
SEND_BIT2_0
    ws2812_output_0
    goto    SEND_BIT1
SEND_BIT2_1
    ws2812_output_1

SEND_BIT1
    btfsc   BLUE_VALUE, 1
    goto    SEND_BIT1_1
SEND_BIT1_0
    ws2812_output_0
    goto    SEND_BIT0
SEND_BIT1_1
    ws2812_output_1

SEND_BIT0
    btfsc   BLUE_VALUE, 0
    goto    SEND_BIT0_1
SEND_BIT0_0
    ws2812_output_0
    return
SEND_BIT0_1
    ws2812_output_1
    return


    ; init led palette indeces to w
INIT_LEDS_TO_PALETTE_IDX    
    movwf   TEMP1
    movlw   STRIP_LEDS_COUNT
    movwf   CUR_LED_IDX
    decf    CUR_LED_IDX
INIT_NEXT_LED
    set_array LEDS_PALETTE, CUR_LED_IDX, TEMP1
    decf    CUR_LED_IDX
    btfss   STATUS, C
    goto    INIT_NEXT_LED
    return


LED_DEMO_DO
    banksel CUR_DEMO_IDX
    incf    CUR_DEMO_IDX
    
    movlw   d'255'
    subwf   CUR_DEMO_IDX, w
    btfss   STATUS, Z
    goto    DemoPaint

DemoNextColor:    
    clrf    CUR_DEMO_IDX
    incf    CUR_DEMO_COL
    movlw   8
    subwf   CUR_DEMO_COL, w
    btfss   STATUS, Z
    goto    DemoPaint
    
DemoRestart:
    movlw   1
    movwf   CUR_DEMO_COL
    
DemoPaint:    
    movlw   0
    call INIT_LEDS_TO_PALETTE_IDX
    
    banksel CUR_DEMO_IDX
    movlw   d'60'
    subwf   CUR_DEMO_IDX, w
    btfsc   STATUS, C
    goto    DemoSend

    set_array LEDS_PALETTE, CUR_DEMO_IDX, CUR_DEMO_COL

DemoSend:    
    call LED_OUTPUT
    return
    
    END
