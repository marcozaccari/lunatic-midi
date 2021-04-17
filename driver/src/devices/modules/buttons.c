#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>
//#include <unistd.h>

#include "../../libs/i2c.h"
#include "buttons.h"


static bool work(buttons_t *self) {
	uint8_t buffer[256];
	int size = 0;
	uint8_t b;
	button_event_t event;

	self->events_count = 0;

	if (!i2c_read(&self->base->i2c, buffer, 1))
		return false;

	#if defined(__arm__)
	size = buffer[0];
	#endif

	if (!size)
		return true;

	if (!i2c_read(&self->base->i2c, buffer, size+1))
		return false;

	for (int k=1; k < size+1; k++) {
		if (buffer[k] != 0xFF) {
			b = buffer[k];

			if ((b & 0x80) == 0x80) {
				b = b & 0x7F;

				event.button = b;
				event.state = BUTTON_ON;
			} else {
				event.button = b;
				event.state = BUTTON_OFF;
			}

			self->events[self->events_count] = event;
			self->events_count++;
		}
	}

	return true;
}

static bool init(buttons_t *self) {
	uint8_t buffer[10];

	if (!i2c_open(&self->base->i2c, self->base->i2c_address)) {
		//printf("Cannot open buttons\r\n");
		return false;
	}

	// Reset led
	buffer[0] = 0xFF;
	i2c_write(&self->base->i2c, buffer, 1);

	return true;
}

static bool done(buttons_t *self) {
	return i2c_close(&self->base->i2c);
}

static int get_events(buttons_t *self, button_event_t **events) {
	*events = self->events;
	return self->events_count;
}

buttons_t* new_device_buttons(const char *name, int i2c_address) {
	buttons_t* bt = malloc(sizeof(buttons_t));

	bt->base = new_device(i2c_address, name, DEVICE_BUTTONS, bt);

	bt->init = &init;
	bt->done = &done;

	bt->work = &work;
	bt->get_events = &get_events;

	return bt;
}
