#include "tests.h"
#include "i2c.h"

void test_i2c(void) {
    uint8_t counter = 1;

    for (;;) {
        CLRWDT(); // clear watchdog

        //led_on();

        I2C_tx(counter);
        counter++;
        if (counter > 90)
            counter = 1;

        __delay_ms(5);
        //led_off();
        //__delay_ms(10);
    }
}
