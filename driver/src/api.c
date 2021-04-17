#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libs/log.h"
#include "ipc/ipc.h"
#include "settings.h"
#include "console.h"
#include "api.h"

static volatile bool terminate_request;
volatile bool api_thread_terminated = true;

static inline void work();
static bool ipc_parse_command(int client_id, const char *msg);


bool api_init() {
	api_thread_terminated = false;

	if (settings.ipc_port) {
		if (!ipc_init(&ipc_parse_command, settings.ipc_port, 5))
			return false;
	}	

	return true;
}

void api_done() {
	ipc_done();
}

void* api_thread_start() {
	terminate_request = false;

	dlog(_LOG_NOTICE, "API thread started"); 

	while (!terminate_request) {
		work();
	}

	api_thread_terminated = true;
	return NULL;
}

void api_thread_stop() {
	terminate_request = true;
}

static inline void work() {
	ipc_work();

	//dlog(_LOG_TERMINAL, "API thread fired!");
}

static bool ipc_parse_command(int client_id, const char *msg) {
	if (strcmp(msg, "debug") == 0) {
		if (ipc_debug_connected())
			console_change_log_mode(false);
		else {
			ipc_set_debug_client(client_id);
			console_change_log_mode(true);
		}

		ipc_debug_send("Logging level: %s\n", loglevel_name[log_min_level]);
		return false;
	}
   
	//ipc_send(client_id, (char*)msg);
	
	return false;
}
