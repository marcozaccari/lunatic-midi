#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "ipc.h"
#include "../libs/log.h"

ipc_callback_t *ipc_callback = NULL;

#define MAX_SIZE 256

static fd_set savedset;  // descriptor set to be monitored
static int client[FD_SETSIZE]; // array of client descriptors
static int listen_desc; // main listening descriptor and connected descriptor
static int maxfd, maxi; // max value descriptor and index in client array

static int socket_sleep_secs;

static int debug_client = -1;  // current client index using debug

static bool process(int client_id, const char *command);
bool send_string(int client_id, const char *s);


const char* blank_chars = " \t\n\r\f\v";

bool ipc_init(ipc_callback_t *callback, int port, int sleep_secs) {
	struct sockaddr_in serv_addr;
	int i, yes=1;

	ipc_callback = callback;
	
	socket_sleep_secs = sleep_secs;

	listen_desc = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_desc < 0) {
		dlog(_LOG_ERROR, "Failed creating IPC socket on port %d", port);
		return false;
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (setsockopt(listen_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		dlog(_LOG_ERROR, "Failed to setup IPC socket on port %d", port);
		return false;
	}

	if (bind(listen_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		dlog(_LOG_ERROR, "Failed to bind IPC on port %d", port);
		return false;
	}

	listen(listen_desc, 5);

	maxfd = listen_desc; // Initialize the max descriptor with the first valid one we have
	maxi = -1; // index in the client connected descriptor array
	for (i=0; i<FD_SETSIZE; i++)
		client[i] = -1;  // this indicates the entry is available. It will be filled with a valid descriptor

	FD_ZERO(&savedset); // initialize the descriptor set to be monitored to empty
	FD_SET(listen_desc, &savedset); // add the current listening descriptor to the monitored set

	dlog(_LOG_NOTICE, "IPC server started on port %u", port);
	return true;
}

static void close_client(int client_id) {
	if (client_id == debug_client)
		debug_client = -1;
	
	close(client[client_id]);
	FD_CLR(client[client_id], &savedset);
	client[client_id] = -1;
	dlog(_LOG_DEBUG, "IPC client disconnected [%d]", client_id);
}

bool ipc_work() {
	fd_set tempset;
	int conn_desc;
	int numready;
	struct sockaddr_in client_addr;
	char buff[MAX_SIZE];
	char command[STR_MAXSIZE];
	int j,client_idx;

	// assign all currently monitored descriptor set to a local variable. This is needed because select
	// will overwrite this set and we will lose track of what we originally wanted to monitor.
	tempset = savedset;
	struct timeval timeout;
	timeout.tv_sec = socket_sleep_secs;
	timeout.tv_usec = 0;
	
	numready = select(maxfd+1, &tempset, NULL, NULL, &timeout);
	if (numready == -1) {
		if (errno == 4)
			return true;  // catch SIG_INT, SIG_STOP, SIG_TERM
		
		dlog(_LOG_ERROR, "IPC select() error: %d %s", errno, strerror(errno));
		return false;
	}
  
  	//  dlog(_LOG_DEBUG, "IPC select() numready: %d", numready);
	if (numready < 0) 
		return true;

	if (FD_ISSET(listen_desc, &tempset)) { // new client connection
		socklen_t size = sizeof(client_addr);
		conn_desc = accept(listen_desc, (struct sockaddr *)&client_addr, &size);

		char *cs;
		cs = inet_ntoa(client_addr.sin_addr);

		for (j=0; j<FD_SETSIZE; j++) {
			if (client[j] < 0) {
				client[j] = conn_desc; // save the descriptor
				break;
			}
		}

		FD_SET(conn_desc, &savedset); // add new descriptor to set of monitored ones
		if (conn_desc > maxfd)
			maxfd = conn_desc; // max for select
		if (j > maxi)
			maxi = j;   // max used index in client array

		dlog(_LOG_DEBUG, "IPC connected: %s [%d/%d]", cs, j, maxi+1);

		return true;
	}

	for (client_idx=0; client_idx<=maxi; client_idx++) { // check all clients if any received data
		if (client[client_idx] > 0) {
			if (FD_ISSET(client[client_idx], &tempset)) {
				int num_bytes;
				
				if ((num_bytes = read(client[client_idx], buff, MAX_SIZE)) > 0) {
					buff[num_bytes] = '\0';
					
					string_trim(command, buff);
					
					if (strlen(command) > 0) {
						dlog(_LOG_DEBUG, "IPC received: \"%s\"", command);

						if (process(client_idx, command)) {
							ipc_send_string(client_idx, "disconnected\n");
							close_client(client_idx);
						}
					}
				} else if (num_bytes == 0)  // connection was closed by client
					close_client(client_idx);

				if (--numready <=0) // num of monitored descriptors returned by select call
					break; 
			}
		}
	}
   
	return true;
}

void ipc_done(){
	close(listen_desc);
}

bool ipc_send_string(int client_id, const char *s) {
	if (client_id >= FD_SETSIZE) 
		return false;

	return write(client[client_id], s, strlen(s));
}

static bool process(int client_id, const char *command){
	bool ret = false;
	
	if (strcmp(command, "quit") == 0)
		return true;

	if (ipc_callback != NULL){
		ret = ipc_callback(client_id, command);
		if (ret)
			return true;
	}

	return false;
}

void ipc_set_debug_client(int client_id){
	if (client_id < 0)
		client_id = -1;
	
	debug_client = client_id;
}

bool ipc_debug_connected() {
	return (debug_client != -1);
}

void ipc_debug_send_string(const char *s) {
	if (debug_client != -1)
		ipc_send_string(debug_client, s);
}

void ipc_debug_clear(){
	ipc_debug_send_string("\x1b\x1b[2J\x1b\x1b[;H");  // escaping sequence "clear & go home"
	//send_debug("\f");  // form feed
}

void v_ipc_send(int client_id, const char *fmt, va_list args) {
	char s[10240];

	vsprintf(s, fmt, args);
	ipc_send_string(client_id, s);
}

void ipc_send(int client_id, const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	v_ipc_send(client_id, fmt, args);
	va_end(args);
}
	
void v_ipc_send_debug(const char *fmt, va_list args) {
	char s[10240];

	vsprintf(s, fmt, args);
	ipc_debug_send_string(s);
}

void ipc_debug_send(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	v_ipc_send_debug(fmt, args);
	va_end(args);
}
