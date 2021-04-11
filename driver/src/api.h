#ifndef _API_H
#define _API_H

bool api_init();

void* api_thread_start();
void api_thread_stop();

extern volatile bool api_thread_terminated;

#endif
