#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"

#ifndef SYSLOG_PROC_NAME
    #define SYSLOG_PROC_NAME "lunatic-driver"
#endif

typedef enum {
	_LOG_TERMINAL = 0,
	_LOG_DEBUG = 1,
	_LOG_INFO = 2,
	_LOG_NOTICE = 3,    // Syslog
	_LOG_WARNING = 4,   // Syslog
	_LOG_ERROR = 5      // Syslog
} loglevel_t;

#define LOGLEVEL_MAX  6

char log_filename[256];
bool log_to_syslog;
loglevel_t log_min_level;  // default _LOG_NOTICE

typedef void log_callback(char*);
log_callback *client_log_callback;

bool terminal_active;    // if running in console (no service) or by 'debug' parameter
bool master_process;

extern const char loglevel_name[LOGLEVEL_MAX][30];
extern const char loglevel_color[LOGLEVEL_MAX][10];

loglevel_t log_debug_min_level;

void dlog(loglevel_t level, const char *fmt, ...);
void v_dlog(loglevel_t level, const char *fmt, va_list args);

void log_init();
void log_post_init();
void log_done();

#endif
