#ifndef _BUTTONS_H
#define _BUTTONS_H

#include "devices.h"

#define BUTTONS_COUNT 128

typedef struct buttons_tag {
	device_t *base;

    bool buttons[BUTTONS_COUNT];

	bool (*init)(struct buttons_tag *self);
	bool (*done)(struct buttons_tag *self);
	bool (*work)(struct buttons_tag *self);

} buttons_t;

buttons_t* new_device_buttons(char *name, int i2c_address);

#endif
