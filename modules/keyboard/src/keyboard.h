#ifndef KEYBOARD_H
#define	KEYBOARD_H

#include <xc.h>

/*******************************************************************************
 * A1..A4 = bank selector (0..7, A4 always 0)
 * A0     = key switch selector for velocity measure (0 = key press start, 1 = key press end)
 * B0..B7 = bank keys state (0 = switch start, 1 = switch end)
 */

// A1..A4 = bank selector (0..7, A4 always 0)
#define KEYS_BANK_SELECTOR_PIN_0       RA1
#define KEYS_BANK_SELECTOR_PIN_0_TRIS  TRISA1
#define KEYS_BANK_SELECTOR_PIN_1       RA2
#define KEYS_BANK_SELECTOR_PIN_1_TRIS  TRISA2
#define KEYS_BANK_SELECTOR_PIN_2       RA3
#define KEYS_BANK_SELECTOR_PIN_2_TRIS  TRISA3
#define KEYS_BANK_SELECTOR_PIN_3       RA4
#define KEYS_BANK_SELECTOR_PIN_3_TRIS  TRISA4

void keyboard_init(void);
inline void keyboard_scan(void);

#endif
