#include "device.h"

void device_init(void) {
    TRISC = 0;  // PortC as output

    TRISC3 = 1; // RC3 as clock input (SCL)
    TRISC4 = 1; // RC4 as data (SDA)

    PORTC = 0; // clear output data latches on port

    //WDT_Initialize();
    //I2C_Initialize();
}

inline void led_on(void) {
    PORTCbits.RC2 = 1;
}

inline void led_off() {
    PORTCbits.RC2 = 0;
}

inline void led_debug_on() {
    #ifdef PROGRAMMER
    led_on();
    #endif
}

inline void led_debug_off() {
    #ifdef PROGRAMMER
    led_off();
    #endif
}
