#ifndef TIMERS_H
#define	TIMERS_H

#include <xc.h>

extern volatile uint16_t timer_count;

void timers_init(void);
inline void timers_isr(void);
inline uint16_t get_elapsed(uint16_t since);

#endif
