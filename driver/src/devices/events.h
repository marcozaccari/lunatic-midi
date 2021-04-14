#ifndef _DEVICES_EVENTS_H
#define _DEVICES_EVENTS_H

#include "types.h"

typedef struct keyboard_event_tag {
	int key;
	keyb_state_t state;
	int velocity;
} keyboard_event_t;

typedef struct button_event_tag {
	int button;
	button_state_t state;
} button_event_t;

typedef struct analog_event_tag {
	int channel;
	int value;
} analog_event_t;

typedef struct ledstrip_event_tag {
	int index;
	led_color_t color;
} ledstrip_event_t;

int pop_keyboard_events(keyboard_event_t* events);
void enqueue_keyboard_events(keyboard_event_t* events, int count);

int pop_buttons_events(button_event_t* events);
void enqueue_buttons_events(button_event_t* events, int count);

int pop_analog_events(analog_event_t* events);
void enqueue_analog_events(analog_event_t* events, int count);

#endif
