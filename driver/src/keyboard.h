#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "../lib/utils.h"

#define KEYBOARD_DEFAULT_KEY_OFFSET 48
unsigned int keyboard_key_offset;


typedef enum {
	KEY_OFF = 0,
	KEY_ON  = 1
} keyb_state_t;

typedef struct key_tag {
	int key;
	keyb_state_t state;
	unsigned int velocity;
} keyb_t;



bool keyboard_init(int i2c_address);
bool keyboard_done();

void keyboard_debug();
bool keyboard_do();

#endif
