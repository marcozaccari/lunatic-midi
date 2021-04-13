#ifndef _BUTTONS_H
#define _BUTTONS_H

#include "devices.h"

#define BUTTONS_BUFFER_SIZE 256

typedef enum {
	BUTTON_OFF = 0,
	BUTTON_ON  = 1
} button_state_t;

typedef struct button_tag {
	int button;
	button_state_t state;
} button_t;

typedef struct buttons_tag {
	device_t *base;

    button_t buttons_buffer[BUTTONS_BUFFER_SIZE];
	int buttons_buffer_head;
	int buttons_buffer_tail;

	bool (*init)(struct buttons_tag *self);
	bool (*done)(struct buttons_tag *self);
	bool (*work)(struct buttons_tag *self);

} buttons_t;

buttons_t* new_device_buttons(char *name, int i2c_address);

#endif
