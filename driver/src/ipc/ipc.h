#ifndef _IPC_H
#define _IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

// client_id, command; please return true in order to close the socket
typedef bool ipc_callback_t(int, const char*);

bool ipc_init(ipc_callback_t *callback, int port, int sleep_secs);
bool ipc_do();
void ipc_done();

bool ipc_debug_connected();
void ipc_set_debug_client(int client_id);
void ipc_debug_send_string(const char *s);
void ipc_debug_send(const char *fmt, ...);
void ipc_debug_clear();

bool ipc_send_string(int client_id, const char *s);
void ipc_send(int client_id, const char *fmt, ...);

#endif
