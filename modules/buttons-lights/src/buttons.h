#ifndef BUTTONS_H
#define	BUTTONS_H

#include <xc.h>

void buttons_init(void);
inline void buttons_worker(void); // must be called at every main cycle

// A0..A3 = bank selector (0..15)
#define BUTTONS_BANK_SELECTOR_PIN_0       RA0
#define BUTTONS_BANK_SELECTOR_PIN_0_TRIS  TRISA0
#define BUTTONS_BANK_SELECTOR_PIN_1       RA1
#define BUTTONS_BANK_SELECTOR_PIN_1_TRIS  TRISA1
#define BUTTONS_BANK_SELECTOR_PIN_2       RA2
#define BUTTONS_BANK_SELECTOR_PIN_2_TRIS  TRISA2
#define BUTTONS_BANK_SELECTOR_PIN_3       RA3
#define BUTTONS_BANK_SELECTOR_PIN_3_TRIS  TRISA3

// A4 = buttons / leds selector 
#define BUTTONS_LED_SELECTOR_PIN          RA4
#define BUTTONS_LED_SELECTOR_PIN_TRIS     TRISA4

// A5 = leds latch (off/on - default)
#define BUTTONS_LEDS_LATCH_PIN            RA5
#define BUTTONS_LEDS_LATCH_PIN_TRIS       TRISA5

#endif
