/* Timer accuracy: https://yosh.ke.mu/article/raspberry_pi_getting_time
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../lib/log.h"
#include "../lib/gpio.h"
#include "main.settings.h"
#include "threads.h"
#include "globals.h"

#include "keyboard.h"
#include "buttons.h"
#include "led_monitor.h"
#include "adc.h"


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


bool devices_thread_init() {
   if (!keyboard_init()) {
      dlog(_LOG_ERROR, "Cannot open Keyboard");
      return false;
   }
   
   if (!buttons_init()) {
      dlog(_LOG_ERROR, "Cannot open Buttons");
      keyboard_done();
      return false;
   }
      
   if (!led_monitor_init()) {
      dlog(_LOG_ERROR, "Cannot open Led Monitor");
      buttons_done();
      keyboard_done();
      return false;
   }

   if (!adc_init()) {
      dlog(_LOG_ERROR, "Cannot open ADC");
      buttons_done();
      keyboard_done();
      led_monitor_done();
      return false;
   }

   return true;
}

/** Measure usleep() internal delay
 * 
 * @return int
 */
int tune_usleep() {
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
      time_diff += 1000000000;				//(Rolls over every 1 second)
      

   elapsed_us = time_diff / 1000;

   adjust_us = (tests*1000 - elapsed_us) / tests;

   dlog(_LOG_DEBUG, "usleep() adjust = %dus", adjust_us);
	
   return adjust_us;
}


void* devices_thread() {
	struct timespec gettime_now;
   long int start_time;
	long int time_diff;
   int elapsed_us;
   int remain_us;
   int adjust_us = tune_usleep();
   
   int cur_task;
   
   for (;;) {
      gpio_output(DEBUG_LED_GPIO, 1);

      clock_gettime(CLOCK_REALTIME, &gettime_now);
      start_time = gettime_now.tv_nsec;
      
      
      cur_task++;
      cur_task %= SCHEDULER_TASKS_MAX;
      
      switch (tasks_scheduler[cur_task]) {
         case KEYBOARD_TASK:
            keyboard_do();
            break;

         case BUTTONS_TASK:
            buttons_do();
            break;

         case LED_MONITOR_TASK:
            led_monitor_do();
            break;

         case ADC_TASK:
            adc_do();
            break;

      }
      
      if (should_terminate)
         break;


      clock_gettime(CLOCK_REALTIME, &gettime_now);
		time_diff = gettime_now.tv_nsec - start_time;
		if (time_diff < 0)
			time_diff += 1000000000;				//(Rolls over every 1 second)
      elapsed_us = time_diff / 1000;

      remain_us = TIMESLICES_US - elapsed_us + adjust_us;
      if (remain_us > 0) {
         gpio_output(DEBUG_LED_GPIO, 0);
         usleep(remain_us);
      }
   }

   buttons_done();
   keyboard_done();
   led_monitor_done();
   adc_done();
   
   threads_terminated[DEVICES_THREAD] = true;
   return NULL;
}
