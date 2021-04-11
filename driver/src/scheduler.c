// Timer accuracy: https://yosh.ke.mu/article/raspberry_pi_getting_time

#include <unistd.h>
#include <time.h>
#include <string.h>

#include "libs/log.h"

#include "debug.h"
#include "scheduler.h"

#include "devices/devices.h"
#include "devices/keyboard.h"
#include "devices/buttons.h"
#include "devices/analog.h"
#include "devices/ledstrip.h"

// Time window for every task
#define TIMESLICES_US 500

enum tasks {
	KEYBOARD_TASK,
	BUTTONS_TASK,  // 7ms antibounce
	ADC_TASK,
	LED_MONITOR_TASK  // 2.5ms delay
};

#define SCHEDULER_TASKS_MAX 7
int tasks_scheduler[SCHEDULER_TASKS_MAX] = {
	ADC_TASK,
	KEYBOARD_TASK,
	ADC_TASK,
	KEYBOARD_TASK,
	ADC_TASK,
	BUTTONS_TASK,
	LED_MONITOR_TASK,
};

#define MAX_TASKS 64
device_t *tasks[MAX_TASKS];
int tasks_count = 0;
int tasks_cur = 0;

volatile bool terminate_request;
volatile bool scheduler_terminated = true;

int usleep_latency;

static int calc_usleep_latency();
static inline void work();


bool scheduler_init_tasks() {
	bool res;

 	usleep_latency = calc_usleep_latency();

	int devices_count = get_devices_count();

	if (!devices_count) {
		dlog(_LOG_ERROR, "No devices found");
		return false;
	}

	for (int i=0; i<devices_count; i++) {
		device_t *device = get_device(i);

		switch (device->type) {
			case DEVICE_KEYBOARD:
				res = ((keyboard_t*)device->obj)->init(device->obj);
				break;

			case DEVICE_BUTTONS:
				res = ((buttons_t*)device->obj)->init(device->obj);
				break;

			case DEVICE_ANALOG:
				res = ((analog_t*)device->obj)->init(device->obj);
				break;

			case DEVICE_LEDSTRIP:
				res = ((ledstrip_t*)device->obj)->init(device->obj);
				break;
		}

		if (!res) {
			dlog(_LOG_ERROR, "Cannot init device %s (address %s)", 
				device->name, device->i2c_address);
			return false;
		}

		tasks[tasks_count] = device;
		tasks_count++;
	}

	char tasks_s[STR_MAXSIZE];
	for (int i=0; i<tasks_count; i++) {
		device_t *device = tasks[i];

		strcat(tasks_s, device->name);
		if (i < tasks_count -1)
			strcat(tasks_s, ", ");
	}
	dlog(_LOG_NOTICE, "Scheduler tasks: %s", tasks_s); 

	scheduler_terminated = false;

	return true; 
}

void scheduler_done_tasks() {
	int devices_count = get_devices_count();

	for (int i=0; i<devices_count; i++) {
		device_t *device = get_device(i);

		switch (device->type) {
			case DEVICE_KEYBOARD:
				((keyboard_t*)device->obj)->done(device->obj);
				break;

			case DEVICE_BUTTONS:
				((buttons_t*)device->obj)->done(device->obj);
				break;

			case DEVICE_ANALOG:
				((analog_t*)device->obj)->done(device->obj);
				break;

			case DEVICE_LEDSTRIP:
				((ledstrip_t*)device->obj)->done(device->obj);
				break;
		}
	}
}

void* scheduler_thread_start() {
	terminate_request = false;

	dlog(_LOG_NOTICE, "Scheduler thread started"); 

	while (!terminate_request) {
		work();
	}

	scheduler_terminated = true;
	return NULL;
}

void scheduler_thread_stop() {
	terminate_request = true;
}

static inline void work() {
	struct timespec gettime_now;
	long int start_time;
	long int time_diff;
	int elapsed_us;
	int remain_us;
	int usleep_latency = calc_usleep_latency();
	
	debug_led_on();

	clock_gettime(CLOCK_REALTIME, &gettime_now);
	start_time = gettime_now.tv_nsec;
	
	device_t *device = tasks[tasks_cur];
	tasks_cur++;
	tasks_cur %= tasks_count;

	switch (device->type) {
		case DEVICE_KEYBOARD:
			((keyboard_t*)device->obj)->work(device->obj);
			break;

		case DEVICE_BUTTONS:
			((buttons_t*)device->obj)->work(device->obj);
			break;

		case DEVICE_ANALOG:
			((analog_t*)device->obj)->work(device->obj);
			break;

		case DEVICE_LEDSTRIP:
			((ledstrip_t*)device->obj)->work(device->obj);
			break;
	}

	// measure elapsed time of current task
	clock_gettime(CLOCK_REALTIME, &gettime_now);
	time_diff = gettime_now.tv_nsec - start_time;
	if (time_diff < 0)
		time_diff += 1000000000;				//(Rolls over every 1 second)
	elapsed_us = time_diff / 1000;  // nano to micro

	// calculate remaining waiting time in order to complete current time window
	remain_us = TIMESLICES_US - elapsed_us - usleep_latency;
	if (remain_us > 0) {
		debug_led_off();
		usleep(remain_us);
	}
}

/** Measure usleep() internal delay
 * 
 * @return int
 */
static int calc_usleep_latency() {
	struct timespec gettime_now;
	long int start_time;
	long int time_diff;
	int elapsed_us, adjust_us;
	int tests = 100;

	clock_gettime(CLOCK_REALTIME, &gettime_now);
	start_time = gettime_now.tv_nsec;

	for (int k=0; k<tests; k++)
		usleep(1000);

	clock_gettime(CLOCK_REALTIME, &gettime_now);
	time_diff = gettime_now.tv_nsec - start_time;
	if (time_diff < 0)
		time_diff += 1000000000; // Rolls over every 1 second

	elapsed_us = time_diff / 1000;  // nano to micro

	adjust_us = (elapsed_us - tests*1000) / tests;

	dlog(_LOG_DEBUG, "usleep() latency = %dus", adjust_us);
	
	return adjust_us;
}
