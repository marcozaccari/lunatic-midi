#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>

#include "../lib/i2c.h"
#include "keyboard.h"


i2c_t i2c_keyboard;

#define BUFFER_SIZE 256
keyb_t keys_buffer[BUFFER_SIZE];
unsigned int keys_buffer_head;
unsigned int keys_buffer_tail;


keyb_t last_key;

void parse(uint8_t b) {
	uint8_t keyb_key_abs, keyb_key;
	bool keyb_keyon;

	if ((b & 0x80) == 0x80) {
		// key
		keyb_keyon = (b & 0x40) == 0x40;

		keyb_key_abs = (b & 0x3F);
		keyb_key = keyb_key_abs + keyboard_key_offset;

		last_key.key = keyb_key;

		if (keyb_keyon)
			last_key.state = KEY_ON;
		else
			last_key.state = KEY_OFF;

	} else {
		// velocity
		last_key.velocity = 127-b+1;

		keys_buffer[keys_buffer_head] = last_key;
      keys_buffer_head++;
	}
}



void keyboard_debug() {
	uint8_t buffer[256];
	int size;

	if (!i2c_read(&i2c_keyboard, buffer, 1)) {
		printf("Cannot read keyboard\r\n");
		return;
	}

	size = buffer[0];
	if (size) {
		printf("KEYB [%u] = ", size);

		if (!i2c_read(&i2c_keyboard, buffer, size+1)) {
			printf("Cannot read keyboard\r\n");
			printf("\r\n");
			return;
		} else {
			for (int k=1; k < size+1; k++) {
				if (buffer[k] == 0xFF)
					printf("- ");
				else
					printf("%x ", buffer[k]);
			}
			printf("\r\n");
		}
	}
}

bool keyboard_do() {
   #if defined(__arm__)
	uint8_t buffer[256];
	int size;

	if (!i2c_read(&i2c_keyboard, buffer, 1))
		return false;

	size = buffer[0];

	if (size) {
		if (!i2c_read(&i2c_keyboard, buffer, size+1))
			return false;
		else {
			for (int k=1; k < size+1; k++) {
				if (buffer[k] != 0xFF)
					parse(buffer[k]);
			}
		}
	}
   #endif
	return true;
}

bool keyboard_init() {
	uint8_t buffer[10];

	if (!i2c_open(&i2c_keyboard, 0x30)) {
		//printf("Cannot open keyboard\r\n");
		return false;
	}
	//printf("Keyboard opened with handle %u\r\n", i2c_keyboard.file);

	// disable velocity on note-off
	buffer[0] = 0x80;
	i2c_write(&i2c_keyboard, buffer, 1);

	// default settings
	keys_buffer_head = 0;
	keys_buffer_tail = 0;

	last_key.key = -1;

	keyboard_key_offset = KEYBOARD_DEFAULT_KEY_OFFSET;
   
	return true;
}

bool keyboard_done() {
	return i2c_close(&i2c_keyboard);
}
