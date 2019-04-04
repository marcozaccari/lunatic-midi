/* TODO: 
 * - IPC
 * - emulazione devices via terminale
 * - scheduler threads
 * - MIDI
 * - ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <wait.h>

#include "../lib/utils.h"
#include "../lib/exceptions.h"
#include "../lib/log.h"

#include "globals.h"
#include "main.settings.h"
#include "console.h"
#include "threads.h"


void show_logo(bool version_only){
	dlog(_LOG_TERMINAL, "Lunatic MIDI-Controller v.%s\n", MIDI_CONTROLLER_VERSION);
	
	if (!version_only){
		dlog(_LOG_TERMINAL, 
 "             _..-´|\n"
 "       _..-´´_..-´|\n"
 "      |_..-´´     |          |\\\n"
 "      |           |          | \\\n"
 "      |           |          | ´\n"
 "      |        __ |          |\n"
 "      |      ,d88b|          |\n"
 "   __ |      88888|       __ |\n"
 " ,d88b|      `Y88P'     ,d88b|\n"
 " 88888|                 88888|\n"
 " `Y88P'                 `Y88P'\n");
	}
}

void list_args(){
	show_logo(true);
	dlog(_LOG_TERMINAL, "usage:");
	dlog(_LOG_TERMINAL, " midi-controller          - normal start");
	dlog(_LOG_TERMINAL, " midi-controller debug    - start in debug mode (no fork)");
	dlog(_LOG_TERMINAL, " midi-controller stop     - terminate the daemon");
	dlog(_LOG_TERMINAL, " midi-controller config=filename - override config file");
}

void signal_handler(int signo) {
   //signal(signo, SIG_IGN);

   switch (signo) {
      case SIGTERM:
         dlog(_LOG_WARNING, "Received SIGTERM");
         should_terminate = true;
         break;
         
      case SIGINT:
         //signal(SIGINT, signal_handler);
         dlog(_LOG_WARNING, "Received SIGINT");
         should_terminate = true;
         break;

      case SIGQUIT:
         dlog(_LOG_WARNING, "Received SIGQUIT");
         should_terminate = true;
         break;
   }
}


void start_controller(){
   if (terminal_active)
      set_terminal_non_canonical();  // need to re-set on crash too
	
	log_post_init();
	
	dlog(_LOG_NOTICE, "[MAIN] Starting MIDI-Controller v.%s ...", MIDI_CONTROLLER_VERSION);
	
   char pid_s[16];

   int pid_file = open(settings.pid_file, O_CREAT | O_RDWR, 0666);
   int rc = flock(pid_file, LOCK_EX | LOCK_NB);
   if (rc){
      if (EWOULDBLOCK == errno)
         exit(EXIT_FAILURE);
   }

   sprintf(pid_s, "%ld\n", (long)getpid());
   write(pid_file, pid_s, strlen(pid_s) + 1);
   
   should_terminate = false;
   
   signal(SIGTERM, signal_handler);
   signal(SIGQUIT, signal_handler);
   signal(SIGINT, signal_handler);
   
   if (!threads_start())
      exit(EXIT_FAILURE);

   if (!console_loop())  // main loop
      exit(EXIT_FAILURE);
   
	dlog(_LOG_NOTICE, "[MAIN] Terminating...");

   threads_stop();

	log_done();

	close(pid_file);
	unlink(settings.pid_file);

	exit(EXIT_SUCCESS);
}

void check_params(int argc, char *argv[],
						bool* debug, 
						bool* cmd_start, 
						bool* cmd_stop){
	int k;
	char *char_ptr;

	if (argc > 0){
		for (k = 0; k < argc; k++){
			if ((strcmp(argv[k], "help") == 0) ||
				 (strcmp(argv[k], "-help") == 0) ||
				 (strcmp(argv[k], "--help") == 0) ||
				 (strcmp(argv[k], "h") == 0) ||
				 (strcmp(argv[k], "/?") == 0) ||
				 (strcmp(argv[k], "?") == 0) ||
				 (strcmp(argv[k], "/help") == 0) ||
				 (strcmp(argv[k], "/h") == 0)){
				list_args();
				exit(0);

			}else	if (strcmp(argv[k], "stop") == 0){
				*cmd_stop = true;
				*cmd_start = false;

			}else	if (strcmp(argv[k], "debug") == 0){
				*debug = true;

			}else	if (strncmp(argv[k], "config=", 7) == 0){
				char_ptr = argv[k];
				strcpy(settings_filename, &char_ptr[7]);
			}
		}

		if (*cmd_stop && *debug){
			list_args();
			exit(0);
		}
	}
}

void start_supervisor(bool debug){
   pid_t cpid, w;
   int status;
   bool restart;

   do{
      //dlog(_LOG_TERMINAL, "fork...");

      if (debug)
         restart = false;
      else{
         cpid = fork();
         if (cpid == -1){
            dlog(_LOG_ERROR, "[MAIN] Cannot fork process");
            exit(EXIT_FAILURE);
         }
      }

      if (debug || (cpid == 0)){
         // **** CHILD PROCESS ****
         master_process = false;
         start_controller();
         break;

      }else{
         // **** MASTER PROCESS ****

         master_process = true;
         //log_done_master();

         restart = false;
         do{
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
            if (w == -1){
               dlog(_LOG_ERROR, "[MAIN] *** MIDI-Controller (master) waitpid error");
               exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status)){
               if (WEXITSTATUS(status) == EXIT_FAILURE){
                  // self crash
                  restart = true;
                  dlog(_LOG_ERROR, "[MAIN] *** exited, status %d", WEXITSTATUS(status));
               }else
                  dlog(_LOG_WARNING, "[MAIN] *** exited, status %d", WEXITSTATUS(status));

            }else if (WIFSIGNALED(status)){
               if (WTERMSIG(status) == 11){
                  // Segmentation fault
               }
               restart = true;
               dlog(_LOG_ERROR, "[MAIN] *** killed by signal %d", WTERMSIG(status));

            }else if (WIFSTOPPED(status)){
               dlog(_LOG_WARNING, "[MAIN] *** stopped by signal %d", WSTOPSIG(status));

            }else if (WIFCONTINUED(status)){
               dlog(_LOG_WARNING, "[MAIN] *** continued");
            }

         }while (!WIFEXITED(status) && !WIFSIGNALED(status));

         if (restart){
            dlog(_LOG_ERROR, "[MAIN] *** crash! Restart...");
            sleep(5);
         }
      }
   }while (restart);
}

int main(int argc, char *argv[]){
	bool cmd_start = true;
	bool cmd_stop = false;
	bool debug = false;
	
   //This will force malloc to use mmap instead of sbrk(which cause fragmented memory that can't be returned to OS also if freed)
   //if size is over the specified threshold (4096 = the minimum page size=memory wasted in case of mmap)
   mallopt(M_MMAP_THRESHOLD, 4096);
        
	exceptions_init();
    
	log_init();
	
	strcpy(settings_filename, "midi-controller.ini");
	check_params(argc, argv, &debug, &cmd_start, &cmd_stop);

	if (cmd_start)
		show_logo(false);
	
	if (debug)
		printf("- DEBUG mode ON -\r\n");
	
	if (terminal_active && !cmd_stop)
		printf("--- Keyboard: use 'q' for quit, 'l' to change log level\r\n");
	
	if (!load_ini_settings(cmd_start)){
		return 2;
	}
	
   char pid_s[16];
   pid_t pid;
   FILE* fd;

   int pid_file = open(settings.pid_file, O_CREAT | O_RDWR, 0666);
   if (!pid_file){
      dlog(_LOG_ERROR, "PID file '%s' error: %s", settings.pid_file, strerror(errno));
      exit(1);
   }

   int rc = flock(pid_file, LOCK_EX | LOCK_NB);
   if (rc != 0){
      if (EWOULDBLOCK == errno){
         // another instance is running

         if (cmd_start){
            list_args();
            printf("\n");
            dlog(_LOG_TERMINAL, "MIDI-Controller already started");
            exit(1);
         }

         if (cmd_stop){
            fd = fopen(settings.pid_file, "r");
            if (fd){
               dlog(_LOG_TERMINAL, "stopping MIDI-Controller");
               fgets(pid_s, 16, fd);
               fclose(fd);
               pid = atol(pid_s);
               kill(pid, SIGQUIT);
               exit(0);
            }
         }
      }else{
         dlog(_LOG_ERROR, "PID lock '%s' error: %s", settings.pid_file, strerror(errno));
      }
   }else{
      // this is the first instance
      close(pid_file);
      unlink(settings.pid_file);

      if (cmd_stop){
         dlog(_LOG_TERMINAL, "MIDI-Controller not started");
      }

      if (cmd_start){
         start_supervisor(debug);
         exit(EXIT_SUCCESS);
      }
   }

	return EXIT_SUCCESS;
}
