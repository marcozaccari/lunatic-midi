#ifndef _DEVICES_TYPES_H
#define _DEVICES_TYPES_H

typedef enum {
    LED_OFF     = 0,
    LED_RED     = 1,
    LED_GREEN	= 2,
    LED_YELLOW	= 3,
    LED_BLUE	= 4,
    LED_VIOLET	= 5,
    LED_CYAN	= 6,
    LED_WHITE	= 7
} led_color_t;

typedef enum {
	KEY_OFF = 0,
	KEY_ON  = 1
} keyb_state_t;

typedef enum {
	BUTTON_OFF = 0,
	BUTTON_ON  = 1
} button_state_t;

#endif
