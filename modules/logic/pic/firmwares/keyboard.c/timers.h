#ifndef TIMERS_H
#define	TIMERS_H

#include <xc.h>

// Global timer counter, 1KHz
extern volatile uint16_t Timer_count;

void timers_init(void);
inline void timers_isr(void);

// Get elapsed milliseconds since timestamp
inline uint16_t get_elapsed(uint16_t since);

#endif
