#ifndef _IPC_H
#define _IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>


typedef bool(*ipc_callback_c)(char*, uint8_t);  // message, client_id; return true in order to close the socket

bool ipc_init(ipc_callback_c callback, int port, int sleep_secs);
bool ipc_do();
void ipc_done();


bool ipc_debug_connected();
void ipc_set_debug_client(int client_id);
void ipc_send_debug(const char *fmt, ...);
void ipc_debug_clear();

bool ipc_send_string(int client_id, uint8_t *s);
void ipc_send(int client_id, const char *fmt, ...);


#endif