#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/log.h"
#include "../lib/ipc.h"
#include "main.settings.h"


void change_log_mode(bool reset) {
   if (reset) 
      log_min_level = _LOG_DEBUG;
   else {
      log_min_level++;
      log_min_level %= LOGLEVEL_MAX;
   }

   dlog(_LOG_TERMINAL, "Logging level: %s", loglevel_name[log_min_level]);
}

void log_cb(char* s) {
   if (ipc_debug_connected())
      ipc_send_debug(s);
}

bool ipc_parse(char *msg, uint8_t client_id) {
   if (strcmp(msg, "debug") == 0) {
      if (ipc_debug_connected())
         change_log_mode(false);
      else {
         ipc_set_debug_client(client_id);
         change_log_mode(true);
      }

      ipc_send_debug("Logging level: %s\n", loglevel_name[log_min_level]);
      return false;
   }
   
   ipc_send(client_id, (char*)msg);
   return false;
}

int console_keyboard_char_count = 0;
void console_keyboard_do() {
   unsigned char c;
   //int *test_crash;
   
   if (kbhit()) {
      c = getchar();

      switch (c) {
         case 'q':
            should_terminate = true;
            break;

         case 'l':
            change_log_mode(console_keyboard_char_count == 0);
            break;
            
         /*case '/':
            // simulate crash
            test_crash = 13;
            *test_crash = 13;
            break;*/
            
         default:
            dlog(_LOG_TERMINAL, "pressed char %c [#%u]", c, c);
      }

      console_keyboard_char_count++;
   }
}

bool console_loop() {
   int ipc_sleep_secs;

   if (terminal_active)
      ipc_sleep_secs = 1;
   else
      ipc_sleep_secs = 5;
   
   if (!ipc_init(ipc_parse, settings.ipc_port, ipc_sleep_secs))
      return false;
   
   client_log_callback = log_cb;
   
   while (!should_terminate) {
      if (terminal_active)
         console_keyboard_do();
      
      if (!ipc_do())
         return false;
   }
   
   ipc_done();
   
   return true;
}