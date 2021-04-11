#ifndef _LED_MONITOR_H
#define _LED_MONITOR_H

#include <stdint.h>
#include "devices.h"

#define LED_OFF 	0
#define LED_RED 	1
#define LED_GREEN	2
#define LED_YELLOW	3
#define LED_BLUE	4
#define LED_VIOLET	5
#define LED_CYAN	6
#define LED_WHITE	7

#define MAX_SEND_BYTES_PER_LOOP 16   // 16*8*2.5 = 320us

#define LED_COUNT 60

typedef struct ledstrip_tag {
	device_t *base;

    uint8_t framebuffer[LED_COUNT];
    uint8_t framebuffer_last[LED_COUNT];

    bool (*init)(struct ledstrip_tag *self);
	bool (*done)(struct ledstrip_tag *self);
	bool (*work)(struct ledstrip_tag *self);

    void (*fill)(struct ledstrip_tag *self, uint8_t color);
    void (*set)(struct ledstrip_tag *self, unsigned int index, uint8_t color);
    
} ledstrip_t;

ledstrip_t* new_device_ledstrip(char *name, int i2c_address);

#endif
