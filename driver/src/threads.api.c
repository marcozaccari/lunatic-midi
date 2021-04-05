#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/log.h"
#include "main.settings.h"
#include "threads.h"
#include "globals.h"


bool api_thread_init() {
   return true;
}

void* api_thread() {
   for (;;) {
      //dlog(_LOG_DEBUG, "API thread fired!");
      sleep(3);
      
      if (should_terminate)
         break;
   }
   
   threads_terminated[API_THREAD] = true;
   return NULL;
}
