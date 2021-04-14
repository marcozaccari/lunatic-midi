#ifndef _DEVICE_MODULE_BUTTONS_H
#define _DEVICE_MODULE_BUTTONS_H

#include "../types.h"
#include "../devices.h"
#include "../events.h"

#define MAX_BUTTONS_EVENTS 32

typedef struct buttons_tag {
	device_t *base;

    button_event_t events[MAX_BUTTONS_EVENTS];
	int events_count;

	bool (*init)(struct buttons_tag *self);
	bool (*done)(struct buttons_tag *self);

	bool (*work)(struct buttons_tag *self);
	int (*get_events)(struct buttons_tag *self, button_event_t *events);

} buttons_t;

buttons_t* new_device_buttons(char *name, int i2c_address);

#endif
