#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>
//#include <unistd.h>

#include "../lib/i2c.h"
#include "buttons.h"


i2c_t i2c_buttons;

#define BUTTONS_COUNT 128

bool buttons[BUTTONS_COUNT];


void buttons_debug() {
	char buffer[256];
	int size;

	if (!i2c_read(&i2c_buttons, buffer, 1)) {
		printf("Cannot read buttons\r\n");
		return;
	}

	size = buffer[0];
	if (size) {
		printf("BUT [%u] = ", size);

		if (!i2c_read(&i2c_buttons, buffer, size+1)) {
			printf("Cannot read buttons\r\n");
			printf("\r\n");
			return;
		} else {
			for (int k=1; k < size+1; k++) {
				if (buffer[k] == 0xFF)
					printf("- ");
				else
					printf("%x ", buffer[k]);
			}
			printf("\r\n");
		}
	}
}

bool buttons_do() {
	char buffer[256];
	int size;
	uint8_t b;

	if (!i2c_read(&i2c_buttons, buffer, 1))
		return false;

	size = buffer[0];

	if (size) {
		if (!i2c_read(&i2c_buttons, buffer, size+1))
			return false;
		else {
			for (int k=1; k < size+1; k++) {
				if (buffer[k] != 0xFF) {
					b = buffer[k];
					if ((b & 0x80) == 0x80) {
						b = b & 0x7F;
						buttons[b] = true;
					} else {
						buttons[b] = false;
					}
				}
			}
		}
	}

	return true;
}

bool buttons_init() {
	//char buffer[10];

	if (!i2c_open(&i2c_buttons, 0x31)) {
		//printf("Cannot open buttons\r\n");
		return false;
	}

	/*buffer[0] = 0x80;
	buffer[1] = 0x81;
	buffer[2] = 0x82;
	buffer[3] = 0x83;
	buffer[4] = 0x84;
	buffer[5] = 0x85;
	buffer[6] = 0x86;
	buffer[7] = 0x87;
	i2c_write(&i2c_buttons, buffer, 8);*/

	return true;
}

bool buttons_done() {
	return i2c_close(&i2c_buttons);
}

