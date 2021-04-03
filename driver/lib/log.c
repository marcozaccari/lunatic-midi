#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <syslog.h>

#include "log.h"
#include "utils.h"

#define KCOL_NORMAL  "\x1B[0m"
#define KCOL_GREEN  "\x1B[92m"
#define KCOL_YELLOW  "\x1B[33m"
#define KCOL_RED  "\x1B[91m"
#define KCOL_NAVY  "\x1B[34m"
#define KCOL_BLUE  "\x1B[94m"
#define KCOL_CYAN  "\x1B[36m"
#define KCOL_WHITE  "\x1B[97m"	

FILE* info_log;

static bool initialized = false;
static ino_t inode = 0;

const char loglevel_name[LOGLEVEL_MAX][30] = {
	//"TERMINAL", 
	"OFF", 
	"DEBUG",
	"INFO",
	"NOTICE",
	"WARNING",
	"ERROR"
};

const char loglevel_color[LOGLEVEL_MAX][10] = {
   KCOL_WHITE,  // TERMINAL
	KCOL_BLUE, // DEBUG
	KCOL_CYAN, // INFO
	KCOL_GREEN, // NOTICE
	KCOL_YELLOW, // WARNING
	KCOL_RED// ERROR
};

static const int log2syslog[5] = {LOG_INFO, LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_ALERT};


void v_dlog(loglevel_t level, const char *fmt, va_list args){
	FILE *log_file;
	static time_t ltime;
	struct tm *tm;
	static struct stat mstat;
	bool parsed = false;
	bool should_log;
	static char s[10240];
	static char s2[11240];
	
	should_log = (log_min_level > 0) && (level >= log_min_level);
	
	// get time for timestamp
	ltime = time(NULL);
	tm = localtime(&ltime);

	// log to debug client
	if (should_log && client_log_callback) {
		vsprintf(s, fmt, args);
		parsed = true;

		sprintf(s2, "%s#[%d:%02d:%02d] %s\r\n"KCOL_NORMAL, loglevel_color[level], tm->tm_hour, tm->tm_min, tm->tm_sec, s);

		client_log_callback(s2);
	}
	
	// log to console
	if (terminal_active &&
      (  (level == _LOG_TERMINAL) || 
         should_log
      )
   ) {
		if (!parsed){
			vsprintf(s, fmt, args);
			parsed = true;
		}
		
		printf("%s", loglevel_color[level]);

		if (master_process) printf("(supervisor) ");
		
		if (level != _LOG_TERMINAL)
			printf("[%d:%02d:%02d] ", tm->tm_hour, tm->tm_min, tm->tm_sec);
		
		printf("%s\r\n"KCOL_NORMAL, s);
	}
	
	if (!initialized) return;

	// log to file	or syslog
	if (should_log) {
		if (!parsed) {
			vsprintf(s, fmt, args);
			parsed = true;
		}
		
		if (log_to_syslog){
			syslog(log2syslog[level-1], "%s", s);
		}else if (log_filename[0]){
			if (master_process)
				log_file = fopen(log_filename, "a");
			else
				log_file = info_log;

			if ((stat(log_filename, &mstat) != 0) || (mstat.st_ino != inode)){
				log_file = freopen(log_filename, "a", log_file);
				inode = mstat.st_ino;
			}
			
			fprintf(log_file, "[%d/%02d/%d %d:%02d:%02d] ", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			fprintf(log_file, "%s\n", s);
			
			if (master_process)
				fclose(log_file);
			else {
				fflush(log_file);
			}			
		}
		
	}
}

void dlog(loglevel_t level, const char *fmt, ...){
	va_list args;
	
	va_start(args, fmt);
   v_dlog(level, fmt, args);
	va_end(args);
}

void log_init(){
	terminal_active = isatty(fileno(stdin));  // check if in terminal
	
   log_filename[0] = 0;
   info_log = NULL;
   
	log_min_level = _LOG_NOTICE;
   log_to_syslog = false;
	
	master_process = false;
   
   client_log_callback = NULL;
}

void log_post_init(){
	char tmp[1024];

	get_selfpath(tmp);
	
	if (log_to_syslog){
		openlog(SYSLOG_PROC_NAME, LOG_CONS || LOG_PID, LOG_DAEMON);
		dlog(_LOG_TERMINAL, "Logging on SysLog");
	}else{
	
		if (log_filename[0]) {
			if (log_filename[0] != '/') {
				strcat(tmp, log_filename);
				strcpy(log_filename, tmp);
			}else
				strcpy(tmp, log_filename);

			info_log = fopen(tmp, "a");

			dlog(_LOG_TERMINAL, "Logging on file: %s", tmp);
		}
	}

	dlog(_LOG_TERMINAL, "Logging level: %s", loglevel_name[log_min_level]);
	
	initialized = true;
}

static void log_done_master(){
	if (!log_to_syslog){
		if (log_filename[0])
			fclose(info_log);
	}
}

void log_done(){
	if (log_to_syslog){
		closelog();
	}else{
		if (info_log)
			fclose(info_log);
	}
	
	if (terminal_active)
		printf(KCOL_NORMAL);
}
