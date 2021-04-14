#include "events.h"

#define MAX_EVENTS 256

keyboard_event_t keyboard_events[MAX_EVENTS];
int keyboard_events_head;
int keyboard_events_tail;

button_event_t buttons_events[MAX_EVENTS];
int buttons_events_head;
int buttons_events_tail;

analog_event_t analog_events[MAX_EVENTS];
int analog_events_head;
int analog_events_tail;

ledstrip_event_t ledstrips_events[MAX_EVENTS];
int ledstrips_events_head;
int ledstrips_events_tail;


void enqueue_keyboard_events(keyboard_event_t* events, int count) {
	for (int i=0; i<count; i++) {
		keyboard_events[keyboard_events_head] = events[i];
		keyboard_events_head++;
	}
}

int pop_keyboard_events(keyboard_event_t* events) {
	int count = 0;
	keyboard_event_t* event_ptr = events;

	while (keyboard_events_tail != keyboard_events_head) {
		*event_ptr = keyboard_events[keyboard_events_tail];
		keyboard_events_tail++;

		event_ptr++;
		count++;
	}

	return count;
}

void enqueue_buttons_events(button_event_t* events, int count) {
	for (int i=0; i<count; i++) {
		buttons_events[buttons_events_head] = events[i];
		buttons_events_head++;
	}
}

int pop_buttons_events(button_event_t* events) {
	int count = 0;
	button_event_t* event_ptr = events;

	while (buttons_events_tail != buttons_events_head) {
		*event_ptr = buttons_events[buttons_events_tail];
		buttons_events_tail++;

		event_ptr++;
		count++;
	}

	return count;
}

void enqueue_analog_events(analog_event_t* events, int count) {
	for (int i=0; i<count; i++) {
		analog_events[analog_events_head] = events[i];
		analog_events_head++;
	}
}

int pop_analog_events(analog_event_t* events) {
	int count = 0;
	analog_event_t* event_ptr = events;

	while (analog_events_tail != analog_events_head) {
		*event_ptr = analog_events[analog_events_tail];
		analog_events_tail++;

		event_ptr++;
		count++;
	}

	return count;
}
