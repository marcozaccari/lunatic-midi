    EXTERN LED_INIT
    EXTERN LED_OUTPUT
    EXTERN INIT_LEDS_TO_PALETTE_IDX

    EXTERN LED_DEMO_DO
    
    EXTERN RED_ON_VALUE
    EXTERN GREEN_ON_VALUE
    EXTERN BLUE_ON_VALUE
    
    EXTERN LEDS_PALETTE

	
;*******************************************************************************
;    MACROS                                                                    *
;*******************************************************************************

led_init macro
    call LED_INIT
    endm
    
led_output macro
    call LED_OUTPUT
    endm
    
    ; initialize led palette indeces to w
led_init_palette_w macro
    call INIT_LEDS_TO_PALETTE_IDX
    endm
    
    ; set led_index (0..59) as led_palette_value (0..7)
led_set_index_value macro led_index, led_palette_value
    movf    led_index, w
    sublw   d'60'
    movlw   d'60'
    btfss   STATUS, C
    movwf   led_index	    ; overflow -> set index to 60
 
    set_array LEDS_PALETTE, led_index, led_palette_value
    endm
    
led_demo_do macro
    call LED_DEMO_DO
    endm