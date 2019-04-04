#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#include "log.h"
#include "exceptions.h"

#if defined(__i386__) // gcc specific
	#define BITS		32
#elif defined(__x86_64__) // gcc specific
	#define BITS		64
#elif defined(__arm__) // raspberry
	#define BITS		32
#else
	#error Unsupported architecture.
#endif

#define PTR_PAD		(BITS >> 2)
#define PIPE_BUFSIZE	2048
#define MAX_FRAMES	50 // NOTE: must be less than 64 on windows systems.
#define MAX_STRLEN	64 // Maximum characters when dumping string-type parameters.

#define USE_ALTSTACK
static char alternate_stack[MINSIGSTKSZ];

static char pipe_buffer[PIPE_BUFSIZE];
static char* header = "\n--- unhandled exception ---\n";
static char* footer = "\n---------------------------\n";

typedef struct {
	unsigned long     uc_flags;
	struct ucontext   *uc_link;
	stack_t           uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t          uc_sigmask;
} sig_ucontext_t;

void err(int sig_num) {
	fprintf(stderr, "error setting signal handler for %d (%s)\n", sig_num, strsignal(sig_num));
	fflush(stderr);
	exit(EXIT_FAILURE);
}

void handler(int sig_num, siginfo_t* info, void* ucontext) {
	//utils_done();
	
	FILE* fp;
	char* filename = (log_filename[0] == 0) ? "crashdump" : log_filename;

	fp = fopen(filename, "a");
	fputs(header, fp);

	time_t t = time(NULL);
	struct tm* time = localtime(&t);
	fprintf(fp, "%d/%02d/%02d %02d:%02d:%02d\n", 
				time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

	/* Get the address at the time the signal was raised */
	sig_ucontext_t* uc = (sig_ucontext_t *) ucontext;
	void* caller_address = NULL;
	#if defined(__i386__) // gcc specific
		caller_address = (void *) uc->uc_mcontext.eip; // EIP: x86 specific
	#elif defined(__x86_64__) // gcc specific
		caller_address = (void *) uc->uc_mcontext.rip; // RIP: x86_64 specific
	#elif defined(__arm__)
      # https://answers.launchpad.net/gcc-arm-embedded/+question/248296
		caller_address = (void *) uc->uc_mcontext.arm_pc; // uc->uc_mcontext.arm_pc
	#else
		#error Unsupported architecture.
	#endif

	fprintf(fp, "signal %d (%s), at address %p, from %p\n\nstack backtrace:\n",
			sig_num, strsignal(sig_num), info->si_addr, (void*)caller_address);

	fflush(fp);
	fclose(fp);

	// do backtrace using gdb to get full symbol resolution.
	sprintf(pipe_buffer, "gdb --pid=%d -q -batch -ex=\"bt %d\" -ex=\"detach\" -ex=\"q\" >> %s", 
			  getpid(), MAX_FRAMES, filename);
	system(pipe_buffer);
	
	fp = fopen(filename, "a");
	fputs(footer, fp);
	fflush(fp);
	fclose(fp);

	exit(EXIT_FAILURE);
}

void exceptions_init() {
	struct sigaction sig_action;
	sig_action.sa_flags = SA_SIGINFO;
	//sig_action.sa_handler = (void(*)(int))handler;
	sig_action.sa_handler = (void*)handler;

	#ifdef USE_ALTSTACK
	stack_t ss = {};
	ss.ss_sp = (void*)alternate_stack;
	ss.ss_size = SIGSTKSZ;
	ss.ss_flags = 0;
	
	if (sigaltstack(&ss, NULL) != 0) {
		fprintf(stderr, "can't set alternate stack for exception handling.\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	sig_action.sa_flags |= SA_ONSTACK;

	sigfillset(&sig_action.sa_mask);
	#endif
	
	if (sigaction(SIGSEGV, &sig_action, NULL) != 0) { err(SIGSEGV); }
	if (sigaction(SIGSTKFLT, &sig_action, NULL) != 0) { err(SIGSTKFLT); }
	if (sigaction(SIGFPE,  &sig_action, NULL) != 0) { err(SIGFPE); }
	//if (sigaction(SIGINT,  &sig_action, NULL) != 0) { err(SIGINT); }
	if (sigaction(SIGILL,  &sig_action, NULL) != 0) { err(SIGILL); }
	//if (sigaction(SIGTERM, &sig_action, NULL) != 0) { err(SIGTERM); }
	if (sigaction(SIGABRT, &sig_action, NULL) != 0) { err(SIGABRT); }
}
