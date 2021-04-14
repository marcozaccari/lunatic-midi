#ifndef _DEVICES_WORKER_H
#define _DEVICES_WORKER_H

#include "devices.h"

bool worker_init();
void worker_done();

// Blocking
void* worker_thread();
// Stop blocking worker
void worker_thread_stop();

extern volatile bool worker_terminated;

#endif
