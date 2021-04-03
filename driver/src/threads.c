#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../lib/log.h"
#include "main.settings.h"
#include "threads.h"
#include "threads.devices.h"
#include "threads.midi.h"


pthread_t threads[THREADS_MAX];


bool threads_start() {
   int err;

   if (!devices_thread_init())
      return false;
   threads_terminated[DEVICES_THREAD] = false;
   err = pthread_create(&threads[DEVICES_THREAD], NULL, &devices_thread, NULL);
   if (err == 0)
      dlog(_LOG_TERMINAL, "Devices thread created");
   else {
      dlog(_LOG_ERROR, "Can't create devices thread: %s", strerror(err));
      return false;
   }

   if (!midi_thread_init())
      return false;
   threads_terminated[MIDI_THREAD] = false;
   err = pthread_create(&threads[MIDI_THREAD], NULL, &midi_thread, NULL);
   if (err == 0)
      dlog(_LOG_TERMINAL, "MIDI thread created");
   else {
      dlog(_LOG_ERROR, "Can't create MIDI thread: %s", strerror(err));
      return false;
   }
   
   return true;
}

void threads_stop() {
   int timeout_secs = 5;

   dlog(_LOG_TERMINAL, "Waiting for threads ending (%d secs timeout)", timeout_secs);
   bool all_terminated;
   
   for (int i=0; i<timeout_secs; i++) {
      all_terminated = true;
      for (int k=0; k<THREADS_MAX; k++) {
         if (!threads_terminated[k]) {
            all_terminated = false;
            break;
         }
      }
      if (all_terminated)
         break;
         
      sleep(1);
   }
   
   if (all_terminated)
      dlog(_LOG_INFO, "All threads terminated");
   else
      dlog(_LOG_WARNING, "Reached threads timeout: forcing ending");
}
