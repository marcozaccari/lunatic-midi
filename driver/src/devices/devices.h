#ifndef _DEVICES_H
#define _DEVICES_H

#include "../libs/i2c.h"

enum device_type {
	DEVICE_KEYBOARD,
	DEVICE_BUTTONS,
	DEVICE_ANALOG,
	DEVICE_LEDSTRIP
};

// Device base class
typedef struct device_tag {
	char name[STR_MAXSIZE];
	int type;

	i2c_t i2c;
	int i2c_address;

	void *obj;

} device_t;

// Allocate a new device, add to the devices internal list 
// and return initialized device object.
device_t* new_device(int i2c_address, const char *name, int type, void *obj);

// Get internal devices.
int get_devices_count();
device_t* get_device(int index);

#endif
