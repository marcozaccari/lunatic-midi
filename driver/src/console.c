#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libs/log.h"
#include "ipc/ipc.h"
#include "threads.h"
#include "settings.h"


static void change_log_mode(bool reset) {
	if (reset) 
		log_min_level = _LOG_DEBUG;
	else {
		log_min_level++;
		log_min_level %= LOGLEVEL_MAX;
	}

	dlog(_LOG_TERMINAL, "Logging level: %s", loglevel_name[log_min_level]);
}

void log_cb(char* s) {
   if (ipc_debug_connected())
      ipc_debug_send(s);
}

bool ipc_parse_command(int client_id, const char *msg) {
   if (strcmp(msg, "debug") == 0) {
      if (ipc_debug_connected())
         change_log_mode(false);
      else {
         ipc_set_debug_client(client_id);
         change_log_mode(true);
      }

      ipc_debug_send("Logging level: %s\n", loglevel_name[log_min_level]);
      return false;
   }
   
   ipc_send(client_id, (char*)msg);
   return false;
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
				change_log_mode(console_keyboard_char_count == 0);
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

	if (settings.ipc_port) {
		if (!ipc_init(&ipc_parse_command, settings.ipc_port, 10))
			return false;
	}	

	return true;
}

void console_done() {
	ipc_done();
}

void console_work() {
	if (terminal_active)
		console_keyboard_do();
}
