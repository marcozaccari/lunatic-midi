#ifndef _ANALOG_H
#define _ANALOG_H

#include "devices.h"

#define STATES_MAX 3
enum states {
	READ_CHAN0 = 0,
	READ_CHAN1 = 1,
	READ_CHAN2 = 2,
	READ_CHAN3 = 3
};

typedef struct analog_tag {
	device_t *base;

	unsigned int ribbon_value;
	unsigned int slider_value[3];

	bool continous_sampling;
	
	int state_machine;

	bool (*init)(struct analog_tag *self);
	bool (*done)(struct analog_tag *self);
	bool (*work)(struct analog_tag *self);

} analog_t;

analog_t* new_device_analog(char *name, int i2c_address);

#endif
