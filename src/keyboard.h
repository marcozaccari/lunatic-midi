#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "../lib/utils.h"

#define KEYBOARD_DEFAULT_KEY_OFFSET 48
unsigned int keyboard_key_offset;


typedef enum {
	KEY_OFF = 0,
	KEY_ON  = 1
} key_state_t;

typedef struct key_tag {
	int key;
	key_state_t state;
	unsigned int velocity;
} key_t;



bool keyboard_init();
bool keyboard_done();

void keyboard_debug();
bool keyboard_do();

#endif