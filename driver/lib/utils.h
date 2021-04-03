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


/* Usage:
 * printf("blabla "INT_TO_BINARY_PATTERN" blabla", INT_TO_BINARY(value));
 */
#define INT_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define INT_TO_BINARY(value)  \
  (value & 0x8000 ? '1' : '0'), \
  (value & 0x4000 ? '1' : '0'), \
  (value & 0x2000 ? '1' : '0'), \
  (value & 0x1000 ? '1' : '0'), \
  (value & 0x0800 ? '1' : '0'), \
  (value & 0x0400 ? '1' : '0'), \
  (value & 0x0200 ? '1' : '0'), \
  (value & 0x0100 ? '1' : '0'), \
  (value & 0x0080 ? '1' : '0'), \
  (value & 0x0040 ? '1' : '0'), \
  (value & 0x0020 ? '1' : '0'), \
  (value & 0x0010 ? '1' : '0'), \
  (value & 0x0008 ? '1' : '0'), \
  (value & 0x0004 ? '1' : '0'), \
  (value & 0x0002 ? '1' : '0'), \
  (value & 0x0001 ? '1' : '0') 



void reset_terminal_mode();
void set_terminal_non_canonical();

bool kbhit();

void get_selfpath(char* path);

#endif
