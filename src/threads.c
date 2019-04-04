#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../lib/log.h"
#include "main.settings.h"
#include "threads.h"

#define THREADS_MAX 2

enum threads_names {
   MAIN_THREAD,
   DEVICES_THREAD,
};

pthread_t threads[THREADS_MAX];

bool should_terminate;
bool threads_terminated[THREADS_MAX];


void* main_thread() {
   threads_terminated[MAIN_THREAD] = false;

   for (;;) {
      dlog(_LOG_DEBUG, "Main thread fired!");
      sleep(3);
      
      if (should_terminate)
         break;
   }

   threads_terminated[MAIN_THREAD] = true;
   return NULL;
}

void* devices_thread() {
   threads_terminated[DEVICES_THREAD] = false;

   for (;;) {
      dlog(_LOG_DEBUG, "Devices thread fired!");
      sleep(1);

      if (should_terminate)
         break;
   }

   threads_terminated[DEVICES_THREAD] = true;
   return NULL;
}


bool threads_start() {
   int err;

   err = pthread_create(&threads[DEVICES_THREAD], NULL, &devices_thread, NULL);
   if (err == 0)
      dlog(_LOG_TERMINAL, "Devices thread created");
   else {
      dlog(_LOG_ERROR, "Can't create devices thread: %s", strerror(err));
      return false;
   }

   err = pthread_create(&threads[MAIN_THREAD], NULL, &main_thread, NULL);
   if (err == 0)
      dlog(_LOG_TERMINAL, "Main thread created");
   else {
      dlog(_LOG_ERROR, "Can't create main thread: %s", strerror(err));
      return false;
   }
   
   return true;
}

void threads_stop() {
   dlog(_LOG_TERMINAL, "Waiting for threads ending (10 secs timeout)");
   bool all_terminated;
   
   for (int i=0; i<10; i++) {
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
      dlog(_LOG_TERMINAL, "All thread terminated");
   else
      dlog(_LOG_TERMINAL, "Reached timeout: forcing ending");
}
