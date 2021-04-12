#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libs/log.h"
#include "api.h"

static volatile bool terminate_request;
volatile bool api_thread_terminated = true;

static inline void work();


bool api_init() {

	api_thread_terminated = false;
	
	return true;
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
	//dlog(_LOG_TERMINAL, "API thread fired!");
	sleep(3);
}
