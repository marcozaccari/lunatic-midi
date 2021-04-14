#ifndef _DEVICE_MODULE_KEYBOARD_H
#define _DEVICE_MODULE_KEYBOARD_H

#include "../types.h"
#include "../devices.h"
#include "../events.h"

//#define KEYBOARD_DEFAULT_KEY_OFFSET 48

#define MAX_KEYBOARD_EVENTS 64

typedef struct keyboard_tag {
	device_t *base;

	int key_offset;

	keyboard_event_t events[MAX_KEYBOARD_EVENTS];
	int events_count;

	keyboard_event_t last_key;

	bool (*init)(struct keyboard_tag *self);
	bool (*done)(struct keyboard_tag *self);

	bool (*work)(struct keyboard_tag *self);
	int (*get_events)(struct keyboard_tag *self, keyboard_event_t *events);

} keyboard_t;

keyboard_t* new_device_keyboard(char *name, int i2c_address, int key_offset);

#endif
