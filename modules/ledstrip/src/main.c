#include "main.h"
#include "device.h"
#include "leds.h"
#include "i2c.h"
#include "timers.h"
#include "demo.h"
#include <pic16f886.h>
#include "tests.h"

// ===== INTERRUPT =====
void __interrupt() ISR(void) {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0; // clear interrupt flag
        // Timer
        timers_isr();
        return;
    }

    if (PIR1bits.SSPIF) {
        // I2C
        PIR1bits.SSPIF = 0;  // clear interrupt flag
        I2C_isr();
    }
}

void hello() {
    CLRWDT();

    #ifdef PROGRAMMER_CONNECTED
    demo_stop();

    __delay_ms(20);

    led_on();
    __delay_ms(1000);
    led_off();
    __delay_ms(500);

    CLRWDT();
    led_on();
    __delay_ms(100);
    led_off();
    __delay_ms(300);

    CLRWDT();
    led_on();
    __delay_ms(100);
    led_off();
    __delay_ms(300);

    CLRWDT();
    led_on();
    __delay_ms(100);
    led_off();
    __delay_ms(300);
    led_on();
    #endif
}

uint8_t rx_led_idx = 0;
inline void i2c_rx(void) {
    uint8_t rx_size = I2C_get_rx_buffer_size();
    if (!rx_size)
        return;
    
    demo_stop(); // disable demo at first byte received

    uint8_t rx_byte = I2C_get_rx_byte();

    if (rx_byte == 0xFF) {
        // Reset
        leds_reset(0);
        leds_update();
        rx_led_idx = 0;
        return;
    }

    if (rx_byte == 0xC1) {
        // Repaint request
        leds_update();
        rx_led_idx = 0;
        return;
    }

    if ((rx_byte >= 0xC8) && (rx_byte <= 0xCF)) {
        // Fill leds
        leds_fill(rx_byte - 0xC8);
        return;
    }

    if ((rx_byte >= 0x40) && (rx_byte <= 0x7F)) {
        // Set led index
        rx_led_idx = rx_byte - 0x40;
        return;
    }

    if (rx_byte <= 7) {
        // Set led color
        leds_set(rx_led_idx, rx_byte);
        return;
    }

    if ((rx_byte >= 0x80) && (rx_byte <= 0x8F)) {
        // Tune Red
        leds_tune_red(rx_byte - 0x80);
        return;
    }

    if ((rx_byte >= 0x90) && (rx_byte <= 0x9F)) {
        // Tune Green
        leds_tune_green(rx_byte - 0x90);
        return;
    }

    if ((rx_byte >= 0xA0) && (rx_byte <= 0xAF)) {
        // Tune Blue
        leds_tune_blue(rx_byte - 0xA0);
        return;
    }
}

int main(void) {
    device_init();
    
    I2C_init();
    //timers_init();  // High freq timer cannot be used because the interrupt disrupt the timings of the leds update functions
    leds_init();

    PIR1bits.SSPIF = 0; // Clear I2C interrupt flag
    PIR1bits.TMR2IF = 0; // clear Timer2 interrupt flag
    
    INTCONbits.PEIE = 1; //Enable peripheral interrupt
    INTCONbits.GIE = 1;  // Enable global interrupt

    #ifdef TEST_I2C
    test_i2c(); 
    #endif

    #ifdef TEST_TIMER
    test_timer();
    #endif

    hello();

    // MAIN LOOP
    led_on();
    for (;;) {
        CLRWDT(); // clear watchdog

        //led_toggle();

        i2c_rx();

        demo_worker();
    }
    
    return 0;
};
