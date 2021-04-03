#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/log.h"
#include "../lib/midi.h"
#include "main.settings.h"
#include "threads.h"
#include "globals.h"


bool midi_thread_init() {
   strcpy(midi_portname, settings.midi_portname);
   
   if (!midi_init())
      return false;
   
   return true;
}

void* midi_thread() {
   for (;;) {
      dlog(_LOG_DEBUG, "MIDI thread fired!");
      sleep(3);
      
      if (should_terminate)
         break;
   }
   
   midi_done();

   threads_terminated[MIDI_THREAD] = true;
   return NULL;
}
