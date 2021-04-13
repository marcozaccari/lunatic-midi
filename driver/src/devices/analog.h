#ifndef _ANALOG_H
#define _ANALOG_H

#include "devices.h"

#define ANALOG_CHANNELS 4

typedef enum {
	ANALOG_CHANNEL_SLIDER = 0,
	ANALOG_CHANNEL_RIBBON = 1
} analog_channel_type_t;

#define ANALOG_BUFFER_SIZE 256

typedef struct analog_value_tag {
	int channel;
	int value;
} analog_value_t;


typedef struct analog_tag {
	device_t *base;

	analog_channel_type_t channels_type[ANALOG_CHANNELS];

	analog_value_t analog_buffer[ANALOG_BUFFER_SIZE];
	int analog_buffer_head;
	int analog_buffer_tail;

	unsigned int last_values[ANALOG_CHANNELS];

	bool continous_sampling;
	
	int state_machine;

	bool (*init)(struct analog_tag *self);
	bool (*done)(struct analog_tag *self);
	bool (*work)(struct analog_tag *self);

} analog_t;

analog_t* new_device_analog(char *name, int i2c_address);

#endif
