#include <stdio.h>
#include <unistd.h>  // UNIX Standard Definitions
#include <errno.h>   // ERROR Number Definitions
#include <stdbool.h>
#include <stdint.h>

#include "../libs/i2c.h"
#include "ledstrip.h"

static bool init(ledstrip_t *self) {
	if (!i2c_open(&self->base->i2c, self->base->i2c_address)) {
		//printf("Cannot open led\r\n");
		return false;
	}

	for (int x=0; x < LED_COUNT; x++) {
		self->framebuffer[x] = 0;
		self->framebuffer_last[x] = 0;
	}

	// reset led controller
	uint8_t buff[1];

	buff[0] = 0xFF;
	i2c_write(&self->base->i2c, buff, 1);

	return true;
}

static bool done(ledstrip_t *self) {
	return i2c_close(&self->base->i2c);
}

static bool work(ledstrip_t *self) {
	uint8_t buff[256];
	int buff_len;

	buff_len = 0;
	for (int x=0; x < LED_COUNT; x++) {
		if (self->framebuffer[x] != self->framebuffer_last[x]) {
			buff[buff_len] = (uint8_t)x | 0x80;
			buff_len++;
			buff[buff_len] = self->framebuffer[x];
			buff_len++;
			
			self->framebuffer_last[x] = self->framebuffer[x];

			if (buff_len >= MAX_SEND_BYTES_PER_LOOP)
				break;
		}
	}

	if (buff_len) {
		buff[buff_len] = 0x40;  // repaint command
		buff_len++;
		i2c_write(&self->base->i2c, buff, buff_len);
	}

	return true;
}

static void fill(ledstrip_t *self, uint8_t color) {
	for (int x=0; x < LED_COUNT; x++)
		self->framebuffer[x] = color;
}

static void set(ledstrip_t *self, unsigned int index, uint8_t color) {
	if (index >= LED_COUNT)
		return;

	self->framebuffer[index] = color;
}

ledstrip_t* new_device_ledstrip(char *name, int i2c_address) {
	ledstrip_t* led = malloc(sizeof(ledstrip_t));

	led->base = new_device(i2c_address, name, DEVICE_LEDSTRIP, led);

	led->init = &init;
	led->work = &work;
	led->done = &done;

	led->fill = &fill;
	led->set = &set;

	return led;
}
