#include "tests.h"
#include "device.h"
#include "i2c.h"
#include "timers.h"

void test_i2c(void) {
    uint8_t counter = 1;

    for (;;) {
        CLRWDT(); // clear watchdog

        //led_on();

        //I2C_tx(counter);
        //counter++;
        //if (counter > 90)
        //    counter = 1;

        I2C_tx(counter);
        counter++;
        if (counter > 90)
            counter = 1;

        __delay_ms(5);

        //__delay_ms(50);

        //led_off();
        //__delay_ms(10);
    }
}

void test_timer(void) {
    uint16_t threshold = 0xFF00;

    uint16_t timestamp = Timer_count;

    for (;;) {
        CLRWDT(); // clear watchdog

        //__delay_ms(10);

        if (get_elapsed(timestamp) >= threshold) {
            timestamp = Timer_count;
            led_toggle();
        }
    }
}
