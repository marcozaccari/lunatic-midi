#include <stdlib.h>
#include <string.h>
#include "devices.h"

#define MAX_DEVICES 32

device_t *devices[MAX_DEVICES];
int devices_count = 0;

static void add_device(void *device) {
    devices[devices_count] = device;
    devices_count++;
}

int get_devices_count() {
    return devices_count;
}

device_t* get_device(int index) {
    return devices[index];
}

device_t* new_device(int i2c_address, const char *name, int type, void *obj) {
	device_t* dev = malloc(sizeof(device_t));

    dev->obj = obj;

    strcpy(dev->name, name);
    dev->type = type;
    
    dev->i2c_address = i2c_address;

    add_device(dev);
    
    return dev;
}
