#ifndef _CONSOLE_H
#define _CONSOLE_H

bool console_init();
void console_done();

void console_work();

void console_change_log_mode(bool restart_to_debug);

#endif