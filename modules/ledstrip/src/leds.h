#ifndef LIGHTS_H
#define	LIGHTS_H

#include <stdint.h>

void lights_init(void);  // must be called after buttons_init()
void lights_worker(void);  // must be called at every main cycle

typedef enum {
    LIGHT_OFF = 0,
    LIGHT_ON,
    LIGHT_FLASH_LOW,
    LIGHT_FLASH_HIGH
} light_state_t;

#define LIGHTS_FLASH_HIGH_MS 200  // High flash frequency

void lights_reset(void);
void light_set(uint8_t light, light_state_t state);

#endif

