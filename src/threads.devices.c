#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/log.h"
#include "main.settings.h"
#include "threads.h"
#include "globals.h"


bool devices_thread_init() {
   return true;
}

void* devices_thread() {
   for (;;) {
      dlog(_LOG_DEBUG, "Devices thread fired!");
      sleep(1);
      
      if (should_terminate)
         break;
   }

   threads_terminated[DEVICES_THREAD] = true;
   return NULL;
}
