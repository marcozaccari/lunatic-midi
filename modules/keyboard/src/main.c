#include "main.h"
#include "device.h"
#include "keyboard.h"
#include "i2c.h"
#include "timers.h"
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

inline void i2c_rx(void) {
    uint8_t rx_size = I2C_get_rx_buffer_size();
    if (!rx_size)
        return;

    uint8_t rx_byte = I2C_get_rx_byte();

    if (rx_byte == 0xFF) {
        // Reset
        I2C_reset();

        // Send firmware version
        uint8_t size = sizeof(VERSION);
        for (uint8_t i = 0; i < size; i++)
            I2C_tx(VERSION[i]);
        return;
    }
}

void hello() {
    __delay_ms(50);

    CLRWDT();
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
}

int main(void) {
    device_init();
    
    I2C_init();
    timers_init();
    keyboard_init();

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
        keyboard_scan();
    }
    
    return 0;
};
