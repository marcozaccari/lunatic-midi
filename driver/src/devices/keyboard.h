#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "devices.h"

#define KEYBOARD_DEFAULT_KEY_OFFSET 48
#define KEYS_BUFFER_SIZE 256

typedef enum {
	KEY_OFF = 0,
	KEY_ON  = 1
} keyb_state_t;

typedef struct key_tag {
	int key;
	keyb_state_t state;
	unsigned int velocity;
} keyb_t;

typedef struct keyboard_tag {
	device_t *base;

	unsigned int key_offset;

	keyb_t keys_buffer[KEYS_BUFFER_SIZE];
	int keys_buffer_head;
	int keys_buffer_tail;

	keyb_t last_key;

	bool (*init)(struct keyboard_tag *self);
	bool (*done)(struct keyboard_tag *self);
	bool (*work)(struct keyboard_tag *self);

} keyboard_t;

keyboard_t* new_device_keyboard(char *name, int i2c_address, int key_offset);

#endif
