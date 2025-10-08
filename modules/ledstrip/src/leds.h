#ifndef LEDS_H
#define	LEDS_H

#include <stdint.h>

#define LEDS_COUNT 60

void leds_init(void);

inline void leds_reset(uint8_t reset_tuning);
inline void leds_set(uint8_t led, uint8_t rgb);

inline void leds_fill(uint8_t rgb);

inline void leds_update(void);

inline void leds_tune_red(uint8_t red);
inline void leds_tune_green(uint8_t green);
inline void leds_tune_blue(uint8_t blue);

#define LEDS_DEFAULT_RED_VALUE   0x0F
#define LEDS_DEFAULT_GREEN_VALUE 0x0F
#define LEDS_DEFAULT_BLUE_VALUE  0x0F

#define LEDS_UPDATE_EVERY_MS 50

#endif

