#ifndef _THREADS_H
#define _THREADS_H

bool threads_start();
void threads_stop();

void threads_request_stop();
extern volatile bool threads_terminate_request;

#endif
