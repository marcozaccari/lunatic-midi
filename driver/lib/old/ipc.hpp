#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

namespace ipc {

typedef bool(*ipc_callback_t)(string, int);  // message, client_id; return true in order to close the socket
typedef bool(*ipc_callback_c)(char*, uint8_t);  // C 

bool init(ipc_callback_t callback, int port, int sleep_secs);
bool socket_do();
void done();

bool debug_connected();
void set_debug_client(int client_id);
void send_debug(string s);
void debug_clear();

}