#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <wait.h>
#include <signal.h>

#include "libs/utils.h"
#include "libs/exceptions.h"
#include "libs/log.h"
#include "libs/gpio.h"

#include "midi/midi.h"
#include "ipc/ipc.h"

#include "settings.h"
#include "debug.h"
#include "console.h"
#include "threads.h"

#define DRIVER_VERSION "0.7"

void show_version(){
	dlog(_LOG_TERMINAL, "Lunatic Driver v.%s", DRIVER_VERSION);
}

void list_args(){
	show_version();
	dlog(_LOG_TERMINAL, "usage:");
	dlog(_LOG_TERMINAL, " lunatic-driver [--debug] [--config=filename]");
	dlog(_LOG_TERMINAL, "    debug           - simulate fake inputs");
	dlog(_LOG_TERMINAL, "    config=filename - override config file");
	dlog(_LOG_TERMINAL, "\n");
	dlog(_LOG_TERMINAL, "Diagnostic:");
	dlog(_LOG_TERMINAL, " lunatic-driver midiports   - list MIDI ports");
}

void signal_handler(int signo) {
	//signal(signo, SIG_IGN);

	switch (signo) {
		case SIGTERM:
			dlog(_LOG_WARNING, "Received SIGTERM");
			threads_request_stop();
			break;
			
		case SIGINT:
			//signal(SIGINT, signal_handler);
			dlog(_LOG_WARNING, "Received SIGINT");
			threads_request_stop();
			break;

		case SIGQUIT:
			dlog(_LOG_WARNING, "Received SIGQUIT");
			threads_request_stop();
			break;
	}
}

bool start_driver(bool debug) {
	dlog(_LOG_NOTICE, "[MAIN] Starting Driver...");
	
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);
	
	if (!threads_start()) {
		threads_stop();
		return false;
	}

	// Main loop
	while (!threads_terminate_request)
		console_work();
	
	dlog(_LOG_NOTICE, "[MAIN] Terminating...");

	threads_stop();

	return true;
}

char config_filename[STR_MAXSIZE];

void parse_params(int argc, char *argv[], bool* debug) {
	int k;
	char *char_ptr;

	if (argc <= 0)
		return;

	for (k = 0; k < argc; k++) {
		if ((strcmp(argv[k], "help") == 0) ||
			(strcmp(argv[k], "-help") == 0) ||
			(strcmp(argv[k], "--help") == 0) ||
			(strcmp(argv[k], "h") == 0) ||
			(strcmp(argv[k], "/?") == 0) ||
			(strcmp(argv[k], "?") == 0) ||
			(strcmp(argv[k], "/help") == 0) ||
			(strcmp(argv[k], "/h") == 0)) {

			list_args();
			exit(EXIT_SUCCESS);

		} else	if (strcmp(argv[k], "--debug") == 0) {
			*debug = true;

		} else	if (strncmp(argv[k], "--config=", 9) == 0) {
			char_ptr = argv[k];
			strcpy(config_filename, &char_ptr[9]);

		} else if (strcmp(argv[k], "midiports") == 0){
			print_midi_ports();
			exit(0);
		}
	}
}

int main(int argc, char *argv[]) {
	bool debug = false;

	//This will force malloc to use mmap instead of sbrk(which cause fragmented memory that can't be returned to OS also if freed)
	//if size is over the specified threshold (4096 = the minimum page size=memory wasted in case of mmap)
	//mallopt(M_MMAP_THRESHOLD, 4096);
		  
	exceptions_init();
	 
	log_init();
	
	strcpy(config_filename, "lunatic-driver.ini");
	parse_params(argc, argv, &debug);

	show_version();
	
	if (debug) {
		printf("- DEBUG mode ON -\r\n");
		log_min_level = _LOG_DEBUG;
	}
	
	if (terminal_active)
		printf("--- Keyboard: use 'q' for quit, 'l' to change log level\r\n");
	
	if (!load_ini_settings(config_filename))
		return EXIT_FAILURE;

	int pid_file = open(settings.pid_file, O_CREAT | O_RDWR, 0666);
	if (!pid_file){
		dlog(_LOG_ERROR, "PID file '%s' error: %s", settings.pid_file, strerror(errno));
		return EXIT_FAILURE;
	}

	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
	if (rc != 0){
		if (EWOULDBLOCK == errno) {
			// another instance is running
			list_args();
			printf("\n");
			dlog(_LOG_TERMINAL, "Lunatic-driver already started");
		} else
			dlog(_LOG_ERROR, "PID lock '%s' error: %s", settings.pid_file, strerror(errno));

		return EXIT_FAILURE;
	}

	// this is the first instance
	char pid_s[16];
	sprintf(pid_s, "%ld\n", (long)getpid());
	write(pid_file, pid_s, strlen(pid_s) + 1);

	if (terminal_active)
		set_terminal_non_canonical();  // need to re-set on crash too
	
	log_post_init();
	
	if (!gpio_init())
		return EXIT_FAILURE;

	debug_init();

	bool result = true;

	if (!console_init())
		result = false;
	
	if (result) {
		if (debug)
			strcpy(settings.midi_portname, "dummy");
		dlog(_LOG_NOTICE, "Open MIDI port: %s", settings.midi_portname);
		result = midi_init(settings.midi_portname);
	}

	if (result) 
		result = start_driver(debug);

	console_done();

	midi_done();

	log_done();

	close(pid_file);
	unlink(settings.pid_file);

	if (!result)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
