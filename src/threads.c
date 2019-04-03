#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../lib/log.h"
#include "threads.h"


pthread_t t_main;
pthread_t t_devices;
   
void* main_thread() {
   for (;;) {
      dlog(_LOG_DEBUG, "Main thread fired!");
      sleep(3);
   }
}

void* devices_thread() {
   for (;;) {
      dlog(_LOG_DEBUG, "Devices thread fired!");
      sleep(1);
   }
}

bool threads_start() {
   int err;
   
   err = pthread_create(&t_devices, NULL, &devices_thread, NULL);
   if (err == 0)
      dlog(_LOG_TERMINAL, "Devices thread created");
   else {
      dlog(_LOG_ERROR, "Can't create devices thread: %s", strerror(err));
      return false;
   }

   err = pthread_create(&t_main, NULL, &main_thread, NULL);
   if (err == 0)
      dlog(_LOG_TERMINAL, "Main thread created");
   else {
      dlog(_LOG_ERROR, "Can't create main thread: %s", strerror(err));
      return false;
   }
   
   return true;
}