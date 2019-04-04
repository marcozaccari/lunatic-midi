#ifndef _UTILS
#define _UTILS

#include <stdbool.h>

/*#ifndef bool
    #define bool int
    #define true 1
    #define false 0
#endif*/

#define STR_MAXSIZE 256


void reset_terminal_mode();
void set_terminal_non_canonical();

bool kbhit();

void get_selfpath(char* path);

#endif
