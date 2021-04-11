#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>

#include "../libs/i2c.h"
#include "keyboard.h"

static void parse(keyboard_t *self, uint8_t b) {
	uint8_t keyAbs, key;
	bool keyOn;

	if ((b & 0x80) == 0x80) {
		// key
		keyOn = (b & 0x40) == 0x40;

		keyAbs = (b & 0x3F);
		key = keyAbs + self->key_offset;

		self->last_key.key = key;

		if (keyOn)
			self->last_key.state = KEY_ON;
		else
			self->last_key.state = KEY_OFF;

	} else {
		// velocity
		self->last_key.velocity = 127-b+1;

		self->keys_buffer[self->keys_buffer_head] = self->last_key;
		self->keys_buffer_head++;
	}
}

static bool work(keyboard_t *self) {
	uint8_t buffer[256];
	int size = 0;

	if (!i2c_read(&self->base->i2c, buffer, 1))
		return false;

	#if defined(__arm__)
	size = buffer[0];
	#endif

	if (size) {
		if (!i2c_read(&self->base->i2c, buffer, size+1))
			return false;
		else {
			for (int k=1; k < size+1; k++) {
				if (buffer[k] != 0xFF)
					parse(self, buffer[k]);
			}
		}
	}

	return true;
}

static bool init(keyboard_t *self) {

	uint8_t buffer[10];

	if (!i2c_open(&self->base->i2c, self->base->i2c_address)) {
		//printf("Cannot open keyboard\r\n");
		return false;
	}
	//printf("Keyboard opened with handle %u\r\n", i2c_keyboard.file);

	// disable velocity on note-off
	buffer[0] = 0x80;
	i2c_write(&self->base->i2c, buffer, 1);

	// default settings
	self->keys_buffer_head = 0;
	self->keys_buffer_tail = 0;

	self->last_key.key = -1;

	return true;
}

static bool done(keyboard_t *self) {
	return i2c_close(&self->base->i2c);
}

keyboard_t* new_device_keyboard(char *name, int i2c_address, int key_offset) {
	keyboard_t* keyb = malloc(sizeof(keyboard_t));

	keyb->base = new_device(i2c_address, name, DEVICE_KEYBOARD, keyb);

	keyb->key_offset = key_offset;
	//self->key_offset = KEYBOARD_DEFAULT_KEY_OFFSET;

	keyb->init = &init;
	keyb->work = &work;
	keyb->done = &done;

	return keyb;
}
