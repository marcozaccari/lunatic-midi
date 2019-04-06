#include <stdio.h>
#include <unistd.h>  // UNIX Standard Definitions
#include <errno.h>   // ERROR Number Definitions
#include <stdbool.h>
#include <stdint.h>

#include "../lib/i2c.h"
#include "led_monitor.h"

#define LEDS_COUNT 60

i2c_t i2c_led;

uint8_t framebuffer[LEDS_COUNT];
uint8_t framebuffer_old[LEDS_COUNT];


bool led_monitor_init() {
   if (!i2c_open(&i2c_led, 0x33)) {
      //printf("Cannot open leds\r\n");
      return false;
   }

	for (int x=0; x < LEDS_COUNT; x++) {
		framebuffer[x] = 0;
		framebuffer_old[x] = 0;
	}

	// reset leds controller
	uint8_t buff[1];

	buff[0] = 0xFF;
	i2c_write(&i2c_led, buff, 1);

	return true;
}

bool led_monitor_done() {
	return i2c_close(&i2c_led);
}


bool led_monitor_do() {
	uint8_t buff[256];
	int buff_len;

	buff_len = 0;
	for (int x=0; x < LEDS_COUNT; x++) {
		if (framebuffer[x] != framebuffer_old[x]) {
			buff[buff_len] = (uint8_t)x | 0x80;
			buff_len++;
			buff[buff_len] = framebuffer[x];
			buff_len++;
			
			framebuffer_old[x] = framebuffer[x];

			if (buff_len >= MAX_SEND_BYTES_PER_LOOP)
				break;
		}
	}

	if (buff_len) {
		buff[buff_len] = 0x40;  // repaint command
		buff_len++;
		i2c_write(&i2c_led, buff, buff_len);
	}

	return true;
}


void led_monitor_fill(uint8_t color) {
	for (int x=0; x < LEDS_COUNT; x++)
		framebuffer[x] = color;
}

void led_monitor_set(unsigned int index, uint8_t color) {
	if (index >= LEDS_COUNT)
		return;

	framebuffer[index] = color;
}