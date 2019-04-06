#ifndef _UTILS_H
#define _UTILS_H

#include <stdbool.h>

#define UNUSED(expr) do { (void)(expr); } while (0)

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
