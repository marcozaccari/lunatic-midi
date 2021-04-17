#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libs/log.h"
#include "ipc/ipc.h"
#include "threads.h"


void console_change_log_mode(bool restart_to_debug) {
	if (restart_to_debug) 
		log_min_level = _LOG_DEBUG;
	else {
		log_min_level++;
		log_min_level %= LOGLEVEL_MAX;
	}

	dlog(_LOG_TERMINAL, "Logging level: %s", loglevel_name[log_min_level]);
}

void log_cb(char* s) {
	ipc_debug_send(s);
}

static int console_keyboard_char_count = 0;

static void console_keyboard_do() {
	unsigned char c;
	//int *test_crash;
	
	if (kbhit()) {
		c = getchar();

		switch (c) {
			case 'q':
				threads_request_stop();
				break;

			case 'l':
				console_change_log_mode(console_keyboard_char_count == 0);
				break;
				
			/*case '1':
				dlog(_LOG_TERMINAL, "test note 40");
				midi_note_on(40, 0x80);
				sleep(1);
				midi_note_off(40, 0);
				break;*/
				
			/*case '/':
				// simulate crash
				test_crash = 13;
				*test_crash = 13;
				break;*/
				
			default:
				dlog(_LOG_TERMINAL, "pressed char %c [#%u]", c, c);
		}

		console_keyboard_char_count++;
	}
}

bool console_init() {
	client_log_callback = log_cb;

	return true;
}

void console_done() {
}

void console_work() {
	if (terminal_active)
		console_keyboard_do();
}
