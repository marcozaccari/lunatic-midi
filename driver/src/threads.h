#ifndef _THREADS_H
#define _THREADS_H

#include "../lib/utils.h"


#define THREADS_MAX 2

enum threads_names {
   MIDI_THREAD,
   DEVICES_THREAD,
};

bool threads_terminated[THREADS_MAX];


bool threads_start();
void threads_stop();

#endif
