#include "device.h"
#include "demo.h"
#include "leds.h"
#include <stdint.h>

uint8_t demo_mode = 1;

inline void demo_stop(void) {
    if (!demo_mode)
        return;
    
    demo_mode = 0;
    leds_reset(1);
    leds_update();
}

uint8_t demo_colors[14] = {1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0};
uint8_t demo_color = 1;
uint8_t demo_color_idx = 0;
uint8_t demo_x = 0;

uint8_t demo_red = 0xFF;
uint8_t demo_green = 0xFF;
uint8_t demo_blue = 0xFF;

#define DEMO_UPDATE_COUNTER 1000
uint16_t demo_counter = 0;

inline void demo_worker(void) {
    if (!demo_mode)
        return;

    //__delay_ms(10);

    //led_toggle();

    if (demo_counter < DEMO_UPDATE_COUNTER) {
        demo_counter++;
        return;
    }
    demo_counter = 0;

    //led_toggle();

    leds_set(demo_x, demo_color % 8);
    leds_update();

    if (demo_color == 0) {
        if ((demo_x % 4) == 0) {
            demo_red++;
            demo_green++;
            demo_blue++;
        }
    } else {
        if ((demo_x % 4) == 0) {
            demo_red--;
            demo_green--;
            demo_blue--;
        }
    }

    leds_tune_red(demo_red);
    leds_tune_green(demo_green);
    leds_tune_blue(demo_blue);

    demo_x++;
    if (demo_x == LEDS_COUNT) {
        demo_x = 0;

        demo_color_idx++;
        if (demo_color_idx == 14)
            demo_color_idx = 0;

        demo_color = demo_colors[demo_color_idx];        

        if (demo_color == 0) {
            demo_red = 0;
            demo_green = 0;
            demo_blue = 0;
        } else {
            demo_red = 0xFF;
            demo_green = 0xFF;
            demo_blue = 0xFF;
        }
    }
}
