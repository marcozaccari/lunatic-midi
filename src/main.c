#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include <stdint.h>

#include "led_monitor.h"
#include "../lib/i2c.h"
#include "../lib/utils.h"



void process_keyboard(uint8_t b) {
	uint8_t keyb_key_abs, keyb_key;
	bool keyb_keyon;
	uint8_t keyb_velocity;

	if ((b & 0x80) == 0x80) {
		// key
		keyb_keyon = (b & 0x40) == 0x40;

		keyb_key_abs = (b & 0x3F);
		keyb_key = keyb_key_abs + 48;

		if (keyb_keyon) {
			led_monitor_set(keyb_key_abs, LED_BLUE);
			printf("K%u ", keyb_key);
		} else {
			led_monitor_set(keyb_key_abs, LED_OFF);
			printf("k%u ", keyb_key);
		}

	} else {
		// velocity
		keyb_velocity = 127-b+1;

		printf("V%u ", keyb_velocity);

		/*if (keyb_keyon)
			midi_note_on(keyb_key, keyb_velocity, b);
		else
			midi_note_off(keyb_key, keyb_velocity);*/
	}
}



i2c_t i2c_keyboard;
i2c_t i2c_buttons;

bool read_keyboard() {
	char buffer[256];
	int size;

	if (!i2c_read(&i2c_keyboard, buffer, 1)) {
		//printf("Cannot read keyboard\r\n");
		return false;
	}

	size = buffer[0];

	if (size) {
		printf("KEYB [%u] = ", size);

		if (!i2c_read(&i2c_keyboard, buffer, size+1)) {
			// printf("Cannot read keyboard\r\n");
			printf("\r\n");
			return false;
		} else {
			for (int k=1; k < size+1; k++) {
				//printf("%x ", buffer[k]);
				if (buffer[k] == 0xFF)
					printf("- ");
				else
					process_keyboard(buffer[k]);
			}
			printf("\r\n");
		}
	}

	return true;
}

bool read_buttons() {
	char buffer[256];
	int size;
	char b;

	if (!i2c_read(&i2c_buttons, buffer, 1)) {
		//printf("Cannot read buttons\r\n");
		return false;
	}

	size = buffer[0];

	if (size) {
		printf("BUT [%u] = ", size);

		if (!i2c_read(&i2c_buttons, buffer, size+1)) {
			// printf("Cannot read buttons\r\n");
			printf("\r\n");
			return false;
		} else {
			for (int k=1; k < size+1; k++) {
				//printf("%x ", buffer[k]);
				if (buffer[k] == 0xFF)
					printf("- ");
				else {
					b = buffer[k];
					if ((b & 0x80) == 0x80) {
						b = b & 0x7F;
						printf("B%u ", b);
					} else
						printf("b%u ", b);
				}
			}
			printf("\r\n");
		}
	}

	return true;
}

int main(int argc, char *argv[]){
	char buffer[10];

	set_terminal_conio_mode();

	//mallopt(M_MMAP_THRESHOLD, 0);

	if (!i2c_open(&i2c_keyboard, 0x30)) {
		printf("Cannot open keyboard\r\n");
		reset_terminal_mode();
		return -1;
	}
	printf("Keyboard opened with handle %u\r\n", i2c_keyboard.file);

	if (!i2c_open(&i2c_buttons, 0x31)) {
		printf("Cannot open buttons\r\n");
		reset_terminal_mode();
		return -1;
	}
	printf("Buttons opened with handle %u\r\n", i2c_buttons.file);

	buffer[0] = 0x80;
	i2c_write(&i2c_keyboard, buffer, 1);

	buffer[0] = 0x80;
	buffer[1] = 0x81;
	buffer[2] = 0x82;
	buffer[3] = 0x83;
	buffer[4] = 0x84;
	buffer[5] = 0x85;
	buffer[6] = 0x86;
	buffer[7] = 0x87;
	i2c_write(&i2c_buttons, buffer, 8);

	if (!led_monitor_open()) {
		printf("Cannot open led monitor\r\n");
	}

	//led_monitor_fill(LED_RED);

	while (!kbhit()) {
		read_keyboard();
		usleep(1000);
		read_buttons();
		read_keyboard();

		//sleep(1);
		//usleep(500000);
		usleep(1000);

		led_monitor_do();
	}

	i2c_close(&i2c_keyboard);
	i2c_close(&i2c_buttons);

	led_monitor_close();

	reset_terminal_mode();

	return EXIT_SUCCESS;
}

