#include <iostream>
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
#include <vector>

#include "ipc.hpp"

extern "C" {
    //#include "utils.h"
    #include "log.h"
}

using namespace std;

namespace ipc {

ipc_callback_t ipc_callback_cpp = NULL;
ipc_callback_c ipc_callback = NULL;

#define MAX_SIZE 256

static fd_set savedset;  // descriptor set to be monitored
static int client[FD_SETSIZE]; // array of client descriptors
static int listen_desc; // main listening descriptor and connected descriptor
static int maxfd, maxi; // max value descriptor and index in client array

static int socket_sleep_secs;

static int debug_client = -1;  // current client index using debug

bool process(string command, int client_id);
bool send_string(int client_id, string s);


const char* blank_chars = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = blank_chars) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}
// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = blank_chars) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}
// trim from both ends of string (right then left)
inline std::string& trim(std::string& s, const char* t = blank_chars) {
    return ltrim(rtrim(s, t), t);
}

bool init(ipc_callback_t callback, int port, int sleep_secs) {
    struct sockaddr_in serv_addr;
    int i, yes=1;

    ipc_callback_cpp = callback;
    
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

void close_client(int client_id) {
    if (client_id == debug_client)
        debug_client = -1;
    
    close(client[client_id]);
    FD_CLR(client[client_id], &savedset);
    client[client_id] = -1;
    dlog(_LOG_DEBUG, "IPC client disconnected [%d]", client_id);
}

bool socket_do() {
    fd_set tempset;
    int conn_desc;
    int numready;
    struct sockaddr_in client_addr;
    char buff[MAX_SIZE];
    int j,k;

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

    for (k=0; k<=maxi; k++) { // check all clients if any received data
        if (client[k] > 0) {
            if (FD_ISSET(client[k], &tempset)) {
                int num_bytes;
                
                if ((num_bytes = read(client[k], buff, MAX_SIZE)) > 0) {
                    buff[num_bytes] = '\0';
                    string s = buff;
                    s = rtrim(s);
                    if (s.length()) {
                        dlog(_LOG_DEBUG, "IPC received (%u): \"%s\"", s.length(), s.c_str());

                        if (process(s, k)) {
                            send_string(k, "disconnected\n");
                            close_client(k);
                        }
                    }
                } else if (num_bytes == 0)  // connection was closed by client
                    close_client(k);

                if (--numready <=0) // num of monitored descriptors returned by select call
                    break; 
            }
        }
    }
   
    return true;
}

void done(){
    close(listen_desc);
}

bool send_string(int client_id, string s) {
    if (client_id >= FD_SETSIZE) 
        return false;

    std::vector<char> v(s.begin(), s.end());
    return write(client[client_id], &v[0], v.size());
}

bool process(string command, int client_id){
    bool ret = false;
    
    if (command == "quit")
        return true;

    if (ipc_callback_cpp != NULL){
        ret = ipc_callback_cpp(command, client_id);
        if (ret)
            return true;
    }

    if (ipc_callback != NULL){
        ret = ipc_callback((char*)command.c_str(), (uint8_t)client_id);
        if (ret)
            return true;
    }
    
    return false;
}

void set_debug_client(int client_id){
    if (client_id < 0)
        client_id = -1;
    
    debug_client = client_id;
}

bool debug_connected() {
    return (debug_client != -1);
}

void send_debug(string str){
    if (debug_client != -1)
        send_string(debug_client, str);
}

void debug_clear(){
    send_debug("\x1b\x1b[2J\x1b\x1b[;H");  // escaping sequence "clear & go home"
    //send_debug("\f");  // form feed
}

}

extern "C" {
    #include "ipc.h"

    bool ipc_init(ipc_callback_c callback, int port, int sleep_secs){
        ipc::ipc_callback = callback;
        return ipc::init(NULL, port, sleep_secs);
    }

    bool ipc_do(){
        return ipc::socket_do();
    }
	
    void ipc_done() {
        ipc::done();
    }
	
    bool ipc_send_string(int client_id, uint8_t *s) {
        string str = (char*)s;
        return ipc::send_string(client_id, str);
    }
	
    void v_ipc_send(int client_id, const char *fmt, va_list args) {
        char s[10240];
        string str;

        vsprintf(s, fmt, args);
        str = (char*)s;
        ipc::send_string(client_id, str);
    }

    void ipc_send(int client_id, const char *fmt, ...) {
        va_list args;

        va_start(args, fmt);
        v_ipc_send(client_id, fmt, args);
        va_end(args);
    }
	
    void ipc_set_debug_client(int client_id) {
        ipc::set_debug_client(client_id);
    }

    void v_ipc_send_debug(const char *fmt, va_list args) {
        char s[10240];
        string str;

        vsprintf(s, fmt, args);
        str = (char*)s;
        ipc::send_debug(str);
    }

    void ipc_send_debug(const char *fmt, ...) {
        va_list args;

        va_start(args, fmt);
        v_ipc_send_debug(fmt, args);
        va_end(args);
    }

    void ipc_debug_clear() {
        ipc::debug_clear();
    }

    bool ipc_debug_connected() {
        return ipc::debug_connected();
    }

}