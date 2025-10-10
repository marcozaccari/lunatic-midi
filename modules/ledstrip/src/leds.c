#include "leds.h"
#include "device.h"
#include <pic16f886.h>

uint8_t leds_state[LEDS_COUNT]; // 0b00000RGB, 0b00000RGB, ...

uint8_t leds_red_value;
uint8_t leds_green_value;
uint8_t leds_blue_value;

inline void leds_update() {
    //led_on();

    // ws2812 reset (above 50 us)
    LEDS_DATA_PIN = 0;
    __delay_us(50);

    for (uint8_t cur_led = 0; cur_led < LEDS_COUNT; cur_led++) {
        uint8_t state = leds_state[LEDS_COUNT-1-cur_led];

        uint8_t red, green, blue;
        
        if (state & 0b001)  // blue
           blue = leds_blue_value;
        else
           blue = 0;

        if (state & 0b010)  // green
           green = leds_green_value;
        else
           green = 0;

        if (state & 0b100)  // red
           red = leds_red_value;
        else
           red = 0;

        if (!(green & (1 << 7))) {
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        }else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(green & (1 << 6)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(green & (1 << 5)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(green & (1 << 4)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(green & (1 << 3)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(green & (1 << 2)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(green & (1 << 1)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(green & (1 << 0)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");


        if (!(red & (1 << 7)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(red & (1 << 6)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(red & (1 << 5)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(red & (1 << 4)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(red & (1 << 3)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(red & (1 << 2)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(red & (1 << 1)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(red & (1 << 0)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        
        if (!(blue & (1 << 7)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(blue & (1 << 6)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(blue & (1 << 5)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(blue & (1 << 4)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(blue & (1 << 3)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(blue & (1 << 2)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(blue & (1 << 1)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");

        if (!(blue & (1 << 0)))
            asm("bsf PORTA, 0\n bcf PORTA, 0\n");
        else
            asm("bsf PORTA, 0\n nop\n nop\n bcf PORTA, 0\n");
    }

    // deactivated to better encourage the next reset
    //PORTAbits.RA0 = 1;

    //led_off();
}

void leds_init(void) {
    LEDS_DATA_PIN_TRIS = 0;  // data pin as output

    leds_reset(1);
    leds_update();
}

// led: 0..60, state: 0b00000RGB
inline void leds_set(uint8_t led, uint8_t rgb) {
    if (led >= LEDS_COUNT)
        return;

    leds_state[led] = rgb;
}

inline void leds_fill(uint8_t rgb) {
    for (uint8_t i = 0; i < LEDS_COUNT; i++)
        leds_state[i] = rgb;
}

inline void leds_reset(uint8_t reset_tuning) {
    for (uint8_t i = 0; i < LEDS_COUNT; i++)
        leds_state[i] = 0;

    if (reset_tuning) {
        leds_red_value = LEDS_DEFAULT_RED_VALUE;
        leds_green_value = LEDS_DEFAULT_RED_VALUE;
        leds_blue_value = LEDS_DEFAULT_RED_VALUE;
    }
}

inline void leds_tune_red(uint8_t red) {
    leds_red_value = (uint8_t)(red << 4) + 0xF;
}

inline void leds_tune_green(uint8_t green) {
    leds_green_value = (uint8_t)(green << 4) + 0xF;
}

inline void leds_tune_blue(uint8_t blue) {
    leds_blue_value = (uint8_t)(blue << 4) + 0xF;
}
