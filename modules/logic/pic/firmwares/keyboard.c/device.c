#include "device.h"
#include <pic16f886.h>

void init_watchdog() {
    INTCONbits.T0IE = 0; // disable timer0 interrupt

    CLRWDT(); // clear watchdog
    TMR0 = 0; // clear TMR0 and prescaler
    
    OPTION_REGbits.PSA = 1; // select WDT
    uint8_t presc = 0b11111000;
    OPTION_REG &= presc; // mask prescaler bits
    presc |= 0b00000111; // 1:128 prescaler ≈2s
    OPTION_REG = presc;
    
    CLRWDT();
}

void device_init(void) {
    // PORT A
    ANSEL = 0;  // digital I/O (PORTA)

    //TRISA = 0;  // PortA as output
    TRISA5 = 1; // Port A-5 as input (bridge flag)

    // PORT B
    ANSELH = 0; // digital I/O (PORTB)

    // PORT C
    TRISC = 0;  // PortC as output

    TRISC3 = 1; // RC3 as clock input (SCL)
    TRISC4 = 1; // RC4 as data (SDA)

    PORTC = 0; // clear output data latches on port

    // Watchdog
    init_watchdog();
}

inline void led_on(void) {
    PORTCbits.RC2 = 1;
}

inline void led_off() {
    PORTCbits.RC2 = 0;
}

volatile uint8_t led_state = 0;
inline void led_toggle() {
    if (led_state) {
        led_off();
        led_state = 0;
    } else {
        led_on();
        led_state = 1;
    }
}
