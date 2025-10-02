#include "timers.h"

volatile uint16_t timer_count;

void timers_init(void) {
    // Init internal variables
    timer_count = 0;

    PIE1bits.TMR2IE = 1; // interrupt on Timer2 overflow

    T2CON &= 0b10000100;  // mask scaler bits
    T2CON |= 0b00000011;  // prescaler = 16

    //T2CON |= 0b00000000;   // postscaler = 1 (0.5ms) <-- BEST
    //T2CON |= 0b00001000; // postscaler = 2 (1ms)
    T2CON |= 0b00011000; // postscaler = 4 (2ms)

    // 1/(Fosc/4/prescaler)*PR2*postscaler
    PR2 = 155; // 1ms  <-- BEST
    //PR2 = 75; // 0.5ms
    //PR2 = 100;
    //PR2 = 150;

    T2CONbits.TMR2ON = 1;  // enable timer (counter in TMR2)
}

inline void timers_isr(void) {
    timer_count++;
}

inline uint16_t get_elapsed(uint16_t since) {
    uint16_t now = timer_count;

    if (since <= now)
        return now - since;
    else
        return (0xFFFF - since) + now;
}
