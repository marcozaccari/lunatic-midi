#ifndef _DEVICE_MODULE_ANALOG_H
#define _DEVICE_MODULE_ANALOG_H

#include "../types.h"
#include "../devices.h"
#include "../events.h"

#define ANALOG_CHANNELS 4

typedef enum {
	ANALOG_CHANNEL_SLIDER = 0,
	ANALOG_CHANNEL_RIBBON = 1
} analog_channel_type_t;

typedef struct analog_tag {
	device_t *base;

	analog_channel_type_t channels_type[ANALOG_CHANNELS];

	analog_event_t events[4];
	int events_count;

	int last_values[ANALOG_CHANNELS];

	bool continous_sampling;
	
	bool (*init)(struct analog_tag *self);
	bool (*done)(struct analog_tag *self);

	bool (*work)(struct analog_tag *self);
	int (*get_events)(struct analog_tag *self, analog_event_t *events);

	void (*set_channel_type)(struct analog_tag *self, 
		int channel, analog_channel_type_t type);

} analog_t;

analog_t* new_device_analog(char *name, int i2c_address);

#endif
