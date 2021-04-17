#ifndef _DEVICE_MODULE_LEDSTRIP_H
#define _DEVICE_MODULE_LEDSTRIP_H

#include "../types.h"
#include "../devices.h"

#define LED_COUNT 60

typedef struct ledstrip_tag {
	device_t *base;

    led_color_t framebuffer[LED_COUNT];
    led_color_t framebuffer_last[LED_COUNT];

    bool (*init)(struct ledstrip_tag *self);
	bool (*done)(struct ledstrip_tag *self);
	bool (*work)(struct ledstrip_tag *self);

    void (*fill)(struct ledstrip_tag *self, led_color_t color);
    void (*set)(struct ledstrip_tag *self, int index, led_color_t color);

} ledstrip_t;

ledstrip_t* new_device_ledstrip(const char *name, int i2c_address);

#endif
