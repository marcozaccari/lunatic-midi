#ifndef KEYBOARD_H
#define	KEYBOARD_H

#include <xc.h>

void keyboard_init(void);
inline void keyboard_scan(void);

extern uint16_t velocity_max_ms;

#endif
