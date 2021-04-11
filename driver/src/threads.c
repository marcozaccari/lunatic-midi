#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "libs/log.h"
#include "threads.h"
#include "scheduler.h"
#include "api.h"


pthread_t thread_scheduler;
pthread_t thread_api;

volatile bool threads_terminate_request = false;


bool threads_start() {
	int err;

	if (!scheduler_init_tasks())
		return false;

	err = pthread_create(&thread_scheduler, NULL, &scheduler_thread_start, NULL);
	if (err == 0)
		dlog(_LOG_TERMINAL, "Devices thread created");
	else {
		dlog(_LOG_ERROR, "Can't create devices thread: %s", strerror(err));
		return false;
	}

	if (!api_init())
		return false;
		
	err = pthread_create(&thread_api, NULL, &api_thread_start, NULL);
	if (err == 0)
		dlog(_LOG_TERMINAL, "API thread created");
	else {
		dlog(_LOG_ERROR, "Can't create API thread: %s", strerror(err));
		return false;
	}
	
	return true;
}

void threads_request_stop() {
	scheduler_thread_stop();
	api_thread_stop();

	threads_terminate_request = true;
}

void threads_stop() {
	int timeout_secs = 5;

	dlog(_LOG_TERMINAL, "Waiting for threads ending (%d secs timeout)", timeout_secs);
	bool all_terminated = false;

	threads_request_stop();
	
	for (int i=0; i<timeout_secs; i++) {
		if (scheduler_terminated && api_thread_terminated) {
			all_terminated = true;
			break;
		}

		sleep(1);
	}
	
	if (all_terminated)
		dlog(_LOG_INFO, "All threads terminated");
	else
		dlog(_LOG_WARNING, "Reached threads timeout: forcing ending");
}
