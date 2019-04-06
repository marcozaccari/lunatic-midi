#ifndef _LED_MONITOR_H
#define _LED_MONITOR_H

#include <stdint.h>
#include "../lib/utils.h"

#define LED_OFF 	0
#define LED_RED 	1
#define LED_GREEN	2
#define LED_YELLOW	3
#define LED_BLUE	4
#define LED_VIOLET	5
#define LED_CYAN	6
#define LED_WHITE	7

#define MAX_SEND_BYTES_PER_LOOP 16   // 16*8*2.5 = 320us

bool led_monitor_init();
bool led_monitor_done();

bool led_monitor_do();


void led_monitor_fill(uint8_t color);
void led_monitor_set(unsigned int index, uint8_t color);

#endif
