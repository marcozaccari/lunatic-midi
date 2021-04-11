#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "devices/devices.h"

bool scheduler_init_tasks();
void scheduler_done_tasks();

// Blocking worker
void* scheduler_thread_start();
// Stop blocking worker
void scheduler_thread_stop();

extern volatile bool scheduler_terminated;

#endif
