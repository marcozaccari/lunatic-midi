#include "libs/gpio.h"
#include "debug.h"

#define DEBUG_LED_GPIO 25  // Pin 22

void debug_init() {
	gpio_set_pin_to_output(DEBUG_LED_GPIO);
}

void debug_led_on() {
    gpio_output(DEBUG_LED_GPIO, 1);
}

void debug_led_off() {
    gpio_output(DEBUG_LED_GPIO, 0);
}
