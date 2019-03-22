#ifndef _UTILS
#define _UTILS

#ifndef bool
    #define bool int
    #define true 1
    #define false 0
#endif


void reset_terminal_mode();
void set_terminal_conio_mode();

int kbhit();

#endif
